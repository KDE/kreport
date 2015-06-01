/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>
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

#include "krsectiondata.h"

#include "KoReportPluginInterface.h"
#include "KoReportPluginManager.h"
#include "KoReportItemLine.h"
#include "kreport_debug.h"

#include <KPropertySet>

#include <QColor>

KRSectionData::KRSectionData(QObject* parent)
 : QObject(parent)
{
    createProperties(QDomElement());
}

KRSectionData::KRSectionData(const QDomElement & elemSource, KoReportReportData* report)
 : QObject(report)
{
    setObjectName(elemSource.tagName());

    if (objectName() != QLatin1String("report:section")) {
        m_valid = false;
        return;
    }

    m_type = sectionTypeFromString(elemSource.attribute(QLatin1String("report:section-type")));
    if (m_type == KRSectionData::None) {
        m_valid = false;
        return;
    }
    createProperties(elemSource);

    m_backgroundColor->setValue(QColor(elemSource.attribute(QLatin1String("fo:background-color"))));

    KoReportPluginManager* manager = KoReportPluginManager::self();

    QDomNodeList section = elemSource.childNodes();
    for (int nodeCounter = 0; nodeCounter < section.count(); nodeCounter++) {
        QDomElement elemThis = section.item(nodeCounter).toElement();
        QString n = elemThis.tagName();
        if (n.startsWith(QLatin1String("report:"))) {
            QString reportItemName = n.mid(qstrlen("report:"));
            if (reportItemName == QLatin1String("line")) {
                KoReportItemLine * line = new KoReportItemLine(elemThis);
                m_objects.append(line);
                continue;
            }
            KoReportPluginInterface *plugin = manager->plugin(reportItemName);
            if (plugin) {
                QObject *obj = plugin->createRendererInstance(elemThis);
                if (obj) {
                    KoReportItemBase *krobj = dynamic_cast<KoReportItemBase*>(obj);
                    if (krobj) {
                        m_objects.append(krobj);
                    }
                    continue;
                }
            }
        }
        kreportWarning() << "While parsing section encountered an unknown element: " << n;
    }
    qSort(m_objects.begin(), m_objects.end(), zLessThan);
    m_valid = true;
}

KRSectionData::~KRSectionData()
{
    delete m_set;
    qDeleteAll(m_objects);
}

bool KRSectionData::zLessThan(KoReportItemBase* s1, KoReportItemBase* s2)
{
    return s1->Z < s2->Z;
}

bool KRSectionData::xLessThan(KoReportItemBase* s1, KoReportItemBase* s2)
{
    return s1->position().toPoint().x() < s2->position().toPoint().x();
}

void KRSectionData::createProperties(const QDomElement & elemSource)
{
    m_set = new KPropertySet(this, QLatin1String("Section"));

    m_height = new KProperty("height", KoUnit(KoUnit::Centimeter).fromUserValue(2.0), tr("Height"));
    m_backgroundColor = new KProperty("background-color", QColor(Qt::white), tr("Background Color"));
    m_height->setOption("unit", QLatin1String("cm"));
    if (!elemSource.isNull())
        m_height->setValue(KoUnit::parseValue(elemSource.attribute(QLatin1String("svg:height"), QLatin1String("2.0cm"))));

    m_set->addProperty(m_height);
    m_set->addProperty(m_backgroundColor);
}

QString KRSectionData::name() const
{
    return (objectName() + QLatin1Char('-') + sectionTypeString(m_type));
}

QString KRSectionData::sectionTypeString(KRSectionData::Section s)
{
//! @todo use QMap
    QString sectiontype;
    switch (s) {
    case KRSectionData::PageHeaderAny:
        sectiontype = QLatin1String("header-page-any");
        break;
    case KRSectionData::PageHeaderEven:
        sectiontype = QLatin1String("header-page-even");
        break;
    case KRSectionData::PageHeaderOdd:
        sectiontype = QLatin1String("header-page-odd");
        break;
    case KRSectionData::PageHeaderFirst:
        sectiontype = QLatin1String("header-page-first");
        break;
    case KRSectionData::PageHeaderLast:
        sectiontype = QLatin1String("header-page-last");
        break;
    case KRSectionData::PageFooterAny:
        sectiontype = QLatin1String("footer-page-any");
        break;
    case KRSectionData::PageFooterEven:
        sectiontype = QLatin1String("footer-page-even");
        break;
    case KRSectionData::PageFooterOdd:
        sectiontype = QLatin1String("footer-page-odd");
        break;
    case KRSectionData::PageFooterFirst:
        sectiontype = QLatin1String("footer-page-first");
        break;
    case KRSectionData::PageFooterLast:
        sectiontype = QLatin1String("footer-page-last");
        break;
    case KRSectionData::ReportHeader:
        sectiontype = QLatin1String("header-report");
        break;
    case KRSectionData::ReportFooter:
        sectiontype = QLatin1String("footer-report");
        break;
    case KRSectionData::GroupHeader:
        sectiontype = QLatin1String("group-header");
        break;
    case KRSectionData::GroupFooter:
        sectiontype = QLatin1String("group-footer");
        break;
    case KRSectionData::Detail:
        sectiontype = QLatin1String("detail");
        break;
    default:
        ;
    }

    return sectiontype;
}

KRSectionData::Section KRSectionData::sectionTypeFromString(const QString& s)
{
//! @todo use QMap
    KRSectionData::Section sec;
    //kreportDebug() << "Determining section type for " << s;
    if (s == QLatin1String("header-page-any"))
        sec = KRSectionData::PageHeaderAny;
    else if (s == QLatin1String("header-page-even"))
        sec = KRSectionData::PageHeaderEven;
    else if (s == QLatin1String("header-page-odd"))
        sec = KRSectionData::PageHeaderOdd;
    else if (s == QLatin1String("header-page-first"))
        sec = KRSectionData::PageHeaderFirst;
    else if (s == QLatin1String("header-page-last"))
        sec = KRSectionData::PageHeaderLast;
    else if (s == QLatin1String("header-report"))
        sec = KRSectionData::ReportHeader;
    else if (s == QLatin1String("footer-page-any"))
        sec = KRSectionData::PageFooterAny;
    else if (s == QLatin1String("footer-page-even"))
        sec = KRSectionData::PageFooterEven;
    else if (s == QLatin1String("footer-page-odd"))
        sec = KRSectionData::PageFooterOdd;
    else if (s == QLatin1String("footer-page-first"))
        sec = KRSectionData::PageFooterFirst;
    else if (s == QLatin1String("footer-page-last"))
        sec = KRSectionData::PageFooterLast;
    else if (s == QLatin1String("footer-report"))
        sec = KRSectionData::ReportFooter;
    else if (s == QLatin1String("group-header"))
        sec = KRSectionData::GroupHeader;
    else if (s == QLatin1String("group-footer"))
        sec = KRSectionData::GroupFooter;
    else if (s == QLatin1String("detail"))
        sec = KRSectionData::Detail;
    else
        sec = KRSectionData::None;

    return sec;
}
