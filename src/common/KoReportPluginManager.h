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

#ifndef KOREPORTPLUGINMANAGER_H
#define KOREPORTPLUGINMANAGER_H

#include "config-kreport.h"
#include "kreport_export.h"

#include <QList>
#include <QObject>

class KReportPluginMetaData;
class KoReportPluginInterface;

class QStringList;
class QAction;
class QString;

class KREPORT_EXPORT KoReportPluginManager : public QObject
{
    Q_OBJECT
    public:
        static KoReportPluginManager* self();

        QStringList pluginIds() const;

        const KReportPluginMetaData *pluginMetaData(const QString& id) const;

        KoReportPluginInterface* plugin(const QString& id) const;

        QList<QAction*> actions();

    private:
        // class for access to the constructor
        friend class KReportPluginManagerSingleton;

        KoReportPluginManager();

        ~KoReportPluginManager();

        Q_DISABLE_COPY(KoReportPluginManager)
        class Private;
        Private *const d;
};

#endif // KOREPORTPLUGINMANAGER_H
