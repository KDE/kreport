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

#include "KReportDataSource.h"

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
    explicit KReportDetailSectionData(QObject *parent = nullptr);
    KReportDetailSectionData(const QDomElement &elemSource, KReportDocument *report);
    ~KReportDetailSectionData() override;

    enum class PageBreak {
        None,
        AtEnd
    };

    QString name;
    PageBreak pageBreak;
    QList<KReportDataSource::SortedField> sortedFields;

    KReportSectionData *detailSection;

    QList<KReportDetailGroupSectionData*> groupList;

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

    enum class PageBreak {
        None,
        AfterGroupFooter,
        BeforeGroupHeader
    };

    QString column;
    PageBreak pagebreak;
    Qt::SortOrder m_sort;

    KReportSectionData *groupHeader;
    KReportSectionData *groupFooter;
};

#endif
