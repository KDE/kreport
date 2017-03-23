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

#include "KReportDesignerSectionDetailGroup.h"
#include "KReportDesigner.h"
#include "KReportDesignerSection.h"
#include "KReportDesignerSectionDetail.h"
#include "kreport_debug.h"

#include <QDomElement>
#include <QDomDocument>

//! @internal
class Q_DECL_HIDDEN KReportDesignerSectionDetailGroup::Private
{
public:
    explicit Private() {}

    ~Private()
    {
        //Delete these here so that there are no widgets
        //left floating around
        delete groupHeader;
        delete groupFooter;
    }

    QString column;
    KReportDesignerSection *groupHeader;
    KReportDesignerSection *groupFooter;
    KReportDesignerSectionDetail * reportSectionDetail;
    PageBreak pageBreak = KReportDesignerSectionDetailGroup::BreakNone;
    Qt::SortOrder sort = Qt::AscendingOrder;
};

KReportDesignerSectionDetailGroup::KReportDesignerSectionDetailGroup(const QString & column, KReportDesignerSectionDetail * rsd,
                                                   QWidget * parent)
        : QObject(parent)
        , d(new Private())
{
    Q_ASSERT(rsd);
    d->reportSectionDetail = rsd;
    if (!d->reportSectionDetail) {
        kreportWarning() << "Error: ReportSectionDetail is null";
        return;
    }
    KReportDesigner * rd = rsd->reportDesigner();
    d->groupHeader = rd->createSection();
    d->groupFooter = rd->createSection();
    setGroupHeaderVisible(false);
    setGroupFooterVisible(false);
    setColumn(column);
}

KReportDesignerSectionDetailGroup::~KReportDesignerSectionDetailGroup()
{
    delete d;
}

void KReportDesignerSectionDetailGroup::buildXML(QDomDocument *doc, QDomElement *section) const
{
    QDomElement grp = doc->createElement(QLatin1String("report:group"));

    grp.setAttribute(QLatin1String("report:group-column"), column());
    if (pageBreak() == KReportDesignerSectionDetailGroup::BreakAfterGroupFooter) {
        grp.setAttribute(QLatin1String("report:group-page-break"), QLatin1String("after-footer"));
    } else if (pageBreak() == KReportDesignerSectionDetailGroup::BreakBeforeGroupHeader) {
        grp.setAttribute(QLatin1String("report:group-page-break"), QLatin1String("before-header"));
    }

    if (d->sort == Qt::AscendingOrder) {
        grp.setAttribute(QLatin1String("report:group-sort"), QLatin1String("ascending"));
    }
    else {
        grp.setAttribute(QLatin1String("report:group-sort"), QLatin1String("descending"));
    }

    //group head
    if (groupHeaderVisible()) {
        QDomElement gheader = doc->createElement(QLatin1String("report:section"));
        gheader.setAttribute(QLatin1String("report:section-type"), QLatin1String("group-header"));
        groupHeader()->buildXML(doc, &gheader);
        grp.appendChild(gheader);
    }
    // group foot
    if (groupFooterVisible()) {
        QDomElement gfooter = doc->createElement(QLatin1String("report:section"));
        gfooter.setAttribute(QLatin1String("report:section-type"), QLatin1String("group-footer"));
        groupFooter()->buildXML(doc, &gfooter);
        grp.appendChild(gfooter);
    }
    section->appendChild(grp);
}

void KReportDesignerSectionDetailGroup::initFromXML( const QDomElement &element )
{
    if ( element.hasAttribute(QLatin1String("report:group-column") ) ) {
        setColumn( element.attribute( QLatin1String("report:group-column") ) );
    }

    if ( element.hasAttribute( QLatin1String("report:group-page-break") ) ) {
        QString s = element.attribute( QLatin1String("report:group-page-break") );
        if ( s == QLatin1String("after-footer") ) {
            setPageBreak( KReportDesignerSectionDetailGroup::BreakAfterGroupFooter );
        } else if ( s == QLatin1String("before-header") ) {
            setPageBreak( KReportDesignerSectionDetailGroup::BreakBeforeGroupHeader );
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
            d->groupHeader->initFromXML( e );
        } else if ( s == QLatin1String("group-footer") ) {
            setGroupFooterVisible( true );
            d->groupFooter->initFromXML( e );
        }
    }
}

void KReportDesignerSectionDetailGroup::setGroupHeaderVisible(bool visible)
{
    if (groupHeaderVisible() != visible) {
        if (d->reportSectionDetail && d->reportSectionDetail->reportDesigner()) {
            d->reportSectionDetail->reportDesigner()->setModified(true);
        }
    }
    d->groupHeader->setVisible(visible);
    if (d->reportSectionDetail) {
        d->reportSectionDetail->adjustSize();
    }
}

void KReportDesignerSectionDetailGroup::setGroupFooterVisible(bool visible)
{
    if (groupFooterVisible() != visible) {
        if (d->reportSectionDetail && d->reportSectionDetail->reportDesigner()) {
            d->reportSectionDetail->reportDesigner()->setModified(true);
        }
    }
    d->groupFooter->setVisible(visible);
    if (d->reportSectionDetail) {
        d->reportSectionDetail->adjustSize();
    }
}

void KReportDesignerSectionDetailGroup::setPageBreak(KReportDesignerSectionDetailGroup::PageBreak pb)
{
    d->pageBreak = pb;
}

void KReportDesignerSectionDetailGroup::setSort(Qt::SortOrder s)
{
    d->sort = s;
}

Qt::SortOrder KReportDesignerSectionDetailGroup::sort()
{
    return d->sort;
}


bool KReportDesignerSectionDetailGroup::groupHeaderVisible() const
{
    // Check *explicitly* hidden
    return ! d->groupHeader->isHidden();
}
bool KReportDesignerSectionDetailGroup::groupFooterVisible() const
{
    // Check *explicitly* hidden
    return ! d->groupFooter->isHidden();
}
KReportDesignerSectionDetailGroup::PageBreak KReportDesignerSectionDetailGroup::pageBreak() const
{
    return d->pageBreak;
}

QString KReportDesignerSectionDetailGroup::column() const
{
    return d->column;
}
void KReportDesignerSectionDetailGroup::setColumn(const QString & s)
{
    if (d->column != s) {
        d->column = s;
        if (d->reportSectionDetail && d->reportSectionDetail->reportDesigner()) d->reportSectionDetail->reportDesigner()->setModified(true);
    }

    d->groupHeader->setTitle(d->column + QLatin1String(" Group Header"));
    d->groupFooter->setTitle(d->column + QLatin1String(" Group Footer"));
}

KReportDesignerSection * KReportDesignerSectionDetailGroup::groupHeader() const
{
    return d->groupHeader;
}
KReportDesignerSection * KReportDesignerSectionDetailGroup::groupFooter() const
{
    return d->groupFooter;
}




