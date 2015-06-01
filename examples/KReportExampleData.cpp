/* This file is part of the KDE project
   Copyright (C) 2015 by Adam Pigg (adam@piggz.co.uk)

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

#include "KReportExampleData.h"
#include <QDebug>

KReportExampleData::KReportExampleData()
{
    QList<Data> temp {{ 0, "Adam Pigg", "Kexi", QObject::tr("United Kingdom"), "0123456789", 58.816, -3.1484, "1746287369", false },
                        {1, "Jaroslaw Staniek", "Kexi", QObject::tr("Poland"), "8472947462", 51.895182, 19.623270, "1234567890", true },
                        {2, "Boudewijn Rempt", "Krita", QObject::tr("France"), "8472947462", 48.858915, 2.347661, "1234567890", true },
    };


    m_TestData = temp;

    qDebug() << m_TestData.count();
    m_FieldNames << "id" << "devname" << "projet" << "country" << "mobile" << "lat" << "lon" << "code" << "projetlead";

    m_CurrentRecord = 0;
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
            return m_TestData[m_CurrentRecord].id;
            break;
        case 1:
            return m_TestData[m_CurrentRecord].devName;
            break;

        case 2:
            return m_TestData[m_CurrentRecord].project;
            break;

        case 3:
            return m_TestData[m_CurrentRecord].country;
            break;

        case 4:
            return m_TestData[m_CurrentRecord].mobile;
            break;

        case 5:
            return m_TestData[m_CurrentRecord].lat;
            break;

        case 6:
            return m_TestData[m_CurrentRecord].lon;
            break;

        case 7:
            return m_TestData[m_CurrentRecord].code;
            break;

        case 8:
            return m_TestData[m_CurrentRecord].projectLead;
            break;

        default:
            return QVariant();
    }
}

QStringList KReportExampleData::fieldNames() const
{
    return m_FieldNames;
}

QStringList KReportExampleData::fieldKeys() const
{
    return fieldNames();
}

int KReportExampleData::fieldNumber(const QString& field) const
{
    return m_FieldNames.indexOf(field);
}

qint64 KReportExampleData::recordCount() const
{
    return m_TestData.count();
}

qint64 KReportExampleData::at() const
{
    return m_CurrentRecord;
}

bool KReportExampleData::moveLast()
{
    m_CurrentRecord = recordCount() - 1;
    return true;
}

bool KReportExampleData::moveFirst()
{
    m_CurrentRecord = 0;
    return true;
}

bool KReportExampleData::movePrevious()
{
    if (m_CurrentRecord > 0) {
        m_CurrentRecord--;
        return true;
    }
    return false;
}

bool KReportExampleData::moveNext()
{
    if (m_CurrentRecord < recordCount() - 1) {
        m_CurrentRecord++;
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
