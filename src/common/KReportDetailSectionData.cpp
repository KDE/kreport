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

#include "KReportDetailSectionData.h"
#include "KReportSectionData.h"
#include "KReportDocument.h"
#include "KReportUtils.h"

#include "kreport_debug.h"
#include <QDomElement>

KReportDetailSectionData::KReportDetailSectionData(QObject *parent)
 : QObject(parent)
{
    pageBreak = PageBreak::None;
    detailSection = nullptr;
    m_valid = true;
}

KReportDetailSectionData::KReportDetailSectionData(const QDomElement &elemSource, KReportDocument *report)
 : QObject(report)
{
    pageBreak = PageBreak::None;
    detailSection = nullptr;
    m_valid = false;
    //kreportDebug() << elemSource.tagName();
    if (elemSource.tagName() != QLatin1String("report:detail")) {
        return;
    }

    QDomNodeList sections = elemSource.childNodes();

    for (int nodeCounter = 0; nodeCounter < sections.count(); nodeCounter++) {
        QDomElement elemThis = sections.item(nodeCounter).toElement();

        if (elemThis.tagName() == QLatin1String("report:group")) {
            KReportDetailGroupSectionData * dgsd = new KReportDetailGroupSectionData();

            if ( elemThis.hasAttribute( QLatin1String("report:group-column") ) ) {
                dgsd->column = elemThis.attribute( QLatin1String("report:group-column") );
            }

            if ( elemThis.hasAttribute( QLatin1String("report:group-page-break") ) ) {
                QString s = elemThis.attribute( QLatin1String("report:group-page-break") );
                if ( s == QLatin1String("after-footer") ) {
                    dgsd->pagebreak = KReportDetailGroupSectionData::PageBreak::AfterGroupFooter;
                } else if ( s == QLatin1String("before-header") ) {
                    dgsd->pagebreak = KReportDetailGroupSectionData::PageBreak::BeforeGroupHeader;
                } else {
                    dgsd->pagebreak = KReportDetailGroupSectionData::PageBreak::None;
                }
            }

            if (elemThis.attribute(QLatin1String("report:group-sort"), QLatin1String("ascending")) == QLatin1String("ascending")) {
                dgsd->m_sort = Qt::AscendingOrder;
            } else {
                dgsd->m_sort = Qt::DescendingOrder;
            }
            
            for ( QDomElement e = elemThis.firstChildElement( QLatin1String("report:section") ); ! e.isNull(); e = e.nextSiblingElement( QLatin1String("report:section") ) ) {
                const QString s = KReportUtils::readSectionTypeNameAttribute(e);
                if ( s == QLatin1String("group-header") ) {
                    KReportSectionData * sd = new KReportSectionData(e, report);
                    if (sd->isValid()) {
                        dgsd->groupHeader = sd;
                    } else {
                        delete sd;
                    }
                } else if ( s == QLatin1String("group-footer") ) {
                    KReportSectionData * sd = new KReportSectionData(e, report);
                    if (sd->isValid()) {
                        dgsd->groupFooter = sd;
                    } else {
                        delete sd;
                    }
                }
            }
            groupList.append(dgsd);
            KReportDataSource::SortedField s;
            s.setField(dgsd->column);
            s.setOrder(dgsd->m_sort);

            sortedFields.append(s);
        } else if (elemThis.tagName() == QLatin1String("report:section")
                   && KReportUtils::readSectionTypeNameAttribute(elemThis)
                       == QLatin1String("detail"))
        {
            KReportSectionData *sd = new KReportSectionData(elemThis, report);
            if (sd->isValid()) {
                detailSection = sd;
            } else
                delete sd;
        } else {
            kreportWarning() << "While parsing detail section encountered an unknown element: " << elemThis.tagName();
        }
    }
    
    m_valid = true;
}

KReportDetailSectionData::~KReportDetailSectionData()
{
}

KReportDetailGroupSectionData::KReportDetailGroupSectionData()
{
    pagebreak = PageBreak::None;
    m_sort = Qt::AscendingOrder;
    groupHeader = nullptr;
    groupFooter = nullptr;
}

