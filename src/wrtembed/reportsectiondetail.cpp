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

#include "reportsectiondetail.h"
#include "reportsectiondetailgroup.h"
#include "reportsection.h"

#include "KoReportDesigner.h"

#include <QVBoxLayout>
#include <QDomDocument>
#include "kreport_debug.h"

//! @internal
class ReportSectionDetail::Private
{
public:
    explicit Private()
        : pageBreak(ReportSectionDetail::BreakNone)
    {}

    ~Private()
    {
    }

    QString name;
    ReportSection *detail;
    KoReportDesigner *reportDesigner;

    QList<ReportSectionDetailGroup*> groupList;

    QVBoxLayout *vboxlayout;

    int pageBreak;
};

ReportSectionDetail::ReportSectionDetail(KoReportDesigner * rptdes)
        : QWidget(rptdes)
        , d(new Private())
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->vboxlayout = new QVBoxLayout(this);
    d->vboxlayout->setSpacing(0);
    d->vboxlayout->setMargin(0);
    d->reportDesigner = rptdes;
    d->detail = new ReportSection(rptdes /*, this*/);
    d->vboxlayout->addWidget(d->detail);

    this->setLayout(d->vboxlayout);
}

ReportSectionDetail::~ReportSectionDetail()
{
    delete d;
}

int ReportSectionDetail::pageBreak() const
{
    return d->pageBreak;
}
void ReportSectionDetail::setPageBreak(int pb)
{
    d->pageBreak = pb;
}

ReportSection * ReportSectionDetail::detailSection() const
{
    return d->detail;
}

void ReportSectionDetail::buildXML(QDomDocument & doc, QDomElement & section)
{
    if (pageBreak() != ReportSectionDetail::BreakNone) {
        QDomElement spagebreak = doc.createElement(QLatin1String("pagebreak"));
        if (pageBreak() == ReportSectionDetail::BreakAtEnd)
            spagebreak.setAttribute(QLatin1String("when"), QLatin1String("at end"));
        section.appendChild(spagebreak);
    }

    foreach(ReportSectionDetailGroup* rsdg, d->groupList) {
        rsdg->buildXML(doc, section);
    }

    // detail section
    QDomElement gdetail = doc.createElement(QLatin1String("report:section"));
    gdetail.setAttribute(QLatin1String("report:section-type"), QLatin1String("detail"));
    d->detail->buildXML(doc, gdetail);
    section.appendChild(gdetail);
}

void ReportSectionDetail::initFromXML(QDomNode & section)
{
    QDomNodeList nl = section.childNodes();
    QDomNode node;
    QString n;

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();
        //kreportDebug() << n;
        if (n == QLatin1String("pagebreak")) {
            QDomElement eThis = node.toElement();
            if (eThis.attribute(QLatin1String("when")) == QLatin1String("at end"))
                setPageBreak(BreakAtEnd);
        } else if (n == QLatin1String("report:group")) {
            ReportSectionDetailGroup * rsdg = new ReportSectionDetailGroup(QLatin1String("unnamed"), this, this);
            rsdg->initFromXML( node.toElement() );
            insertGroupSection(groupSectionCount(), rsdg);
        } else if (n == QLatin1String("report:section") && node.toElement().attribute(QLatin1String("report:section-type")) == QLatin1String("detail")) {
            //kreportDebug() << "Creating detail section";
            d->detail->initFromXML(node);
        } else {
            // unknown element
            kreportWarning() << "while parsing section encountered and unknown element: " <<  n;
        }
    }

}

KoReportDesigner * ReportSectionDetail::reportDesigner() const
{
    return d->reportDesigner;
}

int ReportSectionDetail::groupSectionCount() const
{
    return d->groupList.count();
}

ReportSectionDetailGroup * ReportSectionDetail::groupSection(int i) const
{
    return d->groupList.at(i);
}

void ReportSectionDetail::insertGroupSection(int idx, ReportSectionDetailGroup * rsd)
{
    d->groupList.insert(idx, rsd);

    rsd->groupHeader()->setParent(this);
    rsd->groupFooter()->setParent(this);

    idx = 0;
    int gi = 0;
    for (gi = 0; gi < (int) d->groupList.count(); gi++) {
        rsd = d->groupList.at(gi);
        d->vboxlayout->removeWidget(rsd->groupHeader());
        d->vboxlayout->insertWidget(idx, rsd->groupHeader());
        idx++;
    }
    d->vboxlayout->removeWidget(d->detail);
    d->vboxlayout->insertWidget(idx, d->detail);
    idx++;
    for (gi = ((int) d->groupList.count() - 1); gi >= 0; --gi) {
        rsd = d->groupList.at(gi);
        d->vboxlayout->removeWidget(rsd->groupFooter());
        d->vboxlayout->insertWidget(idx, rsd->groupFooter());
        idx++;
    }

    if (d->reportDesigner) d->reportDesigner->setModified(true);
    adjustSize();
}

int ReportSectionDetail::indexOfGroupSection(const QString & column) const
{
    // find the item by its name
    for (uint i = 0; i < (uint)d->groupList.count(); i++) {
        ReportSectionDetailGroup * rsd = d->groupList.at(i);
        if (column == rsd->column()) return i;
    }
    return -1;
}

void ReportSectionDetail::removeGroupSection(int idx, bool del)
{
    ReportSectionDetailGroup * rsd = d->groupList.at(idx);

    d->vboxlayout->removeWidget(rsd->groupHeader());
    d->vboxlayout->removeWidget(rsd->groupFooter());

    d->groupList.removeAt(idx);

    if (d->reportDesigner) d->reportDesigner->setModified(true);
    if (del) delete rsd;
    adjustSize();
}

QSize ReportSectionDetail::sizeHint() const
{
    QSize s;
    foreach(ReportSectionDetailGroup* rsdg, d->groupList) {
        if (rsdg->groupHeaderVisible()) s += rsdg->groupHeader()->size();
        if (rsdg->groupFooterVisible()) s += rsdg->groupFooter()->size();
    }
    return s += d->detail->size();
}

void ReportSectionDetail::setSectionCursor(const QCursor& c)
{
    if (d->detail)
        d->detail->setSectionCursor(c);
    foreach(ReportSectionDetailGroup* rsdg, d->groupList) {
        if (rsdg->groupHeader())
            rsdg->groupHeader()->setSectionCursor(c);
        if (rsdg->groupFooter())
            rsdg->groupFooter()->setSectionCursor(c);
    }
}

void ReportSectionDetail::unsetSectionCursor()
{
    if (d->detail)
        d->detail->unsetSectionCursor();

    foreach(ReportSectionDetailGroup* rsdg, d->groupList) {
        if (rsdg->groupHeader())
            rsdg->groupHeader()->unsetSectionCursor();
        if (rsdg->groupFooter())
            rsdg->groupFooter()->unsetSectionCursor();
    }
}

