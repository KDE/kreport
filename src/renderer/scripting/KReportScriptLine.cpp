/* This file is part of the KDE project
 * Copyright (C) 2007-2010 by Adam Pigg (adam@piggz.co.uk)
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

#include "KReportScriptLine.h"
#include "KReportItemLine.h"

#include <KProperty>

namespace Scripting
{

Line::Line(KReportItemLine* l)
{
    m_line = l;
}

Line::~Line()
{
}

QColor Line::lineColor() const
{
    return m_line->m_lineColor->value().value<QColor>();
}

void Line::setLineColor(const QColor& color)
{
    m_line->m_lineColor->setValue(color);
}

int Line::lineWeight() const
{
    return m_line->m_lineWeight->value().toInt();
}

void Line::setLineWeight(int weight)
{
    m_line->m_lineWeight->setValue(weight);
}

int Line::lineStyle() const
{
    return m_line->m_lineStyle->value().toInt();
}

void Line::setLineStyle(int style)
{
    if (style < 0 || style > 5) {
        style = 1;
    }
    m_line->m_lineStyle->setValue(style);
}

QPointF Line::startPosition() const
{
    return m_line->startPosition();
}

void Line::setStartPosition(const QPointF& startPosition)
{
    m_line->setStartPosition(startPosition);
}

QPointF Line::endPosition() const
{
    return m_line->endPosition();
}

void Line::setEndPosition(const QPointF& endPosition)
{
    m_line->setEndPosition(endPosition);
}

}
