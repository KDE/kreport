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

#include "KReportScriptLabel.h"

#include <QSizeF>
#include <QPointF>

namespace Scripting
{
Label::Label(KReportItemLabel *l)
{
    m_label = l;
}


Label::~Label()
{
}

QString Label::caption() const
{
    return m_label->textValue();
}

void Label::setCaption(const QString& c)
{
    m_label->setText(c);
}

int Label::horizontalAlignment() const
{
    const QString a = m_label->horizontalAlignment->value().toString().toLower();

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
void Label::setHorizonalAlignment(int a)
{
    switch (a) {
    case -1:
        m_label->horizontalAlignment->setValue(QLatin1String("left"));
        break;
    case 0:
        m_label->horizontalAlignment->setValue(QLatin1String("center"));
        break;
    case 1:
        m_label->horizontalAlignment->setValue(QLatin1String("right"));
        break;
    default:
        m_label->horizontalAlignment->setValue(QLatin1String("left"));
        break;
    }
}

int Label::verticalAlignment() const
{
    const QString a = m_label->horizontalAlignment->value().toString().toLower();

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
void Label::setVerticalAlignment(int a)
{
    switch (a) {
    case -1:
        m_label->verticalAlignment->setValue(QLatin1String("top"));
        break;
    case 0:
        m_label->verticalAlignment->setValue(QLatin1String("middle"));
        break;
    case 1:
        m_label->verticalAlignment->setValue(QLatin1String("bottom"));
        break;
    default:
        m_label->verticalAlignment->setValue(QLatin1String("middle"));
        break;
    }
}

QColor Label::backgroundColor() const
{
    return m_label->backgroundColor->value().value<QColor>();
}
void Label::setBackgroundColor(const QColor& c)
{
    m_label->backgroundColor->setValue(c);
}

QColor Label::foregroundColor() const
{
    return m_label->foregroundColor->value().value<QColor>();
}
void Label::setForegroundColor(const QColor& c)
{
    m_label->foregroundColor->setValue(c);
}

int Label::backgroundOpacity() const
{
    return m_label->backgroundOpacity->value().toInt();
}
void Label::setBackgroundOpacity(int o)
{
    m_label->backgroundOpacity->setValue(o);
}

QColor Label::lineColor() const
{
    return m_label->lineColor->value().value<QColor>();
}
void Label::setLineColor(const QColor& c)
{
    m_label->lineColor->setValue(c);
}

int Label::lineWeight() const
{
    return m_label->lineWeight->value().toInt();
}
void Label::setLineWeight(int w)
{
    m_label->lineWeight->setValue(w);
}

int Label::lineStyle() const
{
    return m_label->lineStyle->value().toInt();
}
void Label::setLineStyle(int s)
{
    if (s < 0 || s > 5) {
        s = 1;
    }
    m_label->lineStyle->setValue(s);
}

QPointF Label::position() const
{
    return m_label->position();
}
void Label::setPosition(const QPointF &p)
{
    m_label->setPosition(p);
}

QSizeF Label::size() const
{
    return m_label->size();
}
void Label::setSize(const QSizeF &s)
{
    m_label->setSize(s);
}
}

