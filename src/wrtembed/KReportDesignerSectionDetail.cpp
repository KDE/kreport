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

#include "KReportDesignerSectionDetail.h"
#include "KReportDesignerSectionDetailGroup.h"
#include "KReportDesignerSection.h"
#include "KReportDesigner.h"
#include "KReportUtils.h"
#include "kreport_debug.h"

#include <QVBoxLayout>
#include <QDomDocument>

//! @internal
class Q_DECL_HIDDEN KReportDesignerSectionDetail::Private
{
public:
    explicit Private() {}

    ~Private()
    {
    }

    QString name;
    KReportDesignerSection *detail;
    KReportDesigner *reportDesigner;
    QList<KReportDesignerSectionDetailGroup*> groupList;
    QVBoxLayout *vboxlayout;
    KReportDesignerSectionDetail::PageBreak pageBreak = KReportDesignerSectionDetail::PageBreak::None;
};

KReportDesignerSectionDetail::KReportDesignerSectionDetail(KReportDesigner * rptdes)
        : QWidget(rptdes)
        , d(new Private())
{
    Q_ASSERT(rptdes);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->vboxlayout = new QVBoxLayout(this);
    d->vboxlayout->setSpacing(0);
    d->vboxlayout->setMargin(0);
    d->reportDesigner = rptdes;
    d->detail = d->reportDesigner->createSection();
    d->vboxlayout->addWidget(d->detail);

    this->setLayout(d->vboxlayout);
}

KReportDesignerSectionDetail::~KReportDesignerSectionDetail()
{
    delete d;
}

KReportDesignerSectionDetail::PageBreak KReportDesignerSectionDetail::pageBreak() const
{
    return d->pageBreak;
}
void KReportDesignerSectionDetail::setPageBreak(PageBreak pb)
{
    d->pageBreak = pb;
}

KReportDesignerSection * KReportDesignerSectionDetail::detailSection() const
{
    return d->detail;
}

void KReportDesignerSectionDetail::buildXML(QDomDocument *doc, QDomElement *section)
{
    if (pageBreak() != KReportDesignerSectionDetail::PageBreak::None) {
        QDomElement spagebreak = doc->createElement(QLatin1String("pagebreak"));
        if (pageBreak() == KReportDesignerSectionDetail::PageBreak::AtEnd)
            spagebreak.setAttribute(QLatin1String("when"), QLatin1String("at end"));
        section->appendChild(spagebreak);
    }

    foreach(KReportDesignerSectionDetailGroup* rsdg, d->groupList) {
        rsdg->buildXML(doc, section);
    }

    // detail section
    QDomElement gdetail = doc->createElement(QLatin1String("report:section"));
    gdetail.setAttribute(QLatin1String("report:section-type"), QLatin1String("detail"));
    d->detail->buildXML(doc, &gdetail);
    section->appendChild(gdetail);
}

void KReportDesignerSectionDetail::initFromXML(QDomNode *section)
{
    QDomNodeList nl = section->childNodes();
    QDomNode node;
    QString n;

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();
        //kreportDebug() << n;
        if (n == QLatin1String("pagebreak")) {
            QDomElement eThis = node.toElement();
            if (eThis.attribute(QLatin1String("when")) == QLatin1String("at end"))
                setPageBreak(PageBreak::AtEnd);
        } else if (n == QLatin1String("report:group")) {
            KReportDesignerSectionDetailGroup * rsdg = new KReportDesignerSectionDetailGroup(QLatin1String("unnamed"), this, this);
            rsdg->initFromXML( node.toElement() );
            insertGroupSection(groupSectionCount(), rsdg);
        } else if (n == QLatin1String("report:section")
            && KReportUtils::readSectionTypeNameAttribute(node.toElement()) == QLatin1String("detail"))
        {
            // kreportDebug() << "Creating detail section";
            d->detail->initFromXML(node);
        } else {
            // unknown element
            kreportWarning() << "while parsing section encountered and unknown element: " <<  n;
        }
    }

}

KReportDesigner * KReportDesignerSectionDetail::reportDesigner() const
{
    return d->reportDesigner;
}

int KReportDesignerSectionDetail::groupSectionCount() const
{
    return d->groupList.count();
}

KReportDesignerSectionDetailGroup * KReportDesignerSectionDetail::groupSection(int i) const
{
    return d->groupList.at(i);
}

void KReportDesignerSectionDetail::insertGroupSection(int idx, KReportDesignerSectionDetailGroup * rsd)
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

int KReportDesignerSectionDetail::indexOfGroupSection(const QString & column) const
{
    // find the item by its name
    for (uint i = 0; i < (uint)d->groupList.count(); i++) {
        KReportDesignerSectionDetailGroup * rsd = d->groupList.at(i);
        if (column == rsd->column()) return i;
    }
    return -1;
}

void KReportDesignerSectionDetail::removeGroupSection(int idx, bool del)
{
    KReportDesignerSectionDetailGroup * rsd = d->groupList.at(idx);

    d->vboxlayout->removeWidget(rsd->groupHeader());
    d->vboxlayout->removeWidget(rsd->groupFooter());

    d->groupList.removeAt(idx);

    if (d->reportDesigner) d->reportDesigner->setModified(true);
    if (del) delete rsd;
    adjustSize();
}

QSize KReportDesignerSectionDetail::sizeHint() const
{
    QSize s;
    foreach(KReportDesignerSectionDetailGroup* rsdg, d->groupList) {
        if (rsdg->groupHeaderVisible()) s += rsdg->groupHeader()->size();
        if (rsdg->groupFooterVisible()) s += rsdg->groupFooter()->size();
    }
    return s += d->detail->size();
}

void KReportDesignerSectionDetail::setSectionCursor(const QCursor& c)
{
    if (d->detail)
        d->detail->setSectionCursor(c);
    foreach(KReportDesignerSectionDetailGroup* rsdg, d->groupList) {
        if (rsdg->groupHeader())
            rsdg->groupHeader()->setSectionCursor(c);
        if (rsdg->groupFooter())
            rsdg->groupFooter()->setSectionCursor(c);
    }
}

void KReportDesignerSectionDetail::unsetSectionCursor()
{
    if (d->detail)
        d->detail->unsetSectionCursor();

    foreach(KReportDesignerSectionDetailGroup* rsdg, d->groupList) {
        if (rsdg->groupHeader())
            rsdg->groupHeader()->unsetSectionCursor();
        if (rsdg->groupFooter())
            rsdg->groupFooter()->unsetSectionCursor();
    }
}
