/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010-2015 Jarosław Staniek <staniek@kde.org>
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

#include "KReportSection.h"
#include "KReportDesign_p.h"

#include "krreportdata.h"
#include "KoReportPluginInterface.h"
#include "KoReportPluginManager.h"
#include "KoReportItemLine.h"
#include "kreport_debug.h"

#if 0
KReportSection::KReportSection(const QDomElement & elemSource, KoReportReportData* report)
 : QObject(report)
{
    setObjectName(elemSource.tagName());

    if (objectName() != QLatin1String("report:section")) {
        m_valid = false;
        return;
    }

    m_type = sectionTypeFromString(elemSource.attribute(QLatin1String("report:section-type")));
    if (m_type == KReportSection::None) {
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
bool KReportSection::zLessThan(KoReportItemBase* s1, KoReportItemBase* s2)
{
    return s1->Z < s2->Z;
}

bool KReportSection::xLessThan(KoReportItemBase* s1, KoReportItemBase* s2)
{
    return s1->position().toPoint().x() < s2->position().toPoint().x();
}

#endif

KReportSection::~KReportSection()
{
}

#if 0
void KReportSection::createProperties(const QDomElement & elemSource)
{
    m_set = new KPropertySet(/*this*/0, QLatin1String("Section"));

    m_height = new KProperty("height", KReportUnit(KReportUnit::Centimeter).fromUserValue(2.0), tr("Height"));
    m_backgroundColor = new KProperty("background-color", QColor(Qt::white), tr("Background Color"));
    m_height->setOption("unit", QLatin1String("cm"));
    if (!elemSource.isNull())
        m_height->setValue(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:height"), QLatin1String("2.0cm"))));

    m_set->addProperty(m_height);
    m_set->addProperty(m_backgroundColor);
}

QString KReportSection::name() const
{
    return (objectName() + QLatin1Char('-') + sectionTypeString(m_type));
}
#endif

qreal KReportSection::height() const
{
    return d->height >= 0.0 ? d->height : KReportSection::defaultHeight();
}

QColor KReportSection::backgroundColor() const
{
    return d->backgroundColor.isValid() ? d->backgroundColor : KReportSection::defaultBackgroundColor();
}

QList<KReportElement> KReportSection::elements() const
{
    return d->elements;
}

bool KReportSection::addElement(const KReportElement &element)
{
    if (d->elementsSet.contains(element)) {
        kreportWarning() << "Section already contains element" << element;
        return false;
    }
    d->elements.append(element);
    d->elementsSet.insert(element);
    return true;
}

bool KReportSection::insertElement(int i, const KReportElement &element)
{
    if (i < 0 || i > d->elements.count()) {
        kreportWarning() << "Could not insert element at index" << i << "into section";
        return false;
    }
    if (d->elementsSet.contains(element)) {
        kreportWarning() << "Section already contains element" << element;
        return false;
    }
    d->elements.insert(i, element);
    d->elementsSet.insert(element);
    return true;
}

bool KReportSection::removeElement(const KReportElement &element)
{
    if (!d->elementsSet.remove(element)) {
        kreportWarning() << "Could not find element" << element << "in section";
        return false;
    }
    if (!d->elements.removeOne(element)) {
        kreportCritical() << "Could not find element" << element << "in section's list but found in section's set";
        return false;
    }
    return true;
}

bool KReportSection::removeElementAt(int i)
{
    if (i < 0 || i > (d->elements.count() - 1)) {
        kreportWarning() << "Could not find element at index" << i << "in section";
        return false;
    }
    KReportElement e = d->elements.takeAt(i);
    if (!d->elementsSet.remove(e)) {
        kreportWarning() << "Could not find element" << e << "in section";
        return false;
    }
    return true;
}

KReportSection::Data* KReportSection::Data::clone() const
{
    Data *data = new Data(*this);
    data->elements.clear();
    KReportElement eClone;
    foreach(const KReportElement &el, elements) {
        eClone = el.clone();
        data->elements.append(eClone);
        data->elementsSet.insert(eClone);
    }
    return data;
}

//static
qreal KReportSection::defaultHeight()
{
    return KReportDesignGlobal::self()->defaultSectionHeight;
}

//static
void KReportSection::setDefaultHeight(qreal height)
{
    KReportDesignGlobal::self()->defaultSectionHeight = height;
}

//static
QColor KReportSection::defaultBackgroundColor()
{
    return KReportDesignGlobal::self()->defaultSectionBackgroundColor;
}

//static
void KReportSection::setDefaultBackgroundColor(const QColor &color)
{
    KReportDesignGlobal::self()->defaultSectionBackgroundColor = color;
}