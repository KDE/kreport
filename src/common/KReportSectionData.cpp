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
#include "KReportDesigner.h"
#include "KReportDocument.h"
#include "KReportItemLine.h"
#include "KReportPluginInterface.h"
#include "KReportPluginManager.h"
#include "KReportItemLine.h"
#include "KReportDesigner.h"
#include "KReportUtils.h"
#include "KReportUtils_p.h"
#include "kreport_debug.h"

#include <KPropertySet>

#include <QDomElement>

KReportSectionData::KReportSectionData(QObject *parent) : KReportSectionData(QDomElement(), parent)
{
}

KReportSectionData::KReportSectionData(const QDomElement & elemSource, QObject *parent)
 : QObject(parent)
 , m_unit(DEFAULT_UNIT_TYPE)
{
    if (elemSource.isNull()) {
        m_type = Type::None;
    } else {
        setObjectName(elemSource.tagName());
        m_type = sectionTypeFromString(KReportUtils::readSectionTypeNameAttribute(elemSource));
    }

    createProperties(elemSource);

    if (elemSource.isNull()) {
        m_valid = true;
    } else {
        loadXml(elemSource);
    }
    m_set->clearModifiedFlags();
}

void KReportSectionData::loadXml(const QDomElement &elemSource)
{
    if (objectName() != QLatin1String("report:section") || m_type == KReportSectionData::Type::None) {
        m_valid = false;
        return;
    }

    KReportPluginManager* manager = KReportPluginManager::self();

    QDomNodeList section = elemSource.childNodes();
    for (int nodeCounter = 0; nodeCounter < section.count(); nodeCounter++) {
        QDomElement elemThis = section.item(nodeCounter).toElement();
        QString n = elemThis.tagName();
        if (n.startsWith(QLatin1String("report:"))) {
            KReportItemBase *krobj = nullptr;
            QString reportItemName = n.mid(qstrlen("report:"));
            if (reportItemName == QLatin1String("line")) {
                KReportItemLine * line = new KReportItemLine(elemThis);
                krobj = line;
            } else {
                KReportPluginInterface *plugin = manager->plugin(reportItemName);
                if (plugin) {
                    QObject *obj = plugin->createRendererInstance(elemThis);
                    if (obj) {
                        krobj = dynamic_cast<KReportItemBase*>(obj);
                    }
                }
            }
            if (krobj) {
                krobj->propertySet()->clearModifiedFlags();
                m_objects.append(krobj);
            } else {
                kreportWarning() << "Could not create element of type" << reportItemName;
            }
        } else {
            kreportWarning() << "While parsing section encountered an unknown element:" << n;
        }
    }
    qSort(m_objects.begin(), m_objects.end(), zLessThan);
    m_valid = true;
}

KReportSectionData::~KReportSectionData()
{
    delete m_set;
    qDeleteAll(m_objects);
}

KReportUnit KReportSectionData::unit() const
{
    return m_unit;
}

void KReportSectionData::setUnit(const KReportUnit &u)
{
    if (m_unit == u) {
        return;
    }
    // convert values
    KReportUnit oldunit = m_unit;
    m_unit = u;

    m_height->setValue(KReportUnit::convertFromUnitToUnit(m_height->value().toReal(), oldunit, u),
                       KProperty::ValueOption::IgnoreOld);
    m_height->setOption("suffix", u.symbol());
}

bool KReportSectionData::zLessThan(KReportItemBase* s1, KReportItemBase* s2)
{
    return s1->z() < s2->z();
}

bool KReportSectionData::xLessThan(KReportItemBase* s1, KReportItemBase* s2)
{
    return s1->position().toPoint().x() < s2->position().toPoint().x();
}

void KReportSectionData::createProperties(const QDomElement & elemSource)
{
    m_set = new KPropertySet(this);
    KReportDesigner::addMetaProperties(m_set,
        tr("Section", "Report section"), QLatin1String("kreport-section-element"));

    m_height = new KProperty("height", 0.0, tr("Height"));
    m_backgroundColor = new KProperty(
        "background-color",
        KReportUtils::attr(elemSource, QLatin1String("fo:background-color"), QColor(Qt::white)),
        tr("Background Color"));
    m_height->setOption("unit", QLatin1String("cm"));
    if (!elemSource.isNull()) {
        m_height->setValue(m_unit.convertFromPoint(
            KReportUtils::readSizeAttributes(
                elemSource, QSizeF(DEFAULT_SECTION_SIZE_PT, DEFAULT_SECTION_SIZE_PT))
                .height()));
    }

    m_set->addProperty(m_height);
    m_set->addProperty(m_backgroundColor);
    m_set->clearModifiedFlags();
}

