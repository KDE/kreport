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

#ifndef KREPORTSECTION_H
#define KREPORTSECTION_H

#include <KReportElement>

//! @brief The KReportSection class represents a section of a report design
/*! A section has optional report header, report footer, page header, page footer,
    group header, group footer and detail.
    In the case of page header and page footer it is possible to define (firstpage, odd, even or lastpage). */
shared class export=KREPORT_EXPORT explicit operator== KReportSection
{
public:
    enum Type {
        InvalidType = 0,
        PageHeaderFirst = 1,
        PageHeaderOdd,
        PageHeaderEven,
        PageHeaderLast,
        PageHeaderAny,
        ReportHeader,
        ReportFooter,
        PageFooterFirst,
        PageFooterOdd,
        PageFooterEven,
        PageFooterLast,
        PageFooterAny,
        GroupHeader,
        GroupFooter,
        Detail
    };

    /*!
    @getter
    @return section type
    Default section type is InvalidType.
    @setter
    Sets section type.
    */
    data_member KReportSection::Type type default=KReportSection::InvalidType simple_type;

    /*!
    @getter
    @return section height measured in points
    It is equal to KReportSection::defaultHeight() unless setHeight()
    is called with value > 0.
    @setter
    Sets section height measured in points.
    Set negative value to reset to the default height.
    */
    data_member qreal height default=-1 custom_getter;

    /*!
    @getter
    @return section background color
    It is equal to KReportSection::defaultBackgroundColor() unless setBackgroundColor()
    is called with a valid color.
    @setter
    Sets section background color.
    Set invalid color (QColor()) to reset to the default background color.
    */
    data_member QColor backgroundColor custom_getter;

    /*!
    @getter
    @return all elements of this section
    */
    data_member QList<KReportElement> elements custom_getter no_setter;

    //! Adds element to this section
    void addElement(const KReportElement &element);

    //! @return default height for report sections. The standard is 2cm (converted to points).
    static qreal defaultHeight();

    //! Sets default height for report sections in points.
    static void setDefaultHeight(qreal height);

    //! @return default background color for report sections. The standard is white.
    static QColor defaultBackgroundColor();

    //! Sets default background color for report sections.
    static void setDefaultBackgroundColor(const QColor &color);
};

#endif // KREPORTSECTION_H
