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

#include "KReportScriptField.h"

#include <QSizeF>
#include <QPointF>


namespace Scripting
{
Field::Field(KReportItemField *f)
{
    m_field = f;
}


Field::~Field()
{
}

QString Field::source() const
{
    return m_field->itemDataSource();
}

void Field::setSource(const QString& s)
{
    m_field->setItemDataSource(s);
}

int Field::horizontalAlignment() const
{
    const QString a = m_field->horizontalAlignment->value().toString().toLower();

    if (a == QLatin1String("left")) {
        return -1;
    }
    if (a == QLatin1String("center")) {
        return 0;
    }
    if (a == QLatin1String("right")) {
        return 1;
    }
    return -1;
}
void Field::setHorizonalAlignment(int a)
{
    switch (a) {
    case -1:
        m_field->horizontalAlignment->setValue(QLatin1String("left"));
        break;
    case 0:
        m_field->horizontalAlignment->setValue(QLatin1String("center"));
        break;
    case 1:
        m_field->horizontalAlignment->setValue(QLatin1String("right"));
        break;
    default:
        m_field->horizontalAlignment->setValue(QLatin1String("left"));
        break;
    }
}

int Field::verticalAlignment() const
{
    const QString a = m_field->horizontalAlignment->value().toString().toLower();

    if (a == QLatin1String("top")) {
        return -1;
    }
    if (a == QLatin1String("middle")) {
        return 0;
    }
    if (a == QLatin1String("bottom")) {
        return 1;
    }
    return -1;
}
void Field::setVerticalAlignment(int a)
{
    switch (a) {
    case -1:
        m_field->verticalAlignment->setValue(QLatin1String("top"));
        break;
    case 0:
        m_field->verticalAlignment->setValue(QLatin1String("middle"));
        break;
    case 1:
        m_field->verticalAlignment->setValue(QLatin1String("bottom"));
        break;
    default:
        m_field->verticalAlignment->setValue(QLatin1String("middle"));
        break;
    }
}

QColor Field::backgroundColor() const
{
    return m_field->backgroundColor->value().value<QColor>();
}
void Field::setBackgroundColor(const QColor& c)
{
    m_field->backgroundColor->setValue(c);
}

QColor Field::foregroundColor() const
{
    return m_field->foregroundColor->value().value<QColor>();
}
void Field::setForegroundColor(const QColor& c)
{
    m_field->foregroundColor->setValue(c);
}

int Field::backgroundOpacity() const
{
    return m_field->backgroundOpacity->value().toInt();
}
void Field::setBackgroundOpacity(int o)
{
    m_field->backgroundOpacity->setValue(o);
}

QColor Field::lineColor() const
{
    return m_field->lineColor->value().value<QColor>();
}
void Field::setLineColor(const QColor& c)
{
    m_field->lineColor->setValue(c);
}

int Field::lineWeight() const
{
    return m_field->lineWeight->value().toInt();
}
void Field::setLineWeight(int w)
{
    m_field->lineWeight->setValue(w);
}

int Field::lineStyle() const
{
    return m_field->lineStyle->value().toInt();
}
void Field::setLineStyle(int s)
{
    if (s < 0 || s > 5) {
        s = 1;
    }
    m_field->lineStyle->setValue(s);
}

QPointF Field::position() const
{
    return m_field->position();
}
void Field::setPosition(const QPointF &p)
{
    m_field->setPosition(p);
}

QSizeF Field::size() const
{
    return m_field->size();
}
void Field::setSize(const QSizeF &s)
{
    m_field->setSize(s);
}

}
