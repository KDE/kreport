/* This file is part of the KDE project
 * Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>
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

#ifndef KREPORTLINESTYLE_H
#define KREPORTLINESTYLE_H

#include "kreport_export.h"

#include <QColor>

//! @brief The KReportLineStyle class represents line style
class KREPORT_EXPORT KReportLineStyle //SDC: explicit operator== virtual_dtor
{
public:
    /*!
    @getter
    @return Line's width in points. The default is 0 points.
    @setter
    Sets the line's width to @a width.
    */
    qreal weight; //SDC: default=0.0

    /*!
    @getter
    @return Line's color. The default is invalid color.
    @setter
    Sets the line's color to @a color.
    */
    QColor color; //SDC:

    /*!
    @getter
    @return Line's pen style. The default is Qt::NoPen.
    @setter
    Sets the line's color to @a color.
    */
    Qt::PenStyle penStyle; //SDC: default=Qt::NoPen
};

#endif // KREPORTLABELELEMENT_H
