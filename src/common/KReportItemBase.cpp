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

#include "KReportItemBase.h"

#include "KReportUtils.h"
#include "KReportUtils_p.h"

#include <KPropertySet>
#include <KPropertyListData>
#include <QApplication>
#include <QDomElement>

class Q_DECL_HIDDEN KReportItemBase::Private
{
public:
    Private();
    ~Private();

    void setUnit(const KReportUnit& u, bool force)
    {
        if (!force && unit == u) {
            return;
        }
        const QSignalBlocker blocker(set);
        KReportUnit oldunit = unit;
        unit = u;
        // convert values

        if (positionProperty) {
            positionProperty->setValue(KReportUnit::convertFromUnitToUnit(
                                           positionProperty->value().toPointF(), oldunit, u),
                                       KProperty::ValueOption::IgnoreOld);

            positionProperty->setOption("suffix", u.symbol());

        }
        if (sizeProperty) {
            sizeProperty->setValue(
                KReportUnit::convertFromUnitToUnit(sizeProperty->value().toSizeF(), oldunit, u),
                KProperty::ValueOption::IgnoreOld);

            sizeProperty->setOption("suffix", u.symbol());
        }

    }

    KPropertySet *set;
    KProperty *nameProperty;
    KProperty *sizeProperty;
    KProperty *positionProperty;
    KProperty *dataSourceProperty = nullptr;
    QString oldName;
    qreal z = 0;
    KReportUnit unit;
};

KReportItemBase::Private::Private()
{
    set = new KPropertySet();
    nameProperty = new KProperty("name", QString(), tr("Name"), tr("Object Name"));
    nameProperty->setValueSyncPolicy(KProperty::ValueSyncPolicy::FocusOut);

    positionProperty = new KProperty("position", QPointF(), QCoreApplication::translate("ItemPosition", "Position"));
    sizeProperty = new KProperty("size", QSizeF(), QCoreApplication::translate("ItemSize", "Size"));
    setUnit(DEFAULT_UNIT, true);

    set->addProperty(nameProperty);
    set->addProperty(positionProperty);
    set->addProperty(sizeProperty);
}

KReportItemBase::Private::~Private()
{
    delete set;
}


KReportItemBase::KReportItemBase() : d(new Private())
{
    connect(propertySet(), &KPropertySet::propertyChanged,
            this, &KReportItemBase::propertyChanged);

    connect(propertySet(), &KPropertySet::aboutToDeleteProperty, this, &KReportItemBase::aboutToDeleteProperty);
}

KReportItemBase::~KReportItemBase()
{
    delete d;
}

void KReportItemBase::createDataSourceProperty()
{
    if (d->dataSourceProperty) {
        return;
    }
    d->dataSourceProperty
        = new KProperty("item-data-source", new KPropertyListData, QVariant(), tr("Data Source"));
    d->dataSourceProperty->setOption("extraValueAllowed", true);
    d->set->addProperty(d->dataSourceProperty);
}

bool KReportItemBase::parseReportTextStyleData(const QDomElement & elemSource, KReportTextStyleData *ts)
{
    return KReportUtils::parseReportTextStyleData(elemSource, ts);
}

bool KReportItemBase::parseReportLineStyleData(const QDomElement & elemSource, KReportLineStyle *ls)
{
    return KReportUtils::parseReportLineStyleData(elemSource, ls);
}


bool KReportItemBase::parseReportRect(const QDomElement & elemSource)
{
    const QRectF r(KReportUtils::readRectAttributes(elemSource, DEFAULT_ELEMENT_RECT_PT));
    setPosition(r.topLeft());
    setSize(r.size());
    return true;
}

KReportUnit KReportItemBase::unit() const
{
    return d->unit;
}

void KReportItemBase::setUnit(const KReportUnit& u)
{
    d->setUnit(u, false);
}

int KReportItemBase::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                       const QVariant &data, KReportScriptHandler* script)
{
    Q_UNUSED(page)
    Q_UNUSED(section)
    Q_UNUSED(offset)
    Q_UNUSED(data)
    Q_UNUSED(script)
    return 0;
}

