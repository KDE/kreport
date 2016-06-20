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

#include "KReportSectionData.h"
#include "KReportDocument.h"
#include "KReportPluginInterface.h"
#include "KReportPluginManager.h"
#include "KReportItemLine.h"
#include "KReportDesigner.h"
#include "kreport_debug.h"

#include <KPropertySet>

#include <QDomElement>

KReportSectionData::KReportSectionData(QObject* parent)
 : QObject(parent)
{
    createProperties(QDomElement());
}

KReportSectionData::KReportSectionData(const QDomElement & elemSource, KReportDocument* report)
 : QObject(report)
{
    setObjectName(elemSource.tagName());

    if (objectName() != QLatin1String("report:section")) {
        m_valid = false;
        return;
    }

    m_type = sectionTypeFromString(elemSource.attribute(QLatin1String("report:section-type")));
    if (m_type == KReportSectionData::None) {
        m_valid = false;
        return;
    }
    createProperties(elemSource);

    m_backgroundColor->setValue(QColor(elemSource.attribute(QLatin1String("fo:background-color"))));

    KReportPluginManager* manager = KReportPluginManager::self();

    QDomNodeList section = elemSource.childNodes();
    for (int nodeCounter = 0; nodeCounter < section.count(); nodeCounter++) {
        QDomElement elemThis = section.item(nodeCounter).toElement();
        QString n = elemThis.tagName();
        if (n.startsWith(QLatin1String("report:"))) {
            QString reportItemName = n.mid(qstrlen("report:"));
            if (reportItemName == QLatin1String("line")) {
                KReportItemLine * line = new KReportItemLine(elemThis);
                m_objects.append(line);
                continue;
            }
            KReportPluginInterface *plugin = manager->plugin(reportItemName);
            if (plugin) {
                QObject *obj = plugin->createRendererInstance(elemThis);
                if (obj) {
                    KReportItemBase *krobj = dynamic_cast<KReportItemBase*>(obj);
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

KReportSectionData::~KReportSectionData()
{
    delete m_set;
    qDeleteAll(m_objects);
}

bool KReportSectionData::zLessThan(KReportItemBase* s1, KReportItemBase* s2)
{
    return s1->Z < s2->Z;
}

bool KReportSectionData::xLessThan(KReportItemBase* s1, KReportItemBase* s2)
{
    return s1->position().toPoint().x() < s2->position().toPoint().x();
}

void KReportSectionData::createProperties(const QDomElement & elemSource)
{
    m_set = new KPropertySet(this);
    KReportDesigner::addMetaProperties(m_set,
        tr("Section", "Report section"), QLatin1String("kreport_section_element"));

    m_height = new KProperty("height", KReportUnit(KReportUnit::Centimeter).fromUserValue(2.0), tr("Height"));
    m_backgroundColor = new KProperty("background-color", QColor(Qt::white), tr("Background Color"));
    m_height->setOption("unit", QLatin1String("cm"));
    if (!elemSource.isNull())
        m_height->setValue(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:height"), QLatin1String("2.0cm"))));

    m_set->addProperty(m_height);
    m_set->addProperty(m_backgroundColor);
}

QString KReportSectionData::name() const
{
    return (objectName() + QLatin1Char('-') + sectionTypeString(m_type));
}

QString KReportSectionData::sectionTypeString(KReportSectionData::Section s)
{
//! @todo use QMap
    QString sectiontype;
    switch (s) {
    case KReportSectionData::PageHeaderAny:
        sectiontype = QLatin1String("header-page-any");
        break;
    case KReportSectionData::PageHeaderEven:
        sectiontype = QLatin1String("header-page-even");
        break;
    case KReportSectionData::PageHeaderOdd:
        sectiontype = QLatin1String("header-page-odd");
        break;
    case KReportSectionData::PageHeaderFirst:
        sectiontype = QLatin1String("header-page-first");
        break;
    case KReportSectionData::PageHeaderLast:
        sectiontype = QLatin1String("header-page-last");
        break;
    case KReportSectionData::PageFooterAny:
        sectiontype = QLatin1String("footer-page-any");
        break;
    case KReportSectionData::PageFooterEven:
        sectiontype = QLatin1String("footer-page-even");
        break;
    case KReportSectionData::PageFooterOdd:
        sectiontype = QLatin1String("footer-page-odd");
        break;
    case KReportSectionData::PageFooterFirst:
        sectiontype = QLatin1String("footer-page-first");
        break;
    case KReportSectionData::PageFooterLast:
        sectiontype = QLatin1String("footer-page-last");
        break;
    case KReportSectionData::ReportHeader:
        sectiontype = QLatin1String("header-report");
        break;
    case KReportSectionData::ReportFooter:
        sectiontype = QLatin1String("footer-report");
        break;
    case KReportSectionData::GroupHeader:
        sectiontype = QLatin1String("group-header");
        break;
    case KReportSectionData::GroupFooter:
        sectiontype = QLatin1String("group-footer");
        break;
    case KReportSectionData::Detail:
        sectiontype = QLatin1String("detail");
        break;
    default:
        ;
    }

    return sectiontype;
}

KReportSectionData::Section KReportSectionData::sectionTypeFromString(const QString& s)
{
//! @todo use QMap
    KReportSectionData::Section sec;
    //kreportDebug() << "Determining section type for " << s;
    if (s == QLatin1String("header-page-any"))
        sec = KReportSectionData::PageHeaderAny;
    else if (s == QLatin1String("header-page-even"))
        sec = KReportSectionData::PageHeaderEven;
    else if (s == QLatin1String("header-page-odd"))
        sec = KReportSectionData::PageHeaderOdd;
    else if (s == QLatin1String("header-page-first"))
        sec = KReportSectionData::PageHeaderFirst;
    else if (s == QLatin1String("header-page-last"))
        sec = KReportSectionData::PageHeaderLast;
    else if (s == QLatin1String("header-report"))
        sec = KReportSectionData::ReportHeader;
    else if (s == QLatin1String("footer-page-any"))
        sec = KReportSectionData::PageFooterAny;
    else if (s == QLatin1String("footer-page-even"))
        sec = KReportSectionData::PageFooterEven;
    else if (s == QLatin1String("footer-page-odd"))
        sec = KReportSectionData::PageFooterOdd;
    else if (s == QLatin1String("footer-page-first"))
        sec = KReportSectionData::PageFooterFirst;
    else if (s == QLatin1String("footer-page-last"))
        sec = KReportSectionData::PageFooterLast;
    else if (s == QLatin1String("footer-report"))
        sec = KReportSectionData::ReportFooter;
    else if (s == QLatin1String("group-header"))
        sec = KReportSectionData::GroupHeader;
    else if (s == QLatin1String("group-footer"))
        sec = KReportSectionData::GroupFooter;
    else if (s == QLatin1String("detail"))
        sec = KReportSectionData::Detail;
    else
        sec = KReportSectionData::None;

    return sec;
}
