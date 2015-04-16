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

#include "reportsectiondetailgroup.h"
#include "KoReportDesigner.h"
#include "reportsection.h"
#include "reportsectiondetail.h"

#include <QDomElement>
#include <QDomDocument>

ReportSectionDetailGroup::ReportSectionDetailGroup(const QString & column, ReportSectionDetail * rsd,
                                                   QWidget * parent)
        : QObject(parent)
{
    m_pageBreak = BreakNone;
    m_sort = Qt::AscendingOrder;
    KoReportDesigner * rd = 0;
    m_reportSectionDetail = rsd;
    if (m_reportSectionDetail) {
        rd = rsd->reportDesigner();
    } else {
        qWarning() << "Error: ReportSectionDetail is null";
    }
    m_groupHeader = new ReportSection(rd /*, _rsd*/);
    m_groupFooter = new ReportSection(rd /*, _rsd*/);
    setGroupHeaderVisible(false);
    setGroupFooterVisible(false);

    setColumn(column);
}

ReportSectionDetailGroup::~ReportSectionDetailGroup()
{
    // I delete these here so that there are no widgets
    //left floating around
    delete m_groupHeader;
    delete m_groupFooter;
}

void ReportSectionDetailGroup::buildXML(QDomDocument & doc, QDomElement & section) const
{
    QDomElement grp = doc.createElement(QLatin1String("report:group"));

    grp.setAttribute(QLatin1String("report:group-column"), column());
    if (pageBreak() == ReportSectionDetailGroup::BreakAfterGroupFooter) {
        grp.setAttribute(QLatin1String("report:group-page-break"), QLatin1String("after-footer"));
    } else if (pageBreak() == ReportSectionDetailGroup::BreakBeforeGroupHeader) {
        grp.setAttribute(QLatin1String("report:group-page-break"), QLatin1String("before-header"));
    }

    if (m_sort == Qt::AscendingOrder) {
        grp.setAttribute(QLatin1String("report:group-sort"), QLatin1String("ascending"));
    }
    else {
        grp.setAttribute(QLatin1String("report:group-sort"), QLatin1String("descending"));
    }

    //group head
    if (groupHeaderVisible()) {
        QDomElement gheader = doc.createElement(QLatin1String("report:section"));
        gheader.setAttribute(QLatin1String("report:section-type"), QLatin1String("group-header"));
        groupHeader()->buildXML(doc, gheader);
        grp.appendChild(gheader);
    }
    // group foot
    if (groupFooterVisible()) {
        QDomElement gfooter = doc.createElement(QLatin1String("report:section"));
        gfooter.setAttribute(QLatin1String("report:section-type"), QLatin1String("group-footer"));
        groupFooter()->buildXML(doc, gfooter);
        grp.appendChild(gfooter);
    }
    section.appendChild(grp);
}

void ReportSectionDetailGroup::initFromXML( const QDomElement &element )
{
    if ( element.hasAttribute(QLatin1String("report:group-column") ) ) {
        setColumn( element.attribute( QLatin1String("report:group-column") ) );
    }

    if ( element.hasAttribute( QLatin1String("report:group-page-break") ) ) {
        QString s = element.attribute( QLatin1String("report:group-page-break") );
        if ( s == QLatin1String("after-footer") ) {
            setPageBreak( ReportSectionDetailGroup::BreakAfterGroupFooter );
        } else if ( s == QLatin1String("before-header") ) {
            setPageBreak( ReportSectionDetailGroup::BreakBeforeGroupHeader );
        }
    }

    if (element.attribute(QLatin1String("report:group-sort"), QLatin1String("ascending")) == QLatin1String("ascending")) {
        setSort(Qt::AscendingOrder);
    }
    else {
        setSort(Qt::DescendingOrder);
    }

    for ( QDomElement e = element.firstChildElement( QLatin1String("report:section") ); ! e.isNull(); e = e.nextSiblingElement( QLatin1String("report:section") ) ) {
        QString s = e.attribute( QLatin1String("report:section-type") );
        if ( s == QLatin1String("group-header") ) {
            setGroupHeaderVisible( true );
            m_groupHeader->initFromXML( e );
        } else if ( s == QLatin1String("group-footer") ) {
            setGroupFooterVisible( true );
            m_groupFooter->initFromXML( e );
        }
    }
}

void ReportSectionDetailGroup::setGroupHeaderVisible(bool yes)
{
    if (groupHeaderVisible() != yes) {
        if (m_reportSectionDetail && m_reportSectionDetail->reportDesigner()) m_reportSectionDetail->reportDesigner()->setModified(true);
    }
    if (yes) m_groupHeader->show();
    else m_groupHeader->hide();
    m_reportSectionDetail->adjustSize();
}

void ReportSectionDetailGroup::setGroupFooterVisible(bool yes)
{
    if (groupFooterVisible() != yes) {
        if (m_reportSectionDetail && m_reportSectionDetail->reportDesigner()) m_reportSectionDetail->reportDesigner()->setModified(true);
    }
    if (yes) m_groupFooter->show();
    else m_groupFooter->hide();
    m_reportSectionDetail->adjustSize();
}

void ReportSectionDetailGroup::setPageBreak(ReportSectionDetailGroup::PageBreak pb)
{
    m_pageBreak = pb;
}

void ReportSectionDetailGroup::setSort(Qt::SortOrder s)
{
    m_sort = s;
}

Qt::SortOrder ReportSectionDetailGroup::sort()
{
    return m_sort;
}


bool ReportSectionDetailGroup::groupHeaderVisible() const
{
    // Check *explicitly* hidden
    return ! m_groupHeader->isHidden();
}
bool ReportSectionDetailGroup::groupFooterVisible() const
{
    // Check *explicitly* hidden
    return ! m_groupFooter->isHidden();
}
ReportSectionDetailGroup::PageBreak ReportSectionDetailGroup::pageBreak() const
{
    return m_pageBreak;
}

QString ReportSectionDetailGroup::column() const
{
    return m_column;
}
void ReportSectionDetailGroup::setColumn(const QString & s)
{
    if (m_column != s) {
        m_column = s;
        if (m_reportSectionDetail && m_reportSectionDetail->reportDesigner()) m_reportSectionDetail->reportDesigner()->setModified(true);
    }

    m_groupHeader->setTitle(m_column + QLatin1String(" Group Header"));
    m_groupFooter->setTitle(m_column + QLatin1String(" Group Footer"));
}

ReportSection * ReportSectionDetailGroup::groupHeader() const
{
    return m_groupHeader;
}
ReportSection * ReportSectionDetailGroup::groupFooter() const
{
    return m_groupFooter;
}




