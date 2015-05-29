/* This file is part of the KDE project
   Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)
   Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KoReportPluginManager.h"
#include "KoReportPluginManagerPrivate.h"
#include "KReportPluginMetaData.h"
#include "KReportJsonTrader_p.h"

#include <QDebug>
#include <QAction>

//Include the static items
#include "../items/label/KoReportLabelPlugin.h"
#include "../items/check/KoReportCheckPlugin.h"
#include "../items/field/KoReportFieldPlugin.h"
#include "../items/image/KoReportImagePlugin.h"
#include "../items/text/KoReportTextPlugin.h"

KoReportPluginManager::Private::Private(KoReportPluginManager *qq)
    : q(qq), m_parent(new QObject), m_findPlugins(true)
{
}

// ---

KReportPluginEntry::KReportPluginEntry()
    : m_loader(0), m_interface(0), m_metaData(0)
{
}

KReportPluginEntry::KReportPluginEntry(KoReportPluginInterface *staticInterface)
    : m_loader(0), m_interface(staticInterface), m_metaData(0)
{
}

KReportPluginEntry::~KReportPluginEntry()
{
    delete m_metaData;
    delete m_interface;
}

KoReportPluginInterface* KReportPluginEntry::plugin()
{
    if (m_interface) {
        return m_interface;
    }
    if (!m_loader) {
        qWarning() << "No such plugin";
        return 0;
    }
    if (!m_loader->load()) {
        qWarning() << "Could not load plugin" << m_loader->fileName();
        return 0;
    }
    KPluginFactory *factory = qobject_cast<KPluginFactory*>(m_loader->instance());
    if (!factory) {
        qWarning() << "Could not create factory for plugin" << m_loader->fileName();
        return 0;
    }
    m_interface = factory->create<KoReportPluginInterface>();
    if (!m_interface) {
        qWarning() << "Could not create instance of plugin" << m_loader->fileName();
        return 0;
    }
    m_interface->setMetaData(m_metaData);
    return m_interface;
}

void KReportPluginEntry::setBuiltIn(bool set)
{
    m_metaData->setBuiltIn(set);
}

void KReportPluginEntry::setStatic(bool set)
{
    m_metaData->setStatic(set);
}

const KReportPluginMetaData *KReportPluginEntry::metaData() const
{
    return m_metaData;
}

void KReportPluginEntry::setMetaData(KReportPluginMetaData *metaData)
{
    delete m_metaData;
    m_metaData = metaData;
    if (m_interface) {
        m_interface->setMetaData(m_metaData);
    }
}

void KReportPluginEntry::setMetaData(const QJsonObject &metaData)
{
    setMetaData(new KReportPluginMetaData(metaData));
}

void KReportPluginEntry::setMetaData(QPluginLoader *loader)
{
    m_loader = loader;
    setMetaData(new KReportPluginMetaData(*m_loader));
}

// ---

KoReportPluginManager::Private::~Private()
{
    delete m_parent;
}

template<class PluginClass>
void KoReportPluginManager::Private::addBuiltInPlugin(const QJsonObject &json)
{
    KReportPluginEntry *entry = new KReportPluginEntry(new PluginClass(m_parent));
    QJsonObject j = json.value(QLatin1String("MetaData")).toObject();
    qDebug() << j;
    entry->setMetaData(j);
    entry->setBuiltIn(true);
    if (entry->metaData()->id().isEmpty()) {
        qWarning() << "Plugin" << entry->metaData()->name() << "has no identifier so won't be added to manager";
        delete entry;
        return;
    }
    entry->setStatic(true);
    m_plugins.insert(entry->metaData()->id(), entry);
}

#if 0
//! Typedef of plugin version matching the one exported by KEXI_EXPORT_PLUGIN.
typedef quint32 (*plugin_version_t)();

loadPlugin(KService::Ptr service)
{
    if (service.isNull()) {
        qWarning() << "No service specified";
        return;
    }
    qDebug() << "library:" << service->library();
    QPluginLoader loader(service->library());
    QLibrary lib(loader.fileName());
    plugin_version_t plugin_version_function = (plugin_version_t)lib.resolve("plugin_version");
    if (!plugin_version_function) {
        qWarning() << "Plugin version not found for" << service->name();
        return;
    }
    quint32 foundVersion = plugin_version_function();
    qDebug() << "foundVersion:" << d->foundVersion;
    QPointer<QPluginFactory> factory = loader.factory();
    if (!d->factory) {
        qWarning() << "Failed to create instance of factory for plugin" << ptr->name();
        return;
    }
    QString pluginName;
    if (!pluginNameProperty.isEmpty()) {
        pluginName = ptr->property(pluginNameProperty).toString();
    }

    if (!factory())
        return 0;
    KoReportPluginInterface* plugin = factory()->create<KoReportPluginInterface>(parent);
    if (plugin) {
        plugin->setObjectName(pluginName());
    }
    return plugin;
#endif

#define KREPORT_ADD_BUILTIN_PLUGIN(name) \
    addBuiltInPlugin<name>(KREPORT_STATIC_PLUGIN_METADATA(name))

QMap<QString, KReportPluginEntry*>* KoReportPluginManager::Private::plugins()
{
    if (!m_findPlugins) {
        return &m_plugins;
    }
    m_findPlugins = false;

    KREPORT_ADD_BUILTIN_PLUGIN(KoReportLabelPlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KoReportCheckPlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KoReportFieldPlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KoReportImagePlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KoReportTextPlugin);

    //qDebug() << "Load all plugins";
    const QList<QPluginLoader*> offers = KReportJsonTrader::self()->query(QLatin1String("KReport/Element"));
    foreach(QPluginLoader *loader, offers) {
        //QJsonObject json = loader->metaData();
        //qDebug() << json;
        //! @todo check version
        KReportPluginEntry *entry = new KReportPluginEntry;
        entry->setMetaData(loader);
        m_plugins.insert(entry->metaData()->id(), entry);
    }
    return &m_plugins;
}

// ---

//! Class for access to KoReportPluginManager constructor
class KReportPluginManagerSingleton
{
public:
    KoReportPluginManager object;
};


KoReportPluginManager::KoReportPluginManager()
    : d(new Private(this))
{
}

KoReportPluginManager::~KoReportPluginManager()
{
    delete d;
}

Q_GLOBAL_STATIC(KReportPluginManagerSingleton, s_self)

KoReportPluginManager* KoReportPluginManager::self()
{
    return &s_self->object;
}

QStringList KoReportPluginManager::pluginIds()
{
    return d->plugins()->keys();
}

const KReportPluginMetaData *KoReportPluginManager::pluginMetaData(const QString& id)
{
    KReportPluginEntry *entry = d->plugins()->value(id);
    if (!entry) {
        return 0;
    }
    return entry->metaData();
}

KoReportPluginInterface* KoReportPluginManager::plugin(const QString& id)
{
    QString i = id;
    if (!id.startsWith(QLatin1String("org.kde.kreport"))) {
        i = QLatin1String("org.kde.kreport.") + id;
    }

    KReportPluginEntry *entry = d->plugins()->value(i);
    if (!entry) {
        return 0;
    }
    return entry->plugin();
}

QList<QAction*> KoReportPluginManager::actions()
{
    const QMap<QString, KReportPluginEntry*> *plugins = d->plugins();
    QList<QAction*> actList;
    foreach(KReportPluginEntry* plugin, *plugins) {
        const KReportPluginMetaData *metaData = plugin->metaData();
        if (metaData) {
            QAction *act = new QAction(QIcon::fromTheme(metaData->iconName()), metaData->name(), this);
            act->setObjectName(metaData->id());
            act->setCheckable(true);

            //Store the order priority in the user data field
            act->setData(metaData->priority());
            actList << act;
        }
    }
    return actList;
}
