/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010-2015 Jarosław Staniek <staniek@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "KReportElement.h"

#include <QDebug>
#include <QHash>

KReportElement::~KReportElement()
{
}

void KReportElement::setBackgroundOpacity(qreal backgroundOpacity)
{
    d->backgroundOpacity = qMax(qMin(backgroundOpacity, 1.0), 0.0);
}

KREPORT_EXPORT QDebug operator<<(QDebug dbg, const KReportElement& element)
{
    dbg.nospace() << qPrintable(
        QString::fromLatin1("KReportElement: name=\"%1\" rect=%2 z=%3 foregroundColor=%4 "
                            "backgroundColor=%5 backgroundOpacity=%6")
                         .arg(element.name()).arg(QVariant(element.rect()).toString())
                         .arg(element.z()).arg(QVariant(element.foregroundColor()).toString())
                         .arg(QVariant(element.backgroundColor()).toString())
                         .arg(element.backgroundOpacity()));
    return dbg.space();
}

KREPORT_EXPORT uint qHash(const KReportElement &element, uint seed)
{
    return qHash(element.name(), seed)
            ^ qHash(element.z(), seed)
            ^ qHash(element.rect().x(), seed)
            ^ qHash(element.foregroundColor().name(), seed)
            ^ qHash(element.backgroundColor().name(), seed)
            ^ qHash(qRound64(element.backgroundOpacity() * 1000.0), seed);
}
