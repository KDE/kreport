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

#ifndef KRPOS_H
#define KRPOS_H

#include <QPointF>

#include "kreport_export.h"
#include "KReportUnit.h"

class KProperty;

/**
*/
class KREPORT_EXPORT KRPos
{
public:
    enum UpdatePropertyFlag {
        UpdateProperty,
        DontUpdateProperty
    };

    explicit KRPos(const KReportUnit& unit = KReportUnit(KReportUnit::Centimeter));

    ~KRPos();
    QPointF toUnit() const;
    QPointF toPoint() const;
    QPointF toScene() const;
    void setScenePos(const QPointF&, UpdatePropertyFlag update = UpdateProperty);
    void setUnitPos(const QPointF&, UpdatePropertyFlag update = UpdateProperty);
    void setPointPos(const QPointF&, UpdatePropertyFlag update = UpdateProperty);
    void setUnit(const KReportUnit& unit);
    void setName(const QString& name);
    KReportUnit unit() const { return m_unit; }

    KProperty* property() const {
        return m_property;
    }
private:
    QPointF m_pointPos;
    KReportUnit m_unit;
    KProperty* m_property;
};

#endif
