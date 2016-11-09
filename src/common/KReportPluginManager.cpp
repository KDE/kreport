/* This file is part of the KDE project
   Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)
   Copyright (C) 2015-2016 Jarosław Staniek <staniek@kde.org>

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
#include "KReportUtils_p.h"

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
    const QString expectedVersion = QString::fromLatin1("%1.%2")
            .arg(KREPORT_STABLE_VERSION_MAJOR).arg(KREPORT_STABLE_VERSION_MINOR);
    foreach(QPluginLoader *loader, offers) {
        //QJsonObject json = loader->metaData();
        //kreportDebug() << json;
        //! @todo check version
        QScopedPointer<KReportPluginEntry> entry(new KReportPluginEntry);
        entry->setMetaData(loader);
        const KReportPluginMetaData *metaData = entry->metaData();
        if (metaData->version() != expectedVersion) {
            kreportWarning() << "KReport element plugin with ID" << metaData->id()
                             << "(" << metaData->fileName() << ")"
                             << "has version" << metaData->version() << "but expected version is"
                             << expectedVersion
                             << "-- skipping it";
            continue;
        }
        if (m_plugins.contains(metaData->id())) {
            kreportWarning() << "KReport element plugin with ID" << metaData->id()
                             << "already found at"
                             << m_plugins.value(metaData->id())->metaData()->fileName()
                             << "-- skipping another at" << metaData->fileName();
            continue;
        }
        if (!KReportPrivate::setupPrivateIconsResourceWithMessage(
            QLatin1String(KREPORT_BASE_NAME_LOWER),
            QString::fromLatin1("icons/%1_%2.rcc")
                .arg(metaData->id()).arg(KReportPrivate::supportedIconTheme),
            QtWarningMsg,
            QString::fromLatin1(":/icons/%1").arg(metaData->id())))
        {
            continue;
        }
        addEntry(entry.take());
    }
    m_findPlugins = false;
}

void KReportPluginManager::Private::addEntry(KReportPluginEntry *entry)
{
    m_plugins.insert(entry->metaData()->id(), entry);
    const QString legacyName(entry->metaData()->value(QLatin1String("X-KDE-PluginInfo-LegacyName"), entry->metaData()->id()));
    if (!legacyName.isEmpty() && entry->metaData()->id().startsWith(QLatin1String("org.kde.kreport"))) {
        m_pluginsByLegacyName.insert(legacyName, entry);
    }
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
    KReportPrivate::setupPrivateIconsResourceWithMessage(
        QLatin1String(KREPORT_BASE_NAME_LOWER),
        QString::fromLatin1("icons/kreport_%1.rcc").arg(KReportPrivate::supportedIconTheme), QtFatalMsg);
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