int KReportItemBase::renderReportData(OROPage *page, OROSection *section, const QPointF &offset,
                                       KReportDataSource *dataSource, KReportScriptHandler* script)
{
    Q_UNUSED(page)
    Q_UNUSED(section)
    Q_UNUSED(offset)
    Q_UNUSED(dataSource)
    Q_UNUSED(script)
    return 0;
}

QString KReportItemBase::itemDataSource() const
{
    return d->dataSourceProperty ? d->dataSourceProperty->value().toString() : QString();
}

void KReportItemBase::setItemDataSource(const QString& source)
{
    if (d->dataSourceProperty && d->dataSourceProperty->value() != source) {
        d->dataSourceProperty->setValue(source);
    }
}

KPropertySet* KReportItemBase::propertySet()
{
 return d->set;
}

bool KReportItemBase::supportsSubQuery() const
{
    return false;
}

QString KReportItemBase::entityName() const
{
    return d->nameProperty->value().toString();
}

void KReportItemBase::setEntityName(const QString& n)
{
    d->nameProperty->setValue(n);
}

KProperty* KReportItemBase::nameProperty()
{
    return d->nameProperty;
}

QString KReportItemBase::oldName() const
{
    return d->oldName;
}

void KReportItemBase::setOldName(const QString& old)
{
    d->oldName = old;
}

KProperty* KReportItemBase::dataSourceProperty()
{
    return d->dataSourceProperty;
}

QPointF KReportItemBase::position() const
{
    return d->unit.convertToPoint(d->positionProperty->value().toPointF());
}

QSizeF KReportItemBase::size() const
{
    return d->unit.convertToPoint(d->sizeProperty->value().toSizeF());
}

const KPropertySet * KReportItemBase::propertySet() const
{
    return d->set;
}

QPointF KReportItemBase::scenePosition(const QPointF &ptPos)
{
    const qreal x = POINT_TO_INCH(ptPos.x()) * KReportPrivate::dpiX();
    const qreal y = POINT_TO_INCH(ptPos.y()) * KReportPrivate::dpiY();
    return QPointF(x, y);
}

QSizeF KReportItemBase::sceneSize(const QSizeF &ptSize)
{
    const qreal w = POINT_TO_INCH(ptSize.width()) * KReportPrivate::dpiX();
    const qreal h = POINT_TO_INCH(ptSize.height()) * KReportPrivate::dpiY();
    return QSizeF(w, h);
}

qreal KReportItemBase::z() const
{
    return d->z;
}

void KReportItemBase::setZ(qreal z)
{
    d->z = z;
}

void KReportItemBase::setPosition(const QPointF& ptPos)
{
    d->positionProperty->setValue(d->unit.convertFromPoint(ptPos));
}

void KReportItemBase::setSize(const QSizeF &ptSize)
{
    d->sizeProperty->setValue(d->unit.convertFromPoint(ptSize));
}

QPointF KReportItemBase::positionFromScene(const QPointF& pos)
{
    const qreal x = INCH_TO_POINT(pos.x() / KReportPrivate::dpiX());
    const qreal y = INCH_TO_POINT(pos.y() / KReportPrivate::dpiY());
    return QPointF(x, y);
}

QSizeF KReportItemBase::sizeFromScene(const QSizeF& size)
{
    qreal w = INCH_TO_POINT(size.width() / KReportPrivate::dpiX());
    qreal h = INCH_TO_POINT(size.height() / KReportPrivate::dpiY());
    return QSizeF(w, h);
}

void KReportItemBase::propertyChanged(KPropertySet& s, KProperty& p)
{
    Q_UNUSED(s)
    Q_UNUSED(p)
}

void KReportItemBase::aboutToDeleteProperty(KPropertySet& set, KProperty& property)
{
    Q_UNUSED(set)
    if (property.name() == "size") {
        d->sizeProperty = nullptr;
    }
    else if (property.name() == "position") {
        d->positionProperty = nullptr;
    }
}
