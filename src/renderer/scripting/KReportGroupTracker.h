/* This file is part of the KDE project
 * Copyright (C) 2015 by Adam Pigg (adam@piggz.co.uk)
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

#ifndef KREPORTGROUPTRACKER_H
#define KREPORTGROUPTRACKER_H

#include <QObject>
#include "kreport_export.h"

/*!
 * @brief Keeps track of groups as the data for the group changes
 */
class KREPORT_EXPORT KReportGroupTracker : public QObject
{
    Q_OBJECT

protected:
    KReportGroupTracker();
    ~KReportGroupTracker() override;

public:
    Q_SLOT virtual void setGroupData(const QMap<QString, QVariant> &groupData) = 0;
};

#endif // KREPORTGROUPTRACKER_H
