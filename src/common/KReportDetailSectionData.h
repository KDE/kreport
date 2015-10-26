/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
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

#ifndef KREPORTDETAILSECTIONDATA_H
#define KREPORTDETAILSECTIONDATA_H

#include <QObject>

#include "KReportData.h"

class KReportSectionData;
class KReportDetailGroupSectionData;
class KReportDocument;

class QDomElement;

/**
*/
class KReportDetailSectionData : public QObject
{
    Q_OBJECT
public:
    explicit KReportDetailSectionData(QObject *parent = 0);
    KReportDetailSectionData(const QDomElement &elemSource, KReportDocument *report);
    ~KReportDetailSectionData();

    enum PageBreak {
        BreakNone = 0,
        BreakAtEnd = 1
    };

    QString m_name;
    int m_pageBreak;
    QList<KReportData::SortedField> m_sortedFields;

    KReportSectionData * m_detailSection;

    QList<KReportDetailGroupSectionData*> m_groupList;

    bool isValid() const {
        return m_valid;
    }

private:
    bool m_valid;
};

class KReportDetailGroupSectionData
{
public:
    KReportDetailGroupSectionData();

    enum PageBreak {
        BreakNone = 0,
        BreakAfterGroupFooter = 1,
        BreakBeforeGroupHeader = 2
    };

    //QString name;
    QString m_column;
    PageBreak m_pagebreak;
    Qt::SortOrder m_sort;

    KReportSectionData *m_groupHeader;
    KReportSectionData *m_groupFooter;
};

#endif
