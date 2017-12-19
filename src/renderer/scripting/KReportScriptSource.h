/* This file is part of the KDE project
 * Copyright (C) 2017 by Adam Pigg (adam@piggz.co.uk)
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

#ifndef KREPORTSCRIPTSOURCE_H
#define KREPORTSCRIPTSOURCE_H

#include <QStringList>
#include <QString>

#include "kreport_export.h"

/*!
 * @brief Abstraction of report script source.
 *
 * A script source is implemented by the source
 * application to provide scripts to the report.
 */
class KREPORT_EXPORT KReportScriptSource
{
public:
    virtual ~KReportScriptSource();

    //! Allow the implementation to return a list of possible scripts
    virtual QStringList scriptList() const = 0;

    //! Allow the implementation to return some script code based on a specific script name
    //! as set in the report
    virtual QString scriptCode(const QString& script) const = 0;
};

#endif
