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
#include <KPropertySet>
#include <QApplication>
#include <QDomElement>

class Q_DECL_HIDDEN KReportItemBase::Private
{
public:
    Private();
    ~Private();
    
    KPropertySet *set;
    KProperty *nameProperty;
    KProperty *sizeProperty;
    KProperty *positionProperty;
    QString oldName;
    qreal z;
};

KReportItemBase::Private::Private()
{
    set = new KPropertySet();
    nameProperty = new KProperty("name", QString(), tr("Name"), tr("Object Name"));
    nameProperty->setAutoSync(0);
    
    positionProperty = new KProperty("position", QPointF(), QCoreApplication::translate("ItemPosition", "Position"));
    sizeProperty = new KProperty("size", QSizeF(), QCoreApplication::translate("ItemSize", "Size"));
    
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
    d->z = 0;
    
    connect(propertySet(), &KPropertySet::propertyChanged,
            this, &KReportItemBase::propertyChanged);
}

KReportItemBase::~KReportItemBase() 
{ 
    delete d;    
}

bool KReportItemBase::parseReportTextStyleData(const QDomElement & elemSource, KRTextStyleData *ts)
{
    return KReportUtils::parseReportTextStyleData(elemSource, ts);
}

bool KReportItemBase::parseReportLineStyleData(const QDomElement & elemSource, KReportLineStyle *ls)
{
    return KReportUtils::parseReportLineStyleData(elemSource, ls);
}


bool KReportItemBase::parseReportRect(const QDomElement & elemSource)
{
    QPointF pos;
    QSizeF size;

    pos.setX(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:x"), QLatin1String("1cm"))));
    pos.setY(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:y"), QLatin1String("1cm"))));
    size.setWidth(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:width"), QLatin1String("1cm"))));
    size.setHeight(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:height"), QLatin1String("1cm"))));

    setPosition(pos);
    setSize(size);

    return true;
    
}

void KReportItemBase::setUnit(const KReportUnit& u)
{
    qDebug() << "Setting page unit to: " << u.symbol();
    d->positionProperty->setOption("unit", u.symbol());
    d->sizeProperty->setOption("unit", u.symbol());    
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
                                       KReportData *data, KReportScriptHandler* script)
{
    Q_UNUSED(page)
    Q_UNUSED(section)
    Q_UNUSED(offset)
    Q_UNUSED(data)
    Q_UNUSED(script)
    return 0;
}

QString KReportItemBase::itemDataSource() const
{
    return QString();
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

QPointF KReportItemBase::position() const
{
    return d->positionProperty->value().toPointF();
}

QSizeF KReportItemBase::size() const
{
    return d->sizeProperty->value().toSizeF();
}

const KPropertySet * KReportItemBase::propertySet() const
{
    return propertySet();
}

QPointF KReportItemBase::scenePosition(const QPointF &pos)
{
    const qreal x = POINT_TO_INCH(pos.x()) * KReportDpi::dpiX();
    const qreal y = POINT_TO_INCH(pos.y()) * KReportDpi::dpiY();
    return QPointF(x, y);
}

QSizeF KReportItemBase::sceneSize(const QSizeF &size)
{
    const qreal w = POINT_TO_INCH(size.width()) * KReportDpi::dpiX();
    const qreal h = POINT_TO_INCH(size.height()) * KReportDpi::dpiY();
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

void KReportItemBase::setPosition(const QPointF& pos)
{
    d->positionProperty->setValue(pos);
}

void KReportItemBase::setSize(const QSizeF& size)
{
    d->sizeProperty->setValue(size);
}

QPointF KReportItemBase::positionFromScene(const QPointF& pos)
{
    const qreal x = INCH_TO_POINT(pos.x() / KReportDpi::dpiX());
    const qreal y = INCH_TO_POINT(pos.y() / KReportDpi::dpiY());
    return QPointF(x, y);
}

QSizeF KReportItemBase::sizeFromScene(const QSizeF& size)
{
    qreal w = INCH_TO_POINT(size.width() / KReportDpi::dpiX());
    qreal h = INCH_TO_POINT(size.height() / KReportDpi::dpiY());
    return QSizeF(w, h);
}

void KReportItemBase::propertyChanged(KPropertySet& s, KProperty& p)
{
}



