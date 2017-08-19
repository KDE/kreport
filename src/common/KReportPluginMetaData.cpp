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

#include "KReportPluginMetaData.h"

#include <QPluginLoader>
#include "kreport_debug.h"

class KReportPluginMetaData::Private
{
public:
    Private(KReportPluginMetaData *metaData) : isBuiltIn(false), isStatic(false)
    {
        const QString s = metaData->value(QLatin1String("X-KReport-PluginInfo-Priority"));
        bool ok;
        int i = s.toInt(&ok);
        priority = ok ? i : 100; // default priority is low
    }

    int priority;
    bool isBuiltIn;
    bool isStatic;
};

// ---

KReportPluginMetaData::KReportPluginMetaData(const QJsonObject &metaData)
    : KPluginMetaData(metaData, QString()), d(new Private(this))
{
    //kreportDebug() << rawData();
}

KReportPluginMetaData::KReportPluginMetaData(const QPluginLoader &loader)
    : KPluginMetaData(loader), d(new Private(this))
{
    //kreportDebug() << rawData();
}

KReportPluginMetaData::~KReportPluginMetaData()
{
    delete d;
}

QString KReportPluginMetaData::id() const
{
    return pluginId();
}

int KReportPluginMetaData::priority() const
{
    return d->priority;
}

bool KReportPluginMetaData::isBuiltIn() const
{
    return d->isBuiltIn;
}

void KReportPluginMetaData::setBuiltIn(bool set)
{
    d->isBuiltIn = set;
}

bool KReportPluginMetaData::isStatic() const
{
    return d->isStatic;
}

void KReportPluginMetaData::setStatic(bool set)
{
    d->isStatic = set;
}
