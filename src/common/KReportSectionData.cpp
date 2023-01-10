/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010-2018 Jarosław Staniek <staniek@kde.org>
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

class Q_DECL_HIDDEN KReportSectionData::Private
{
public:
    explicit Private(KReportSectionData *data, const QDomElement &elemSource) : q(data)
    {
        if (!elemSource.isNull()) {
            q->setObjectName(elemSource.tagName());
            type = sectionTypeFromString(KReportUtils::readSectionTypeNameAttribute(elemSource));
        }
        createProperties(elemSource);
        if (!elemSource.isNull()) {
            loadXml(elemSource);
        }
        set.clearModifiedFlags();
    }

    ~Private()
    {
        qDeleteAll(objects);
    }

    void setHeight(qreal ptHeight, KProperty::ValueOptions options);

    static bool zLessThan(KReportItemBase* s1, KReportItemBase* s2);
    static bool xLessThan(KReportItemBase* s1, KReportItemBase* s2);

    KReportSectionData * const q;
    KPropertySet set;
    KProperty *backgroundColor;
    KProperty *height;
    QList<KReportItemBase *> objects;
    KReportUnit unit = KReportUnit(DEFAULT_UNIT_TYPE);
    Type type = Type::None;
    bool valid = true;

private:
    void createProperties(const QDomElement &elemSource);
    void loadXml(const QDomElement &elemSource);
};

KReportSectionData::KReportSectionData(QObject *parent)
    : KReportSectionData(QDomElement(), parent)
{
}

KReportSectionData::KReportSectionData(const QDomElement & elemSource, QObject *parent)
 : QObject(parent), d(new Private(this, elemSource))
{
}

void KReportSectionData::Private::loadXml(const QDomElement &elemSource)
{
    if (q->objectName() != QLatin1String("report:section") || type == KReportSectionData::Type::None) {
        valid = false;
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
                objects.append(krobj);
            } else {
                kreportWarning() << "Could not create element of type" << reportItemName;
            }
        } else {
            kreportWarning() << "While parsing section encountered an unknown element:" << n;
        }
    }
    std::sort(objects.begin(), objects.end(), zLessThan);
    valid = true;
}

KReportSectionData::~KReportSectionData()
{
    delete d;
}

KReportUnit KReportSectionData::unit() const
{
    return d->unit;
}

void KReportSectionData::setUnit(const KReportUnit &u)
{
    if (d->unit == u) {
        return;
    }
    // convert values
    KReportUnit oldunit = d->unit;
    d->unit = u;

    d->height->setValue(KReportUnit::convertFromUnitToUnit(d->height->value().toReal(), oldunit, u),
                       KProperty::ValueOption::IgnoreOld);
    d->height->setOption("suffix", u.symbol());
}

bool KReportSectionData::Private::zLessThan(KReportItemBase* s1, KReportItemBase* s2)
{
    return s1->z() < s2->z();
}

bool KReportSectionData::Private::xLessThan(KReportItemBase* s1, KReportItemBase* s2)
{
    return s1->position().toPoint().x() < s2->position().toPoint().x();
}

void KReportSectionData::Private::createProperties(const QDomElement & elemSource)
{
    KReportDesigner::addMetaProperties(&set, KReportSectionData::tr("Section", "Report section"),
                                       QLatin1String("kreport-section-element"));
    height = new KProperty("height", 0.0, tr("Height"));
    backgroundColor = new KProperty(
        "background-color",
        KReportUtils::attr(elemSource, QLatin1String("fo:background-color"), QColor(Qt::white)),
        tr("Background Color"));
    height->setOption("unit", QLatin1String("cm"));
    if (!elemSource.isNull()) {
        height->setValue(unit.convertFromPoint(
            KReportUtils::readSizeAttributes(
                elemSource, QSizeF(DEFAULT_SECTION_SIZE_PT, DEFAULT_SECTION_SIZE_PT))
                .height()));
    }

    set.addProperty(height);
    set.addProperty(backgroundColor);
    set.clearModifiedFlags();
}

QString KReportSectionData::name() const
{
    return (objectName() + QLatin1Char('-') + sectionTypeString(d->type));
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
    return d->unit.convertToPoint(d->height->value().toReal());
}

void KReportSectionData::setHeight(qreal ptHeight, KProperty::ValueOptions options)
{
    d->height->setValue(d->unit.convertFromPoint(ptHeight), options);
}

void KReportSectionData::setHeight(qreal ptHeight)
{
    setHeight(ptHeight, KProperty::ValueOptions());
}

KPropertySet* KReportSectionData::propertySet()
{
    return &d->set;
}

const KPropertySet* KReportSectionData::propertySet() const
{
    return &d->set;
}

bool KReportSectionData::isValid() const
{
    return d->valid;
}

QList<KReportItemBase*> KReportSectionData::objects() const
{
    return d->objects;
}

QColor KReportSectionData::backgroundColor() const
{
    return d->backgroundColor->value().value<QColor>();
}

void KReportSectionData::setBackgroundColor(const QColor &color)
{
    d->backgroundColor->setValue(color);
}

KReportSectionData::Type KReportSectionData::type() const
{
    return d->type;
}

KReportItemBase* KReportSectionData::object(int index)
{
    return d->objects.value(index);
}
