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

#ifndef KOREPORTPLUGININFO_H
#define KOREPORTPLUGININFO_H

#include <KPluginMetaData>

#include "kreport_export.h"

//! Information about a KReport plugin.
class KREPORT_EXPORT KReportPluginMetaData : public KPluginMetaData
{
public:
    ~KReportPluginMetaData();

    //! @return internal name of the plugin, a shortcut of pluginId()
    QString id() const;

    int priority() const;

    //! @return true if this plugin is built-in, i.e. has been provided by the KReport
    //! framework itself, not by a plugin file.
    bool isBuiltIn() const;

    //! @return true if this plugin is static, i.e. is compiled-in.
    bool isStatic() const;

protected:
    explicit KReportPluginMetaData(const QJsonObject &metaData);
    explicit KReportPluginMetaData(const QPluginLoader &loader);

    void setBuiltIn(bool set);
    void setStatic(bool set);

    friend class KReportPluginEntry;

private:
    Q_DISABLE_COPY(KReportPluginMetaData)
    class Private;
    Private * const d;
};

#endif // KOREPORTPLUGININFO_H
