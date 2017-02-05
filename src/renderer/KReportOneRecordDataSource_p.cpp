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

#include "KReportOneRecordDataSource_p.h"

namespace KReportPrivate {

OneRecordDataSource::OneRecordDataSource()
{
}

OneRecordDataSource::~OneRecordDataSource()
{

}

QVariant OneRecordDataSource::value(const QString& field) const
{
    Q_UNUSED(field);
    return QVariant();
}

QVariant OneRecordDataSource::value(int fieldNum) const
{
    Q_UNUSED(fieldNum);
    return QVariant();
}

QStringList OneRecordDataSource::fieldNames() const
{
    return QStringList();
}

int OneRecordDataSource::fieldNumber(const QString& field) const
{
    Q_UNUSED(field);
    return 0;
}

qint64 OneRecordDataSource::recordCount() const
{
    return 1;
}

qint64 OneRecordDataSource::at() const
{
    return 0;
}

bool OneRecordDataSource::moveLast()
{
    return true;
}

bool OneRecordDataSource::moveFirst()
{
    return true;
}

bool OneRecordDataSource::movePrevious()
{
    return false;
}

bool OneRecordDataSource::moveNext()
{
    return false;
}

bool OneRecordDataSource::close()
{
    return true;
}

bool OneRecordDataSource::open()
{
    return true;
}

QStringList OneRecordDataSource::dataSourceNames() const
{
    return QStringList();
}

}
