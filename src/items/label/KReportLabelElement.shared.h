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

#ifndef KREPORTLABELELEMENT_H
#define KREPORTLABELELEMENT_H

#include "KReportElement.h"

//! @brief The KReportLabelElement class represents a label element of a report design
shared class export=KREPORT_EXPORT explicit operator== KReportLabelElement : public KReportElement
{
public:
    /*!
    @getter
    @return element's name.
    @setter
    Sets the element's name to @a name.
    */
    data_member QString text;
};

#endif // KREPORTLABELELEMENT_H
