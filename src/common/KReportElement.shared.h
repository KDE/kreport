/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010-2015 Jarosław Staniek <staniek@kde.org>
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

#ifndef KREPORTELEMENT_H
#define KREPORTELEMENT_H

#include "kreport_export.h"

#include <QColor>
#include <QRectF>

class QDebug;

//! @brief The KReportElement class represents a functional visual element of a report design
class KREPORT_EXPORT KReportElement //SDC: explicit operator== virtual_dtor
{
public:
    /*!
    @getter
    @return element's name.
    @setter
    Sets the element's name to @a name.
    */
    QString name; //SDC:

    /*!
    @getter
    @return element's rectangle.
    @setter
    Sets the element's rectangle to @a rect.
    */
    QRectF rect; //SDC:

    /*!
    @getter
    @return element's Z-value.
    The Z value decides the stacking order of sibling (neighboring) elements.
    A sibling element of high Z value will always be drawn on top of another sibling
    element with a lower Z value.
    If you restore the Z value, the element's insertion order will decide its stacking order.
    The Z-value does not affect the element's size in any way.
    The default Z-value is 0.
    @setter
    Sets the element's Z-value to @a z.
    */
    qreal z; //SDC: default=0

    /*!
    @getter
    @return element's foreground color.
    The default foreground color is invalid what means 'unspecified'.
    @setter
    Sets the element's foreground color to @a foregroundColor.
    */
    QColor foregroundColor; //SDC:

    /*!
    @getter
    @return element's background color.
    The default background color is invalid what means 'unspecified'.
    @setter
    Sets the element's background color to @a backgroundColor.
    */
    QColor backgroundColor; //SDC:

    /*!
    @getter
    @return element's background opacity, which is between 0.0 (transparent) and 1.0 (opaque).
    The default opacity is 0.0.
    @setter
    Sets the element's background opacity to @a backgroundOpacity.
    */
    qreal backgroundOpacity; //SDC: default=0.0 custom_setter
};

//! Sends information about the element @a element to debug output @a dbg.
KREPORT_EXPORT QDebug operator<<(QDebug dbg, const KReportElement& element);

KREPORT_EXPORT uint qHash(const KReportElement &element, uint seed = 0) Q_DECL_NOTHROW;

#endif // KREPORTELEMENT_H
