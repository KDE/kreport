/* This file is part of the KDE project
 * Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)
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

#include "KReportScriptCheck.h"
#include <KProperty>

#include <QSizeF>
#include <QPointF>

namespace Scripting
{

CheckBox::CheckBox(KReportItemCheckBox *c)
{
    m_check = c;
}


CheckBox::~CheckBox()
{
}

bool CheckBox::value() const
{
    return m_check->value();
}

void CheckBox::setValue(bool v)
{
    m_check->setValue(v);
}

QString CheckBox::checkStyle() const
{
    return m_check->m_checkStyle->value().toString();
}

void CheckBox::setCheckStyle(const QString &style)
{
    m_check->m_checkStyle->setValue(style);
}

QColor CheckBox::foregroundColor() const
{
    return m_check->m_foregroundColor->value().value<QColor>();
}
void CheckBox::setForegroundColor(const QColor& c)
{
    m_check->m_foregroundColor->setValue(c);
}

QColor CheckBox::lineColor() const
{
    return m_check->m_lineColor->value().value<QColor>();
}
void CheckBox::setLineColor(const QColor& c)
{
    m_check->m_lineColor->setValue(c);
}

int CheckBox::lineWeight() const
{
    return m_check->m_lineWeight->value().toInt();
}
void CheckBox::setLineWeight(int w)
{
    m_check->m_lineWeight->setValue(w);
}

int CheckBox::lineStyle() const
{
    return m_check->m_lineStyle->value().toInt();
}
void CheckBox::setLineStyle(int s)
{
    if (s < 0 || s > 5) {
        s = 1;
    }
    m_check->m_lineStyle->setValue(s);
}

QPointF CheckBox::position() const
{
    return KReportItemBase::scenePosition(m_check->position());
}

void CheckBox::setPosition(const QPointF &p)
{
    m_check->setPosition(KReportItemBase::positionFromScene(p));
}

QSizeF CheckBox::size() const
{
    return KReportItemBase::sceneSize(m_check->size());
}

void CheckBox::setSize(const QSizeF &s)
{
    m_check->setSize(KReportItemBase::sizeFromScene(s));
}
}