QString KReportSectionData::name() const
{
    return (objectName() + QLatin1Char('-') + sectionTypeString(m_type));
}

QString KReportSectionData::sectionTypeString(KReportSectionData::Type type)
{
//! @todo use QMap
    QString sectiontype;
    switch (type) {
    case KReportSectionData::Type::PageHeaderAny:
        sectiontype = QLatin1String("header-page-any");
        break;
    case KReportSectionData::Type::PageHeaderEven:
        sectiontype = QLatin1String("header-page-even");
        break;
    case KReportSectionData::Type::PageHeaderOdd:
        sectiontype = QLatin1String("header-page-odd");
        break;
    case KReportSectionData::Type::PageHeaderFirst:
        sectiontype = QLatin1String("header-page-first");
        break;
    case KReportSectionData::Type::PageHeaderLast:
        sectiontype = QLatin1String("header-page-last");
        break;
    case KReportSectionData::Type::PageFooterAny:
        sectiontype = QLatin1String("footer-page-any");
        break;
    case KReportSectionData::Type::PageFooterEven:
        sectiontype = QLatin1String("footer-page-even");
        break;
    case KReportSectionData::Type::PageFooterOdd:
        sectiontype = QLatin1String("footer-page-odd");
        break;
    case KReportSectionData::Type::PageFooterFirst:
        sectiontype = QLatin1String("footer-page-first");
        break;
    case KReportSectionData::Type::PageFooterLast:
        sectiontype = QLatin1String("footer-page-last");
        break;
    case KReportSectionData::Type::ReportHeader:
        sectiontype = QLatin1String("header-report");
        break;
    case KReportSectionData::Type::ReportFooter:
        sectiontype = QLatin1String("footer-report");
        break;
    case KReportSectionData::Type::GroupHeader:
        sectiontype = QLatin1String("group-header");
        break;
    case KReportSectionData::Type::GroupFooter:
        sectiontype = QLatin1String("group-footer");
        break;
    case KReportSectionData::Type::Detail:
        sectiontype = QLatin1String("detail");
        break;
    default:
        break;
    }

    return sectiontype;
}

KReportSectionData::Type KReportSectionData::sectionTypeFromString(const QString& s)
{
//! @todo use QMap
    KReportSectionData::Type type;
    //kreportDebug() << "Determining section type for " << s;
    if (s == QLatin1String("header-page-any"))
        type = KReportSectionData::Type::PageHeaderAny;
    else if (s == QLatin1String("header-page-even"))
        type = KReportSectionData::Type::PageHeaderEven;
    else if (s == QLatin1String("header-page-odd"))
        type = KReportSectionData::Type::PageHeaderOdd;
    else if (s == QLatin1String("header-page-first"))
        type = KReportSectionData::Type::PageHeaderFirst;
    else if (s == QLatin1String("header-page-last"))
        type = KReportSectionData::Type::PageHeaderLast;
    else if (s == QLatin1String("header-report"))
        type = KReportSectionData::Type::ReportHeader;
    else if (s == QLatin1String("footer-page-any"))
        type = KReportSectionData::Type::PageFooterAny;
    else if (s == QLatin1String("footer-page-even"))
        type = KReportSectionData::Type::PageFooterEven;
    else if (s == QLatin1String("footer-page-odd"))
        type = KReportSectionData::Type::PageFooterOdd;
    else if (s == QLatin1String("footer-page-first"))
        type = KReportSectionData::Type::PageFooterFirst;
    else if (s == QLatin1String("footer-page-last"))
        type = KReportSectionData::Type::PageFooterLast;
    else if (s == QLatin1String("footer-report"))
        type = KReportSectionData::Type::ReportFooter;
    else if (s == QLatin1String("group-header"))
        type = KReportSectionData::Type::GroupHeader;
    else if (s == QLatin1String("group-footer"))
        type = KReportSectionData::Type::GroupFooter;
    else if (s == QLatin1String("detail"))
        type = KReportSectionData::Type::Detail;
    else
        type = KReportSectionData::Type::None;

    return type;
}

qreal KReportSectionData::height() const
{
    return m_unit.convertToPoint(m_height->value().toReal());
}

void KReportSectionData::setHeight(qreal ptHeight, KProperty::ValueOptions options)
{
    m_height->setValue(m_unit.convertFromPoint(ptHeight), options);
}

void KReportSectionData::setHeight(qreal ptHeight)
{
    setHeight(ptHeight, KProperty::ValueOptions());
}
