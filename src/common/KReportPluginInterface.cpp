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

#include "KReportPluginInterface.h"
#include "KReportPluginMetaData.h"
#include "KReportUtils.h"
#include "kreport_debug.h"

#include <QDomElement>

class Q_DECL_HIDDEN KReportPluginInterface::Private
{
public:
    Private() : metaData(nullptr) {}
    ~Private() {}

    const KReportPluginMetaData *metaData;
};

// ---

KReportPluginInterface::KReportPluginInterface(QObject* parent, const QVariantList& args)
 : QObject(parent), d(new Private)
{
    Q_UNUSED(args);
}

KReportPluginInterface::~KReportPluginInterface()
{
    delete d;
}

KReportElement KReportPluginInterface::createElement()
{
    return KReportElement();
}

const KReportPluginMetaData* KReportPluginInterface::metaData() const
{
    return d->metaData;
}

void KReportPluginInterface::setMetaData(KReportPluginMetaData* metaData)
{
    d->metaData = metaData;
}

bool KReportPluginInterface::loadElement(KReportElement *el, const QDomElement &dom, KReportDesignReadingStatus *status)
{
    Q_ASSERT(el);
    Q_UNUSED(status);
    el->setName(KReportUtils::readNameAttribute(dom));
    el->setRect(KReportUtils::readRectAttributes(dom, el->rect()));
    el->setZ(KReportUtils::readZAttribute(dom, el->z()));

    const QDomElement textStyleDom
        = dom.firstChildElement(QLatin1String("report:text-style"));
    el->setForegroundColor(KReportUtils::attr(
        textStyleDom, QLatin1String("fo:foreground-color"), el->foregroundColor()));
    el->setBackgroundColor(KReportUtils::attr(
        textStyleDom, QLatin1String("fo:background-color"), el->backgroundColor()));
    el->setBackgroundOpacity(KReportUtils::attrPercent(
        textStyleDom, QLatin1String("fo:background-opacity"), el->backgroundOpacity()));
    return true;
}
