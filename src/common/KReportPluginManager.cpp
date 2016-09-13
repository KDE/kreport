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

#include "KReportPluginManager.h"
#include "KReportPluginManagerPrivate.h"
#include "KReportPluginMetaData.h"
#include "KReportJsonTrader_p.h"

#include "kreport_debug.h"
#include <QAction>

//Include the static items
#include "../items/label/KReportLabelPlugin.h"
#include "../items/check/KReportCheckBoxPlugin.h"
#include "../items/field/KReportFieldPlugin.h"
#include "../items/image/KReportImagePlugin.h"
#include "../items/text/KReportTextPlugin.h"

KReportPluginManager::Private::Private(KReportPluginManager *qq)
    : q(qq), m_parent(new QObject), m_findPlugins(true)
{
    Q_UNUSED(q);
}

// ---

KReportPluginEntry::KReportPluginEntry()
    : m_loader(0), m_interface(0), m_metaData(0)
{
}

KReportPluginEntry::KReportPluginEntry(KReportPluginInterface *staticInterface)
    : m_loader(0), m_interface(staticInterface), m_metaData(0)
{
}

KReportPluginEntry::~KReportPluginEntry()
{
    delete m_metaData;
    delete m_interface;
}

KReportPluginInterface* KReportPluginEntry::plugin()
{
    if (m_interface) {
        return m_interface;
    }
    if (!m_loader) {
        kreportWarning() << "No such plugin";
        return 0;
    }
    if (!m_loader->load()) {
        kreportWarning() << "Could not load plugin" << m_loader->fileName();
        return 0;
    }
    KPluginFactory *factory = qobject_cast<KPluginFactory*>(m_loader->instance());
    if (!factory) {
        kreportWarning() << "Could not create factory for plugin" << m_loader->fileName();
        return 0;
    }
    m_interface = factory->create<KReportPluginInterface>();
    if (!m_interface) {
        kreportWarning() << "Could not create instance of plugin" << m_loader->fileName();
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

KReportPluginManager::Private::~Private()
{
    delete m_parent;
}

template<class PluginClass>
void KReportPluginManager::Private::addBuiltInPlugin(const QJsonObject &json)
{
    KReportPluginEntry *entry = new KReportPluginEntry(new PluginClass(m_parent));
    QJsonObject j = json.value(QLatin1String("MetaData")).toObject();
    //kreportDebug() << j;
    entry->setMetaData(j);
    entry->setBuiltIn(true);
    if (entry->metaData()->id().isEmpty()) {
        kreportWarning() << "Plugin" << entry->metaData()->name() << "has no identifier so won't be added to manager";
        delete entry;
        return;
    }
    entry->setStatic(true);
    m_plugins.insert(entry->metaData()->id(), entry);
    m_pluginsByLegacyName.insert(entry->metaData()->value(QLatin1String("X-KDE-PluginInfo-LegacyName"), entry->metaData()->id()), entry);
}

#if 0
//! Typedef of plugin version matching the one exported by KEXI_EXPORT_PLUGIN.
typedef quint32 (*plugin_version_t)();

bool loadPlugin(KService::Ptr service)
{
    if (service.isNull()) {
        kreportWarning() << "No service specified";
        return;
    }
    kreportDebug() << "library:" << service->library();
    QPluginLoader loader(service->library());
    QLibrary lib(loader.fileName());
    plugin_version_t plugin_version_function = (plugin_version_t)lib.resolve("plugin_version");
    if (!plugin_version_function) {
        kreportWarning() << "Plugin version not found for" << service->name();
        return;
    }
    quint32 foundVersion = plugin_version_function();
    kreportDebug() << "foundVersion:" << d->foundVersion;
    QPointer<QPluginFactory> factory = loader.factory();
    if (!d->factory) {
        kreportWarning() << "Failed to create instance of factory for plugin" << ptr->name();
        return;
    }
    QString pluginName;
    if (!pluginNameProperty.isEmpty()) {
        pluginName = ptr->property(pluginNameProperty).toString();
    }

    if (!factory())
        return 0;
    KReportPluginInterface* plugin = factory()->create<KReportPluginInterface>(parent);
    if (plugin) {
        plugin->setObjectName(pluginName());
    }
    return plugin;
}
#endif

#define KREPORT_ADD_BUILTIN_PLUGIN(name) \
    addBuiltInPlugin<name>(KREPORT_STATIC_PLUGIN_METADATA(name))

QMap<QString, KReportPluginEntry*>* KReportPluginManager::Private::plugins()
{
    if (m_findPlugins) {
        findPlugins();
    }
    return &m_plugins;
}

QMap<QString, KReportPluginEntry*>* KReportPluginManager::Private::pluginsByLegacyName()
{
    if (m_findPlugins) {
        findPlugins();
    }
    return &m_pluginsByLegacyName;
}

void KReportPluginManager::Private::findPlugins()
{
    KREPORT_ADD_BUILTIN_PLUGIN(KReportLabelPlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KReportCheckBoxPlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KReportFieldPlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KReportImagePlugin);
    KREPORT_ADD_BUILTIN_PLUGIN(KReportTextPlugin);

    //kreportDebug() << "Load all plugins";
    const QList<QPluginLoader*> offers = KReportJsonTrader::self()->query(QLatin1String("KReport/Element"));
    foreach(QPluginLoader *loader, offers) {
        //QJsonObject json = loader->metaData();
        //kreportDebug() << json;
        //! @todo check version
        KReportPluginEntry *entry = new KReportPluginEntry;
        entry->setMetaData(loader);
        m_plugins.insert(entry->metaData()->id(), entry);
        if (entry->metaData()->id().startsWith(QLatin1String("org.kde.kreport"))) {
            m_pluginsByLegacyName.insert(entry->metaData()->value(QLatin1String("X-KDE-PluginInfo-LegacyName"), entry->metaData()->id()), entry);
        }
    }
    m_findPlugins = false;
}

// ---

//! Class for access to KReportPluginManager constructor
class KReportPluginManagerSingleton
{
public:
    KReportPluginManager object;
};


KReportPluginManager::KReportPluginManager()
    : d(new Private(this))
{
}

KReportPluginManager::~KReportPluginManager()
{
    delete d;
}

Q_GLOBAL_STATIC(KReportPluginManagerSingleton, s_self)

KReportPluginManager* KReportPluginManager::self()
{
    return &s_self->object;
}

QStringList KReportPluginManager::pluginIds() const
{
    return d->plugins()->keys();
}

const KReportPluginMetaData *KReportPluginManager::pluginMetaData(const QString& id) const
{
    KReportPluginEntry *entry = d->plugins()->value(id);
    if (!entry) {
        return 0;
    }
    return entry->metaData();
}

KReportPluginInterface* KReportPluginManager::plugin(const QString& id) const
{
    KReportPluginEntry *entry;

    entry = d->plugins()->value(id);

    if (!entry) {
        entry = d->pluginsByLegacyName()->value(id);
    }

    if (!entry) {
        return 0;
    }
    return entry->plugin();
}

QList<QAction*> KReportPluginManager::createActions(QObject *parent)
{
    Q_ASSERT(parent);
    const QMap<QString, KReportPluginEntry*> *plugins = d->plugins();
    QList<QAction*> actList;
    foreach(KReportPluginEntry* plugin, *plugins) {
        const KReportPluginMetaData *metaData = plugin->metaData();
        if (metaData) {
            QAction *act = new QAction(QIcon::fromTheme(metaData->iconName()), metaData->name(), parent);
            act->setObjectName(metaData->id());
            act->setCheckable(true);

            //Store the order priority in the user data field
            act->setData(metaData->priority());
            actList << act;
        }
    }
    return actList;
}
