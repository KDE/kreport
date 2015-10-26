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

#include "KReportDocument.h"
#include "KReportUnit.h"
#include "KReportDetailSectionData.h"
#include "KReportItemBase.h"

#include <QDomElement>
#include <QApplication>
#include <QScreen>
#include "kreport_debug.h"

void KReportDocument::init()
{
    m_pageHeaderFirst = m_pageHeaderOdd = m_pageHeaderEven = m_pageHeaderLast = m_pageHeaderAny = 0;
    m_pageFooterFirst = m_pageFooterOdd = m_pageFooterEven = m_pageFooterLast = m_pageFooterAny = 0;
    m_reportHeader = m_reportFooter = 0;
}

KReportDocument::KReportDocument(QObject *parent)
        : QObject(parent)
        , m_detailSection(0)
{
    init();
    m_valid = true;
}

KReportDocument::KReportDocument(const QDomElement & elemSource, QObject *parent)
        : QObject(parent)
        , m_detailSection(0)
{
    m_valid = false;
    init();
    //kreportDebug();
    if (elemSource.tagName() != QLatin1String("report:content")) {
        kreportWarning() << "QDomElement is not <report:content> tag"
                   << elemSource.text();
        return;
    }
        QScreen *srn = QApplication::screens().at(0);

    const qreal dpiX = srn->logicalDotsPerInchX();
    const qreal dpiY = srn->logicalDotsPerInchY();


    QDomNodeList sections = elemSource.childNodes();
    for (int nodeCounter = 0; nodeCounter < sections.count(); nodeCounter++) {
        QDomElement elemThis = sections.item(nodeCounter).toElement();
        if (elemThis.tagName() == QLatin1String("report:title")) {
            m_title = elemThis.text();
#ifdef KREPORT_SCRIPTING
        } else if (elemThis.tagName() == QLatin1String("report:script")) {
            m_script = elemThis.text();
            m_interpreter = elemThis.attribute(QLatin1String("report:script-interpreter"));
#endif
        } else if (elemThis.tagName() == QLatin1String("report:page-style")) {
            QString pagetype = elemThis.firstChild().nodeValue();

            if (pagetype == QLatin1String("predefined")) {
                page.setPageSize(elemThis.attribute(QLatin1String("report:page-size"), QLatin1String("A4")));
            } else if (pagetype == QLatin1String("custom")) {
                page.setCustomWidth(POINT_TO_INCH(KReportUnit::parseValue(elemThis.attribute(QLatin1String("report:custom-page-width"), QString()))) * dpiX);
                page.setCustomHeight(POINT_TO_INCH(KReportUnit::parseValue(elemThis.attribute(QLatin1String("report:custom-page-height"), QString()))) * dpiY);
                page.setPageSize(QLatin1String("Custom"));
            } else if (pagetype == QLatin1String("label")) {
                page.setLabelType(elemThis.firstChild().nodeValue());
            }
            //! @todo add config for default margins or add within templates support
            page.setMarginBottom(POINT_TO_INCH(KReportUnit::parseValue(elemThis.attribute(QLatin1String("fo:margin-bottom"), QLatin1String("1.0cm")))) * dpiY);
            page.setMarginTop(POINT_TO_INCH(KReportUnit::parseValue(elemThis.attribute(QLatin1String("fo:margin-top"), QLatin1String("1.0cm")))) * dpiY);
            page.setMarginLeft(POINT_TO_INCH(KReportUnit::parseValue(elemThis.attribute(QLatin1String("fo:margin-left"), QLatin1String("1.0cm")))) * dpiX);
            page.setMarginRight(POINT_TO_INCH(KReportUnit::parseValue(elemThis.attribute(QLatin1String("fo:margin-right"), QLatin1String("1.0cm")))) * dpiX);

            page.setPortrait(elemThis.attribute(QLatin1String("report:print-orientation"), QLatin1String("portrait")) == QLatin1String("portrait"));

        } else if (elemThis.tagName() == QLatin1String("report:body")) {
            QDomNodeList sectionlist = elemThis.childNodes();
            QDomNode sec;

            for (int s = 0; s < sectionlist.count(); ++s) {
                sec = sectionlist.item(s);
                if (sec.isElement()) {
                    QString sn = sec.nodeName().toLower();
                    //kreportDebug() << sn;
                    if (sn == QLatin1String("report:section")) {
                        KReportSectionData * sd = new KReportSectionData(sec.toElement(), this);
                        if (!sd->isValid()) {
                            kreportDebug() << "Invalid section";
                            delete sd;
                        } else {
                            //kreportDebug() << "Adding section of type " << sd->type();
                            switch (sd->type()) {
                            case KReportSectionData::PageHeaderFirst:
                                m_pageHeaderFirst = sd;
                                break;
                            case KReportSectionData::PageHeaderOdd:
                                m_pageHeaderOdd = sd;
                                break;
                            case KReportSectionData::PageHeaderEven:
                                m_pageHeaderEven = sd;
                                break;
                            case KReportSectionData::PageHeaderLast:
                                m_pageHeaderLast = sd;
                                break;
                            case KReportSectionData::PageHeaderAny:
                                m_pageHeaderAny = sd;
                                break;
                            case KReportSectionData::ReportHeader:
                                m_reportHeader = sd;
                                break;
                            case KReportSectionData::ReportFooter:
                                m_reportFooter = sd;
                                break;
                            case KReportSectionData::PageFooterFirst:
                                m_pageFooterFirst = sd;
                                break;
                            case KReportSectionData::PageFooterOdd:
                                m_pageFooterOdd = sd;
                                break;
                            case KReportSectionData::PageFooterEven:
                                m_pageFooterEven = sd;
                                break;
                            case KReportSectionData::PageFooterLast:
                                m_pageFooterLast = sd;
                                break;
                            case KReportSectionData::PageFooterAny:
                                m_pageFooterAny = sd;
                                break;
                            default:
                                ;
                            }
                        }

                    } else if (sn == QLatin1String("report:detail")) {
                        KReportDetailSectionData * dsd = new KReportDetailSectionData(sec.toElement(), this);

                        if (dsd->isValid()) {
                            m_detailSection = dsd;
                        } else {
                            kreportDebug() << "Invalid detail section";
                            delete dsd;
                        }
                    }
                } else {
                    kreportWarning() << "Encountered an unknown Element: "  << elemThis.tagName();
                }
            }
        }

        m_valid = true;
    }
}


