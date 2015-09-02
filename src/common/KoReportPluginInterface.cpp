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

#include "KoReportPluginInterface.h"
#include "KReportPluginMetaData.h"
#include "KReportUtils.h"
#include "kreport_debug.h"

#include <QDomElement>

class KoReportPluginInterface::Private
{
public:
    Private() : metaData(0) {}
    ~Private() {}

    const KReportPluginMetaData *metaData;
};

// ---

KoReportPluginInterface::KoReportPluginInterface(QObject* parent, const QVariantList& args)
 : QObject(parent), d(new Private)
{
    Q_UNUSED(args);
}

KoReportPluginInterface::~KoReportPluginInterface()
{
    delete d;
}

const KReportPluginMetaData* KoReportPluginInterface::metaData() const
{
    return d->metaData;
}

void KoReportPluginInterface::setMetaData(KReportPluginMetaData* metaData)
{
    d->metaData = metaData;
}

bool KoReportPluginInterface::loadElement(KReportElement *el, const QDomElement &dom, KReportDesignReadingStatus *status)
{
    Q_ASSERT(el);
    Q_UNUSED(status);
    el->setName(KReportUtils::attr(dom, "report:name", QString()));
    el->setRect(KReportUtils::readRectAttributes(dom, el->rect()));
    el->setZ(KReportUtils::attr(dom, "report:z-index", el->z()));

    const QDomElement textStyleDom = dom.firstChildElement(QLatin1String("report:text-style"));
    el->setForegroundColor(KReportUtils::attr(textStyleDom, "fo:foreground-color", el->foregroundColor()));
    el->setBackgroundColor(KReportUtils::attr(textStyleDom, "fo:background-color", el->backgroundColor()));
    el->setBackgroundOpacity(KReportUtils::attrPercent(textStyleDom, "fo:background-opacity", el->backgroundOpacity()));
    return true;
}
