/* This file is part of the KDE project
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

#include "KReportExampleData.h"

KReportExampleData::KReportExampleData()
{
    QList<Data> temp {{ 0, "Adam Pigg", "Kexi", "United Kingdom", "0123456789", 58.816, -3.1484, "1746287369", false }};

    mTestData = temp;

    mFieldNames << "id" << "devname" << "projet" << "country" << "mobile" << "lat" << "lon" << "code" << "projetlead";

    mCurrentRecord = 0;
}

KReportExampleData::~KReportExampleData()
{

}

QVariant KReportExampleData::value(const QString& field) const
{
    return value(fieldNumber(field));
}

QVariant KReportExampleData::value(unsigned int f) const
{
    switch(f) {
        case 0:
            return mTestData[mCurrentRecord].id;
            break;
        case 1:
            return mTestData[mCurrentRecord].devName;
            break;

        case 2:
            return mTestData[mCurrentRecord].project;
            break;

        case 3:
            return mTestData[mCurrentRecord].country;
            break;

        case 4:
            return mTestData[mCurrentRecord].mobile;
            break;

        case 5:
            return mTestData[mCurrentRecord].lat;
            break;

        case 6:
            return mTestData[mCurrentRecord].lon;
            break;

        case 7:
            return mTestData[mCurrentRecord].code;
            break;

        case 8:
            return mTestData[mCurrentRecord].projectLead;;
            break;

        default:
            return QVariant();
    }
}

QStringList KReportExampleData::fieldNames() const
{
    return mFieldNames;
}

QStringList KReportExampleData::fieldKeys() const
{
    return fieldNames();
}

int KReportExampleData::fieldNumber(const QString& field) const
{
    return mFieldNames.indexOf(field);
}

qint64 KReportExampleData::recordCount() const
{
    return mTestData.count();
}

qint64 KReportExampleData::at() const
{
    return mCurrentRecord;
}

bool KReportExampleData::moveLast()
{
    return true;
}

bool KReportExampleData::moveFirst()
{
    return true;
}

bool KReportExampleData::movePrevious()
{
    if (mCurrentRecord > 0) {
        mCurrentRecord--;
        return true;
    }
    return false;
}

bool KReportExampleData::moveNext()
{
    if (mCurrentRecord < recordCount() - 1) {
        mCurrentRecord++;
        return true;
    }
    return false;
}

bool KReportExampleData::close()
{
    return true;
}

bool KReportExampleData::open()
{
    return true;
}
