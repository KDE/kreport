/* This file is part of the KDE project
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>
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

#include "krpos.h"

#include <KProperty>

#include <klocalizedstring.h>
#include <QScreen>
#include <QApplication>

KRPos::KRPos(const KReportUnit& unit)
{
    m_unit = unit;
    m_property = new KProperty("position", toScene(), i18n("Position"));
}

void KRPos::setName(const QString& name)
{
    m_property->setName(name.toLatin1());
    m_property->setCaption(name);
}

KRPos::~KRPos()
{
}

void KRPos::setScenePos(const QPointF& pos, UpdatePropertyFlag update)
{
    QScreen *srn = QApplication::screens().at(0);

    const qreal x = INCH_TO_POINT(pos.x() / srn->logicalDotsPerInchX());
    const qreal y = INCH_TO_POINT(pos.y() / srn->logicalDotsPerInchY());

    m_pointPos.setX(x);
    m_pointPos.setY(y);

    if (update == UpdateProperty)
        m_property->setValue(toUnit());
}

void KRPos::setUnitPos(const QPointF& pos, UpdatePropertyFlag update)
{
    const qreal x = m_unit.fromUserValue(pos.x());
    const qreal y = m_unit.fromUserValue(pos.y());

    m_pointPos.setX(x);
    m_pointPos.setY(y);

    if (update == UpdateProperty)
        m_property->setValue(toUnit());
}

void KRPos::setPointPos(const QPointF& pos, UpdatePropertyFlag update)
{
    m_pointPos.setX(pos.x());
    m_pointPos.setY(pos.y());

    if (update == UpdateProperty)
        m_property->setValue(toUnit());
}

void KRPos::setUnit(const KReportUnit& u)
{
    m_unit = u;
    m_property->setValue(toUnit());
}

QPointF KRPos::toPoint() const
{
    return m_pointPos;
}

QPointF KRPos::toScene() const
{
    QScreen *srn = QApplication::screens().at(0);

    const qreal x = POINT_TO_INCH(m_pointPos.x()) * srn->logicalDotsPerInchX();
    const qreal y = POINT_TO_INCH(m_pointPos.y()) * srn->logicalDotsPerInchY();

    return QPointF(x, y);
}

QPointF KRPos::toUnit() const
{
    const qreal x = m_unit.toUserValue(m_pointPos.x());
    const qreal y = m_unit.toUserValue(m_pointPos.y());

    return QPointF(x, y);
}

