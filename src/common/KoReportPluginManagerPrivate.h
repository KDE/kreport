/* This file is part of the KDE project
   Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)
 
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

#ifndef KOREPORTPLUGINMANAGERP_H
#define KOREPORTPLUGINMANAGERP_H

#include <QMap>
#include <QPluginLoader>

class KoReportPluginInterface;

//! A single entry for a built-in or dynamic item plugin
class ReportPluginEntry
{
public:
    ReportPluginEntry();
    KoReportPluginInterface* plugin();

    KoReportPluginInterface *interface;
    QPluginLoader *loader;
};

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


class KoReportPluginManagerPrivate
{
public:
    KoReportPluginManagerPrivate();
    ~KoReportPluginManagerPrivate();

    void findPlugins();

    // add A built-in element plugins
    template<class PluginClass>
    void addBuiltInPlugin();

    //! Map of name -> plugin instances
    QMap<QString, ReportPluginEntry*> plugins;

private:
    QObject *m_parent;
};

#endif
