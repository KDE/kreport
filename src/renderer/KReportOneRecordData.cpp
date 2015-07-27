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

#include "KReportOneRecordData.h"

KReportOneRecordData::KReportOneRecordData()
{
}

KReportOneRecordData::~KReportOneRecordData()
{

}

QVariant KReportOneRecordData::value(const QString& field) const
{
    Q_UNUSED(field);
    return QVariant();
}

QVariant KReportOneRecordData::value(unsigned int fieldNum) const
{
    Q_UNUSED(fieldNum);
    return QVariant();
}

QStringList KReportOneRecordData::fieldNames() const
{
    return QStringList();
}

int KReportOneRecordData::fieldNumber(const QString& field) const
{
    Q_UNUSED(field);
    return 0;
}

qint64 KReportOneRecordData::recordCount() const
{
    return 1;
}

qint64 KReportOneRecordData::at() const
{
    return 0;
}

bool KReportOneRecordData::moveLast()
{
    return true;
}

bool KReportOneRecordData::moveFirst()
{
    return true;
}

bool KReportOneRecordData::movePrevious()
{
    return false;
}

bool KReportOneRecordData::moveNext()
{
    return false;
}

bool KReportOneRecordData::close()
{
    return true;
}

bool KReportOneRecordData::open()
{
    return true;
}
