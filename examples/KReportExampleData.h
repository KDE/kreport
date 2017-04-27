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

#ifndef KREPORTEXAMPLEDATA_H
#define KREPORTEXAMPLEDATA_H

#include <KReportData>
#include <QVariant>
#include <QStringList>
#include <QList>

class KReportExampleData : public KReportData
{
public:
    KReportExampleData();
    ~KReportExampleData() override;
    QVariant value(const QString &field) const override;
    QVariant value(unsigned int) const override;
    QStringList fieldNames() const override;
    QStringList fieldKeys() const override;
    int fieldNumber(const QString &field) const override;
    qint64 recordCount() const override;
    qint64 at() const override;
    bool moveLast() override;
    bool moveFirst() override;
    bool movePrevious() override;
    bool moveNext() override;
    bool close() override;
    bool open() override;

    QStringList scriptList() const override;
    QString scriptCode(const QString &script) const override;

private:
    struct Data
    {
        int id;
        QString devName;
        QString project;
        QString country;
        QString mobile;
        float lat;
        float lon;
        QString code;
        bool projectLead;
    };

    QList<Data> m_testData;
    QStringList m_fieldNames;
    int m_currentRecord;
};

#endif // KREPORTEXAMPLEDATA_H
