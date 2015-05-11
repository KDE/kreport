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
#include <QPluginLoader>
#include <QFileInfo>

//Include the static items
#include "../items/label/KoReportLabelPlugin.h"
#include "../items/check/KoReportCheckPlugin.h"
#include "../items/field/KoReportFieldPlugin.h"
#include "../items/image/KoReportImagePlugin.h"
#include "../items/text/KoReportTextPlugin.h"

#define KREPORT_ADD_BUILTIN_PLUGIN(name) \
    addBuiltInPlugin<name>(KREPORT_STATIC_PLUGIN_METADATA(name))

KoReportPluginManager::Private::Private(KoReportPluginManager *qq)
    : q(qq), m_parent(new QObject)
{
    KREPORT_ADD_BUILTIN_PLUGIN(KoReportLabelPlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KoReportCheckPlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KoReportFieldPlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KoReportImagePlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KoReportTextPlugin);

    findPlugins();
}

// ---

KReportPluginEntry::KReportPluginEntry()
    : loader(0), m_interface(0), m_metaData(0)
{
}

KReportPluginEntry::KReportPluginEntry(KoReportPluginInterface *staticInterface)
    : loader(0), m_interface(staticInterface), m_metaData(0)
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
    if (!loader) {
        qWarning() << "No such plugin";
        return 0;
    }
    if (!loader->load()) {
        qWarning() << "Could not load plugin" << loader->fileName();
        return 0;
    }
    KPluginFactory *factory = qobject_cast<KPluginFactory*>(loader->instance());
    if (!factory) {
        qWarning() << "Could not factory for plugin" << loader->fileName();
        return 0;
    }
    m_interface = factory->create<KoReportPluginInterface>();
    if (!m_interface) {
        qWarning() << "Could not create instance of plugin" << loader->fileName();
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
    entry->setMetaData(new KReportPluginMetaData(j));
    entry->setBuiltIn(true);
    if (entry->metaData()->pluginId().isEmpty()) {
        qWarning() << "Plugin" << entry->metaData()->name() << "has no identifier so won't be added to manager";
        delete entry;
        return;
    }
    entry->setStatic(true);
    plugins.insert(entry->metaData()->pluginId(), entry);
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

void KoReportPluginManager::Private::findPlugins()
{
    //qDebug() << "Load all plugins";
    const QList<QPluginLoader*> offers = KReportJsonTrader::self()->query(QLatin1String("KReport/Element"));
    foreach(QPluginLoader *loader, offers) {
        //QJsonObject json = loader->metaData();
        //qDebug() << json;
        //! @todo check version
        KReportPluginEntry *entry = new KReportPluginEntry;
        entry->loader = loader;
        entry->setMetaData(new KReportPluginMetaData(*loader));
        plugins.insert(entry->metaData()->pluginId(), entry);
    }
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

QStringList KoReportPluginManager::pluginIds() const
{
    return d->plugins.keys();
}

const KReportPluginMetaData *KoReportPluginManager::pluginMetaData(const QString& id) const
{
    KReportPluginEntry *entry = d->plugins.value(id);
    if (!entry) {
        return 0;
    }
    return entry->metaData();
}

KoReportPluginInterface* KoReportPluginManager::plugin(const QString& id) const
{
    KReportPluginEntry *entry = d->plugins.value(id);
    if (!entry) {
        return 0;
    }
    return entry->plugin();
}

QList<QAction*> KoReportPluginManager::actions()
{
    QList<QAction*> actList;
#if 0 //todo
    KoReportDesigner designer;
    const QMap<QString, KoReportPluginInterface*> plugins = d->plugins;

    foreach(KoReportPluginInterface* plugin, plugins) {
        KReportPluginMetaData *info = plugin->info();
        if (info) {
            KToggleAction *act = new KToggleAction(KIcon(info->icon()), info->name(), this);
            act->setObjectName(QLatin1String(info->className()));

            //Store the order priority in the user data field
            act->setData(info->priority());
            actList << act;
        }
    }
#endif
    return actList;
}
