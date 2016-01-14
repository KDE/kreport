/* This file is part of the KDE project
   Copyright (C) 2015 by Adam Pigg <adam@piggz.co.uk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KREPORTPAGESIZE_H
#define KREPORTPAGESIZE_H
#include <QPageSize>
#include "kreport_export.h"

/**
 * @brief Handle the page sizes we support in reports
 *
 * A group of helper functions and definitions of the page sizes
 * we support in KReport.
 *
 * We don't support all the sizes supported by Qt, so here we provide a list
 * of supported sizes and translatable descriptions, with helper functions to
 * switch between QString and QPageSizeId
 *
 */
namespace KReportPageSize
{

/**
 * @return list of translated names of all the available formats
 */
KREPORT_EXPORT QStringList pageFormatNames();

/**
 * @return list of non-translated names of all the available formats
 */
KREPORT_EXPORT QStringList pageFormatKeys();

/**
 * @return the default format (based on the default printer)
 */
KREPORT_EXPORT QPageSize::PageSizeId defaultSize();

/**
 * @return the page size Id for the given key
 */
KREPORT_EXPORT QPageSize::PageSizeId pageSize(const QString& key);

/**
 * @return the page size string for the given Id
 */
KREPORT_EXPORT QString pageSizeKey(QPageSize::PageSizeId id);


}

#endif // KREPORTPAGESIZE_H
