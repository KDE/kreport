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
#include "KoReportPluginInfo.h"
#include "KReportJsonTrader_p.h"

#include <kicon.h>
#include <kservice.h>
#include <kiconloader.h>
#include <ktoggleaction.h>

#include <QDebug>
#include <QPluginLoader>

//Include the static items
#include "../items/label/KoReportLabelPlugin.h"
#include "../items/check/KoReportCheckPlugin.h"
#include "../items/field/KoReportFieldPlugin.h"
#include "../items/image/KoReportImagePlugin.h"
#include "../items/text/KoReportTextPlugin.h"

KoReportPluginManager::Private::Private(KoReportPluginManager *qq)
    : q(qq), m_parent(new QObject)
{
    addBuiltInPlugin<KoReportLabelPlugin>();
    addBuiltInPlugin<KoReportCheckPlugin>();
    addBuiltInPlugin<KoReportFieldPlugin>();
    addBuiltInPlugin<KoReportImagePlugin>();
    addBuiltInPlugin<KoReportTextPlugin>();

    findPlugins();
}

// ---

ReportPluginEntry::ReportPluginEntry()
    : interface(0), loader(0)
{
}

KoReportPluginInterface* ReportPluginEntry::plugin()
{
    if (interface) {
        return interface;
    }
    if (!loader) {
        qWarning() << "No such plugin";
        return 0;
    }
    if (!loader->load()) {
        qWarning() << "Could not load plugin" << loader->fileName();
        return 0;
    }
    interface = qobject_cast<KoReportPluginInterface*>(loader->instance());
    if (!interface) {
        qWarning() << "Could not create instance of plugin" << loader->fileName();
        return 0;
    }
    return interface;
}

// ---

KoReportPluginManager::Private::~Private()
{
    delete m_parent;
}

template<class PluginClass>
void KoReportPluginManager::Private::addBuiltInPlugin()
{
    ReportPluginEntry *entry = new ReportPluginEntry;
    entry->interface = new PluginClass(m_parent);
    q->setBuiltIn(entry->interface, true);
    plugins.insert(QLatin1String(entry->interface->info()->className()), entry);
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
        QJsonObject json = loader->metaData().value(QLatin1String("MetaData")).toObject();
        json = json.value(QLatin1String("KPlugin")).toObject();
        const QString pluginName = json.value(QLatin1String("Id")).toString();
            //! @todo check version
            //! @todo add pluginNameProperty
            //KexiPluginLoader loader(service, "");

//        KoReportPluginInterface *plugin = loader.createPlugin<KoReportPluginInterface>(this);
//        if (!plugin) {
//            qWarning() << "KPluginFactory could not load the plugin:" << service->library();
//            continue;
//        }
//        plugin->info()->setPriority(plugin->info()->priority() + 10); //Ensure plugins always have a higher prioroty than built-in types
//        m_plugins.insert(plugin->info()->className(), plugin);
        ReportPluginEntry *entry = new ReportPluginEntry;
        entry->loader = loader;
        plugins.insert(pluginName, entry);
    }
}

// ---

KoReportPluginManager::KoReportPluginManager()
    : d(new Private(this))
{
}

KoReportPluginManager::~KoReportPluginManager()
{
    delete d;
}

KoReportPluginManager* KoReportPluginManager::self()
{
    K_GLOBAL_STATIC(KoReportPluginManager, instance) // only instantiated when self() is called
    return instance;
}

QStringList KoReportPluginManager::pluginNames() const
{
    return d->plugins.keys();
}

KoReportPluginInterface* KoReportPluginManager::plugin(const QString& p) const
{
    ReportPluginEntry *entry = d->plugins.value(p);
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
        KoReportPluginInfo *info = plugin->info();
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

void KoReportPluginManager::setBuiltIn(KoReportPluginInterface* interface, bool set)
{
    interface->setBuiltIn(set);
}
