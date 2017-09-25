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

#include "KReportDesignerItemBarcode.h"
#include "KReportDesignerItemBase.h"
#include "KReportDesigner.h"

#include "barcodepaint.h"

#include <KProperty>
#include <KPropertySet>

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDomDocument>
#include <QPainter>
#include "kreportplugin_debug.h"

void KReportDesignerItemBarcode::init(QGraphicsScene *scene)
{
    if (scene)
        scene->addItem(this);

    connect(propertySet(), SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));

    setMaxLengthValue(5);
    setZ(z());

    updateRenderText(itemValue->value().toString().isEmpty() ? formatValue() : QString(),
                     itemValue->value().toString(), QString());
}
// methods (constructors)
KReportDesignerItemBarcode::KReportDesignerItemBarcode(KReportDesigner * rw, QGraphicsScene* scene, const QPointF &pos)
        : KReportDesignerItemRectBase(rw, this)
{
    Q_UNUSED(pos);
    init(scene);
    setSceneRect(properRect(*rw, minWidthTotal*dpiX(), minHeight*dpiY()));
    nameProperty()->setValue(designer()->suggestEntityName(typeName()));
}

KReportDesignerItemBarcode::KReportDesignerItemBarcode(const QDomNode & element, KReportDesigner * rw, QGraphicsScene* scene)
        : KReportItemBarcode(element), KReportDesignerItemRectBase(rw, this)
{
    init(scene);
    setSceneRect(KReportItemBase::scenePosition(item()->position()), KReportItemBase::sceneSize(item()->size()));
}

KReportDesignerItemBarcode* KReportDesignerItemBarcode::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement(QLatin1String("clone"));
    QDomNode n;
    buildXML(&d, &e);
    n = e.firstChild();
    return new KReportDesignerItemBarcode(n, designer(), nullptr);
}

// methods (deconstructor)
KReportDesignerItemBarcode::~KReportDesignerItemBarcode()
{}

QRect KReportDesignerItemBarcode::getTextRect()
{
    QFont fnt = QFont();
    return QFontMetrics(fnt)
            .boundingRect(int (x()), int (y()), 0, 0, 0,
                          dataSourceAndObjectTypeName(itemDataSource(), QLatin1String("barcode")));
}

void KReportDesignerItemBarcode::paint(QPainter* painter,
                                        const QStyleOptionGraphicsItem* option,
                                        QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // store any values we plan on changing so we can restore them
    QPen  p = painter->pen();

    painter->setBackground(Qt::white);

    //Draw a border so user knows the object edge
    painter->setPen(QPen(QColor(224, 224, 224)));
    painter->drawRect(rect());

    drawHandles(painter);

    QByteArray fmt = formatValue().toLatin1();
    if (fmt == "i2of5") {
        renderI2of5(rect().toRect(), renderText(), horizontalAlignmentValue(), painter);
    } else if (fmt == "3of9") {
        render3of9(rect().toRect(), renderText(), horizontalAlignmentValue(), painter);
    } else if (fmt == "3of9+") {
        renderExtended3of9(rect().toRect(), renderText(), horizontalAlignmentValue(), painter);
    } else if (fmt == "128") {
        renderCode128(rect().toRect(), renderText(), horizontalAlignmentValue(), painter);
    } else if (fmt == "upc-a") {
        renderCodeUPCA(rect().toRect(), renderText(), horizontalAlignmentValue(), painter);
    } else if (fmt == "upc-e") {
        renderCodeUPCE(rect().toRect(), renderText(), horizontalAlignmentValue(), painter);
    } else if (fmt == "ean13") {
        renderCodeEAN13(rect().toRect(), renderText(), horizontalAlignmentValue(), painter);
    } else if (fmt == "ean8") {
        renderCodeEAN8(rect().toRect(), renderText(), horizontalAlignmentValue(), painter);
    }

    painter->setPen(Qt::black);
    painter->drawText(rect(), 0, dataSourceAndObjectTypeName(itemDataSource(),
                                                             QLatin1String("barcode")));

    // restore an values before we started just in case
    painter->setPen(p);
}

void KReportDesignerItemBarcode::buildXML(QDomDocument *doc, QDomElement *parent)
{
    //kreportpluginDebug();
    QDomElement entity = doc->createElement(QLatin1String("report:") + typeName());

    // properties
    addPropertyAsAttribute(&entity, nameProperty());
    addPropertyAsAttribute(&entity, controlSource);
    addPropertyAsAttribute(&entity, horizontalAlignment);
    addPropertyAsAttribute(&entity, format);
    addPropertyAsAttribute(&entity, maxLength);
    entity.setAttribute(QLatin1String("report:z-index"), zValue());
    addPropertyAsAttribute(&entity, itemValue);

    // bounding rect
    buildXMLRect(doc, &entity, this);

    parent->appendChild(entity);
}

void KReportDesignerItemBarcode::slotPropertyChanged(KPropertySet &s, KProperty &p)
{
    if (p.name() == "name") {
        //For some reason p.oldValue returns an empty string
        if (!designer()->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(oldName());
        } else {
            setOldName(p.value().toString());
        }
    }

    updateRenderText(itemValue->value().toString().isEmpty() ? formatValue() : QString(),
                     itemValue->value().toString(), QString());

    KReportDesignerItemRectBase::propertyChanged(s, p);
    if (designer()) designer()->setModified(true);
}

void KReportDesignerItemBarcode::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    controlSource->setListData(designer()->fieldKeys(), designer()->fieldNames());
    KReportDesignerItemRectBase::mousePressEvent(event);
}
