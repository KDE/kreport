/* This file is part of the KDE project
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
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
#ifndef SCRIPTINGKRSCRIPTREPORT_H
#define SCRIPTINGKRSCRIPTREPORT_H

#include <QObject>
#include <QtQml/QJSValue>

class KoReportReportData;

namespace Scripting
{

/**
*/
class Report : public QObject
{
    Q_OBJECT
public:
    explicit Report(KoReportReportData*);

    ~Report();

public Q_SLOTS:
    QString title();
    QString name();
    QString recordSource();
    QObject* objectByName(const QString &);
    QObject* sectionByName(const QString &);


    void initialize(QJSValue val);
    void eventOnOpen();
    void eventOnComplete();
    void eventOnNewPage();

private:
    KoReportReportData *m_reportData;
    QJSValue m_scriptObject;
};

}

#endif
