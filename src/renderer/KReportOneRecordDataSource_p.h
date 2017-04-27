/*
 *    This file is part of the KDE project
 *    Copyright (C) 2015 by Adam Pigg <adam@piggz.co.uk>
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Library General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Library General Public License for more details.
 *
 *    You should have received a copy of the GNU Library General Public License
 *    along with this library; see the file COPYING.LIB.  If not, write to
 *    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *    Boston, MA 02110-1301, USA.
 *
*/

#ifndef KREPORTONERECORDDATA_H
#define KREPORTONERECORDDATA_H

#include <KReportDataSource.h>

namespace KReportPrivate {
/**
 * @brief A report data source which returns one record
 *
 * When no other data source is specified the pre-renderer
 * will create this as its data source.  This will return
 * one record of data, and no fields.  Useful for creating
 * a report with a static page of data, or a report page
 * generated from a script only.
 */
class OneRecordDataSource :  public KReportDataSource
{
public:
    OneRecordDataSource();
    ~OneRecordDataSource() override;
    QVariant value(const QString &field) const override;
    QVariant value(int fieldNum) const override;
    QStringList fieldNames() const override;
    int fieldNumber(const QString &field) const override;
    qint64 recordCount() const override;
    qint64 at() const override;
    bool moveLast() override;
    bool moveFirst() override;
    bool movePrevious() override;
    bool moveNext() override;
    bool close() override;
    bool open() override;
    QStringList dataSourceNames() const override;
};
}

#endif // KREPORTONERECORDDATA_H
