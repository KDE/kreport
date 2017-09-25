/* This file is part of the KDE project
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

#include "KReportScriptBarcode.h"

#include <KProperty>
#include <QSizeF>
#include <QPointF>

namespace Scripting
{

Barcode::Barcode(KReportItemBarcode *b)
{
    m_barcode = b;
}

Barcode::~Barcode()
{
}

QPointF Barcode::position() const
{
    return m_barcode->position();
}

void Barcode::setPosition(const QPointF& p)
{
    m_barcode->setPosition(p);
}

QSizeF Barcode::size() const
{
    return m_barcode->size();
}

void Barcode::setSize(const QSizeF& s)
{
    m_barcode->setSize(s);
}

int Barcode::horizontalAlignment() const
{
    return m_barcode->horizontalAlignmentValue();
}

void Barcode::setHorizonalAlignment(int a)
{
    m_barcode->setHorizontalAlignmentValue(a);
}

QString Barcode::source() const
{
    return m_barcode->controlSource->value().toString();
}

void Barcode::setSource(const QString& s)
{
    m_barcode->controlSource->setValue(s);
}

QString Barcode::format() const
{
    return m_barcode->formatValue();
}

void Barcode::setFormat(const QString& s)
{
    m_barcode->setFormatValue(s);
}
}
