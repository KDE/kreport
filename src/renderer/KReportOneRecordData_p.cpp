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

#include "KReportOneRecordData_p.h"

namespace KReportPrivate {

OneRecordData::OneRecordData()
{
}

OneRecordData::~OneRecordData()
{

}

QVariant OneRecordData::value(const QString& field) const
{
    Q_UNUSED(field);
    return QVariant();
}

QVariant OneRecordData::value(unsigned int fieldNum) const
{
    Q_UNUSED(fieldNum);
    return QVariant();
}

QStringList OneRecordData::fieldNames() const
{
    return QStringList();
}

int OneRecordData::fieldNumber(const QString& field) const
{
    Q_UNUSED(field);
    return 0;
}

qint64 OneRecordData::recordCount() const
{
    return 1;
}

qint64 OneRecordData::at() const
{
    return 0;
}

bool OneRecordData::moveLast()
{
    return true;
}

bool OneRecordData::moveFirst()
{
    return true;
}

bool OneRecordData::movePrevious()
{
    return false;
}

bool OneRecordData::moveNext()
{
    return false;
}

bool OneRecordData::close()
{
    return true;
}

bool OneRecordData::open()
{
    return true;
}

}