KReportDocument::~KReportDocument()
{
}

QList<KReportItemBase*> KReportDocument::objects() const
{
    QList<KReportItemBase*> obs;
    for (int i = 1; i <= KReportSectionData::PageFooterAny; i++) {
        KReportSectionData *sec = section((KReportSectionData::Section)i);
        if (sec) {
            obs << sec->objects();
        }
    }

    if (m_detailSection) {
        //kreportDebug() << "Number of groups: " << m_detailSection->m_groupList.count();
        foreach(KReportDetailGroupSectionData* g, m_detailSection->m_groupList) {
            if (g->m_groupHeader) {
                obs << g->m_groupHeader->objects();
            }
            if (g->m_groupFooter) {
                obs << g->m_groupFooter->objects();
            }
        }
        if (m_detailSection->m_detailSection)
            obs << m_detailSection->m_detailSection->objects();
    }

    /*kreportDebug() << "Object List:";
    foreach(KReportItemBase* o, obs) {
        kreportDebug() << o->entityName();
    }*/
    return obs;
}

KReportItemBase* KReportDocument::object(const QString& n) const
{
    QList<KReportItemBase*> obs = objects();

    foreach(KReportItemBase* o, obs) {
        if (o->entityName() == n) {
            return o;
        }
    }
    return 0;
}

QList<KReportSectionData*> KReportDocument::sections() const
{
    QList<KReportSectionData*> secs;
    for (int i = 0; i < 12 ; ++i) {
        KReportSectionData *sec = section((KReportSectionData::Section)(i + 1));
        if (sec) {
            secs << sec;
        }
    }

    if (m_detailSection) {
        //kreportDebug() << "Number of groups: " << m_detailSection->m_groupList.count();
        foreach(KReportDetailGroupSectionData* g, m_detailSection->m_groupList) {
            if (g->m_groupHeader) {
                secs << g->m_groupHeader;
            }
            if (g->m_groupFooter) {
                secs << g->m_groupFooter;
            }
        }
        if (m_detailSection->m_detailSection)
            secs << m_detailSection->m_detailSection;
    }

    return secs;
}

KReportSectionData* KReportDocument::section(const QString& sn) const
{
    QList<KReportSectionData*> secs = sections();

    foreach(KReportSectionData *sec, secs) {
        if (sec->name() == sn) {
            return sec;
        }
    }
    return 0;
}

KReportSectionData* KReportDocument::section(KReportSectionData::Section s) const
{
    KReportSectionData *sec;
    switch (s) {
    case KReportSectionData::PageHeaderAny:
        sec = m_pageHeaderAny;
        break;
    case KReportSectionData::PageHeaderEven:
        sec = m_pageHeaderEven;
        break;
    case KReportSectionData::PageHeaderOdd:
        sec = m_pageHeaderOdd;
        break;
    case KReportSectionData::PageHeaderFirst:
        sec = m_pageHeaderFirst;
        break;
    case KReportSectionData::PageHeaderLast:
        sec = m_pageHeaderLast;
        break;
    case KReportSectionData::PageFooterAny:
        sec = m_pageFooterAny;
        break;
    case KReportSectionData::PageFooterEven:
        sec = m_pageFooterEven;
        break;
    case KReportSectionData::PageFooterOdd:
        sec = m_pageFooterOdd;
        break;
    case KReportSectionData::PageFooterFirst:
        sec = m_pageFooterFirst;
        break;
    case KReportSectionData::PageFooterLast:
        sec = m_pageFooterLast;
        break;
    case KReportSectionData::ReportHeader:
        sec = m_reportHeader;
        break;
    case KReportSectionData::ReportFooter:
        sec = m_reportFooter;
        break;
    default:
        sec = 0;
    }
    return sec;
}

KReportPageOptions KReportDocument::pageOptions() const
{
    return page;
}
