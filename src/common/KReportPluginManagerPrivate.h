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

#ifndef KREPORTPLUGINMANAGERP_H
#define KREPORTPLUGINMANAGERP_H

#include <QMap>
#include <QPluginLoader>

#if 0 // needed by lupdate to avoid "Qualifying with unknown namespace/class"
class KReportPluginManager {};
#endif

class QJsonObject;
class KReportPluginInterface;
class KReportPluginManager;
class KReportPluginMetaData;

//! @internal A single entry for a built-in or dynamic item plugin
class KReportPluginEntry
{
public:
    //! Used for dynamic plugins.
    KReportPluginEntry();

    //! Used for static plugins.
    KReportPluginEntry(KReportPluginInterface *staticInterface);

    ~KReportPluginEntry();

    KReportPluginInterface* plugin();

    void setBuiltIn(bool set);

    void setStatic(bool set);

    const KReportPluginMetaData *metaData() const;

    void setMetaData(const QJsonObject &metaData);

    void setMetaData(QPluginLoader *loader);

private:
    void setMetaData(KReportPluginMetaData *metaData);

    QPluginLoader *m_loader;

    KReportPluginInterface *m_interface;

    //! Plugin info, owned.
    KReportPluginMetaData *m_metaData;
};

//! @internal
class KReportPluginManager::Private
{
public:
    explicit Private(KReportPluginManager *qq);
    ~Private();

    QMap<QString, KReportPluginEntry*> *plugins();
    QMap<QString, KReportPluginEntry*> *pluginsByLegacyName();

    //! Add a built-in element plugins
    template<class PluginClass>
    void addBuiltInPlugin(const QJsonObject &json);

private:
    KReportPluginManager *q;
    QObject *m_parent;
    bool m_findPlugins;
    void findPlugins();

    //! A map of name -> plugin instances
    QMap<QString, KReportPluginEntry*> m_plugins;
    //! A map of legacy name -> plugin instances
    QMap<QString, KReportPluginEntry*> m_pluginsByLegacyName;
};

#endif
