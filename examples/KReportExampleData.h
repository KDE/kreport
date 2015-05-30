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

#ifndef KREPORTEXAMPLEDATA_H
#define KREPORTEXAMPLEDATA_H

#include <KoReportData.h>
#include <QVariant>
#include <QStringList>
#include <QList>

class KReportExampleData : public KoReportData
{
public:
    KReportExampleData();
    ~KReportExampleData();
    virtual QVariant value(const QString& field) const;
    virtual QVariant value(unsigned int ) const;
    virtual QStringList fieldNames() const;
    virtual QStringList fieldKeys() const;
    virtual int fieldNumber(const QString& field) const;
    virtual qint64 recordCount() const;
    virtual qint64 at() const;
    virtual bool moveLast();
    virtual bool moveFirst();
    virtual bool movePrevious();
    virtual bool moveNext();
    virtual bool close();
    virtual bool open();

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

    QList<Data> mTestData;
    QStringList mFieldNames;
    int mCurrentRecord;
};

#endif // KREPORTEXAMPLEDATA_H
