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
#ifndef KRSIZE_H
#define KRSIZE_H

#include <QSizeF>

#include "KReportUnit.h"
#include "kreport_export.h"

class KProperty;

/**
*/
class KREPORT_EXPORT KRSize
{
public:
    enum UpdatePropertyFlag {
        UpdateProperty,
        DontUpdateProperty
    };
    explicit KRSize(const KReportUnit& unit = KReportUnit(KReportUnit::Centimeter));

    ~KRSize();
    QSizeF toUnit() const;
    QSizeF toPoint() const;
    QSizeF toScene() const;
    void setSceneSize(const QSizeF&, UpdatePropertyFlag update = UpdateProperty);
    void setUnitSize(const QSizeF&, UpdatePropertyFlag update = UpdateProperty);
    void setPointSize(const QSizeF&, UpdatePropertyFlag update = UpdateProperty);
    void setUnit(KReportUnit);

    KProperty* property() const {
        return m_property;
    }
private:
    QSizeF m_pointSize;
    KReportUnit m_unit;
    KProperty* m_property;
};

#endif
