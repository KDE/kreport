/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
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

#include "KReportPageOptions.h"
#include "KReportPageFormat.h"
#include "KReportUnit.h"
#include <QApplication>
#include <QScreen>

KReportPageOptions::KReportPageOptions()
        : QObject(), m_pageSize(QLatin1String("Letter"))
{
    m_marginTop = m_marginBottom = 1.0;
    m_marginLeft = m_marginRight = 1.0;

    m_orientation = Portrait;

    m_customWidth = 8.5;
    m_customHeight = 11.0;
}

KReportPageOptions::KReportPageOptions(const KReportPageOptions & rpo)
        : QObject()
{
    m_marginTop = rpo.m_marginTop;
    m_marginBottom = rpo.m_marginBottom;
    m_marginLeft = rpo.m_marginLeft;
    m_marginRight = rpo.m_marginRight;

    m_pageSize = rpo.m_pageSize;
    m_customWidth = rpo.m_customWidth;
    m_customHeight = rpo.m_customHeight;

    m_orientation = rpo.m_orientation;

    m_labelType = rpo.m_labelType;
}

KReportPageOptions & KReportPageOptions::operator=(const KReportPageOptions & rpo)
{
    m_marginTop = rpo.m_marginTop;
    m_marginBottom = rpo.m_marginBottom;
    m_marginLeft = rpo.m_marginLeft;
    m_marginRight = rpo.m_marginRight;

    m_pageSize = rpo.m_pageSize;
    m_customWidth = rpo.m_customWidth;
    m_customHeight = rpo.m_customHeight;

    m_orientation = rpo.m_orientation;

    m_labelType = rpo.m_labelType;

    return *this;
}

qreal KReportPageOptions::getMarginTop()
{
    return m_marginTop;
}

void KReportPageOptions::setMarginTop(qreal v)
{
    if (m_marginTop == v)
        return;

    m_marginTop = v;
    emit pageOptionsChanged();
}

qreal KReportPageOptions::getMarginBottom()
{
    return m_marginBottom;
}

void KReportPageOptions::setMarginBottom(qreal v)
{
    if (m_marginBottom == v)
        return;

    m_marginBottom = v;
    emit pageOptionsChanged();
}

qreal KReportPageOptions::getMarginLeft()
{
    return m_marginLeft;
}

void KReportPageOptions::setMarginLeft(qreal v)
{
    if (m_marginLeft == v)
        return;

    m_marginLeft = v;
    emit pageOptionsChanged();
}

qreal KReportPageOptions::getMarginRight()
{
    return m_marginRight;
}

void KReportPageOptions::setMarginRight(qreal v)
{
    if (m_marginRight == v)
        return;

    m_marginRight = v;
    emit pageOptionsChanged();
}

const QString & KReportPageOptions::getPageSize()
{
    return m_pageSize;
}
void KReportPageOptions::setPageSize(const QString & s)
{
    if (m_pageSize == s)
        return;

    m_pageSize = s;
    emit pageOptionsChanged();
}
qreal KReportPageOptions::getCustomWidth()
{
    return m_customWidth;
}
void KReportPageOptions::setCustomWidth(qreal v)
{
    if (m_customWidth == v)
        return;

    m_customWidth = v;
    emit pageOptionsChanged();
}
qreal KReportPageOptions::getCustomHeight()
{
    return m_customHeight;
}
void KReportPageOptions::setCustomHeight(qreal v)
{
    if (m_customHeight == v)
        return;

    m_customHeight = v;
    emit pageOptionsChanged();
}

KReportPageOptions::PageOrientation KReportPageOptions::getOrientation()
{
    return m_orientation;
}

bool KReportPageOptions::isPortrait()
{
    return (m_orientation == Portrait);
}

void KReportPageOptions::setOrientation(PageOrientation o)
{
    if (m_orientation == o)
        return;

    m_orientation = o;
    emit pageOptionsChanged();
}
void KReportPageOptions::setPortrait(bool yes)
{
    setOrientation((yes ? Portrait : Landscape));
}

const QString & KReportPageOptions::getLabelType()
{
    return m_labelType;
}
void KReportPageOptions::setLabelType(const QString & type)
{
    if (m_labelType == type)
        return;

    m_labelType = type;
    emit pageOptionsChanged();
}

//Convenience functions that return the page width/height in pixels based on the DPI
qreal KReportPageOptions::widthPx()
{
    int pageWidth;

    if (isPortrait()) {
        pageWidth = KReportPageFormat::width(KReportPageFormat::formatFromString(getPageSize()), KReportPageFormat::Portrait);
    } else {
        pageWidth = KReportPageFormat::width(KReportPageFormat::formatFromString(getPageSize()), KReportPageFormat::Landscape);
    }

    KReportUnit pageUnit(KReportUnit::Millimeter);
    QScreen *srn = QApplication::screens().at(0);

    pageWidth = KReportUnit::toInch(pageUnit.fromUserValue(pageWidth)) * srn->logicalDotsPerInchX();

    return pageWidth;
}

qreal KReportPageOptions::heightPx()
{
    int pageHeight;

    if (isPortrait()) {
        pageHeight = KReportPageFormat::height(KReportPageFormat::formatFromString(getPageSize()), KReportPageFormat::Portrait);
    } else {
        pageHeight = KReportPageFormat::height(KReportPageFormat::formatFromString(getPageSize()), KReportPageFormat::Landscape);
    }

    KReportUnit pageUnit(KReportUnit::Millimeter);
    QScreen *srn = QApplication::screens().at(0);

    pageHeight = KReportUnit::toInch(pageUnit.fromUserValue(pageHeight)) * srn->logicalDotsPerInchY();

    return pageHeight;
}

