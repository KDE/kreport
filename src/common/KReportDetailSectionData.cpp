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

#include "kreport_debug.h"
#include <QDomElement>

KReportDetailSectionData::KReportDetailSectionData(QObject *parent)
 : QObject(parent)
{
    m_pageBreak = BreakNone;
    m_detailSection = 0;
    m_valid = true;
}

KReportDetailSectionData::KReportDetailSectionData(const QDomElement &elemSource, KReportDocument *report)
 : QObject(report)
{
    m_pageBreak = BreakNone;
    m_detailSection = 0;
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
                dgsd->m_column = elemThis.attribute( QLatin1String("report:group-column") );
            }

            if ( elemThis.hasAttribute( QLatin1String("report:group-page-break") ) ) {
                QString s = elemThis.attribute( QLatin1String("report:group-page-break") );
                if ( s == QLatin1String("after-footer") ) {
                    dgsd->m_pagebreak = KReportDetailGroupSectionData::BreakAfterGroupFooter;
                } else if ( s == QLatin1String("before-header") ) {
                    dgsd->m_pagebreak = KReportDetailGroupSectionData::BreakBeforeGroupHeader;
                } else {
                    dgsd->m_pagebreak = KReportDetailGroupSectionData::BreakNone;
                }
            }

            if (elemThis.attribute(QLatin1String("report:group-sort"), QLatin1String("ascending")) == QLatin1String("ascending")) {
                dgsd->m_sort = Qt::AscendingOrder;
            } else {
                dgsd->m_sort = Qt::DescendingOrder;
            }
            
            for ( QDomElement e = elemThis.firstChildElement( QLatin1String("report:section") ); ! e.isNull(); e = e.nextSiblingElement( QLatin1String("report:section") ) ) {
                QString s = e.attribute( QLatin1String("report:section-type") );
                if ( s == QLatin1String("group-header") ) {
                    KReportSectionData * sd = new KReportSectionData(e, report);
                    if (sd->isValid()) {
                        dgsd->m_groupHeader = sd;
                    } else {
                        delete sd;
                    }
                } else if ( s == QLatin1String("group-footer") ) {
                    KReportSectionData * sd = new KReportSectionData(e, report);
                    if (sd->isValid()) {
                        dgsd->m_groupFooter = sd;
                    } else {
                        delete sd;
                    }
                }
            }
            m_groupList.append(dgsd);
            KReportData::SortedField s;
            s.setField(dgsd->m_column);
            s.setOrder(dgsd->m_sort);
            m_sortedFields.append(s);
	    
        } else if (elemThis.tagName() == QLatin1String("report:section") && elemThis.attribute(QLatin1String("report:section-type")) == QLatin1String("detail")) {
            KReportSectionData * sd = new KReportSectionData(elemThis, report);
            if (sd->isValid()) {
                m_detailSection = sd;
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
    m_pagebreak = BreakNone;
    m_sort = Qt::AscendingOrder;
    m_groupHeader = 0;
    m_groupFooter = 0;
}

