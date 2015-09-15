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
                        {2, "Boudewijn Rempt", "Krita", QObject::tr("Netherlands"), "8472947462", 48.858915, 2.347661, "1234567890", true },
    };


    m_testData = temp;

    qDebug() << m_testData.count();
    m_fieldNames << "id" << "devname" << "project" << "country" << "mobile" << "lat" << "lon" << "code" << "projectlead";

    m_currentRecord = 0;
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
            return m_testData[m_currentRecord].id;
            break;
        case 1:
            return m_testData[m_currentRecord].devName;
            break;

        case 2:
            return m_testData[m_currentRecord].project;
            break;

        case 3:
            return m_testData[m_currentRecord].country;
            break;

        case 4:
            return m_testData[m_currentRecord].mobile;
            break;

        case 5:
            return m_testData[m_currentRecord].lat;
            break;

        case 6:
            return m_testData[m_currentRecord].lon;
            break;

        case 7:
            return m_testData[m_currentRecord].code;
            break;

        case 8:
            return m_testData[m_currentRecord].projectLead;
            break;

        default:
            return QVariant();
    }
}

QStringList KReportExampleData::fieldNames() const
{
    return m_fieldNames;
}

QStringList KReportExampleData::fieldKeys() const
{
    return fieldNames();
}

int KReportExampleData::fieldNumber(const QString& field) const
{
    return m_fieldNames.indexOf(field);
}

qint64 KReportExampleData::recordCount() const
{
    return m_testData.count();
}

qint64 KReportExampleData::at() const
{
    return m_currentRecord;
}

bool KReportExampleData::moveLast()
{
    m_currentRecord = recordCount() - 1;
    return true;
}

bool KReportExampleData::moveFirst()
{
    m_currentRecord = 0;
    return true;
}

bool KReportExampleData::movePrevious()
{
    if (m_currentRecord > 0) {
        m_currentRecord--;
        return true;
    }
    return false;
}

bool KReportExampleData::moveNext()
{
    if (m_currentRecord < recordCount() - 1) {
        m_currentRecord++;
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

QStringList KReportExampleData::scriptList() const
{
    QStringList scripts;

    scripts << "example";

    return scripts;
}

QString KReportExampleData::scriptCode(const QString &script) const
{
    if (script != "example")
        return QString();

    QString scriptcode;

    scriptcode = ""
             "function detail(){\n"
             "  var count = 0;"
             "  this.OnRender = function() {\n"
             "    count++;\n"
             "    debug.print(\"printing this from the javascript engine\");\n"
             "    if (count % 2 == 0) {\n"
             "      example_report.section_detail.setBackgroundColor(\"#ffffff\");\n"
             "    } else {\n"
             "      example_report.section_detail.setBackgroundColor(\"#dddddd\");\n"
             "    }\n"
             "    example_report.section_detail.objectByName(\"label1\").setCaption(\"Record: \" + count);\n"
             "  }\n"
             "}\n"
             "\n"
             "function report(){\n"
             "  this.OnOpen = function() {\n"
             "    debug.print(\"report on-open event\");\n"
             "  }\n"
             "}\n"
             "example_report.section_detail.initialize(new detail());\n"
             "example_report.initialize(new report());\n";

    return scriptcode;
}
