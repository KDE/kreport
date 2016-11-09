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

#include "KReportDesignerItemField.h"
#include "KReportItemField.h"
#include "KReportDesigner.h"
#include "kreportplugin_debug.h"
#include "KReportLineStyle.h"

#include <QDomDocument>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

//
// class ReportEntityField
//

void KReportDesignerItemField::init(QGraphicsScene * scene, KReportDesigner * d)
{
    if (scene)
        scene->addItem(this);

    setZValue(z());

    updateRenderText(m_controlSource->value().toString(), m_itemValue->value().toString(), QLatin1String("field"));
}

// methods (constructors)
KReportDesignerItemField::KReportDesignerItemField(KReportDesigner * rw, QGraphicsScene * scene, const QPointF &pos)
        : KReportDesignerItemRectBase(rw, this)
{
    Q_UNUSED(pos);
    init(scene, rw);
    setSceneRect(properRect(*rw, getTextRect().width(), getTextRect().height()));
    nameProperty()->setValue(designer()->suggestEntityName(typeName()));
}

KReportDesignerItemField::KReportDesignerItemField(const QDomNode & element, KReportDesigner * d, QGraphicsScene * s)
        : KReportItemField(element), KReportDesignerItemRectBase(d, this)
{
    init(s, d);
    setSceneRect(KReportItemBase::scenePosition(item()->position()), KReportItemBase::sceneSize(item()->size()));
}

KReportDesignerItemField* KReportDesignerItemField::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement(QLatin1String("clone"));
    QDomNode n;
    buildXML(&d, &e);
    n = e.firstChild();
    return new KReportDesignerItemField(n, designer(), 0);
}

// methods (deconstructor)
KReportDesignerItemField::~KReportDesignerItemField()
{}

QRect KReportDesignerItemField::getTextRect() const
{
    return QFontMetrics(font()).boundingRect(x(), y(), 0, 0, textFlags(), renderText());
}



void KReportDesignerItemField::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // store any values we plan on changing so we can restore them
    QFont f = painter->font();
    QPen  p = painter->pen();


    painter->setFont(font());
    painter->setBackgroundMode(Qt::TransparentMode);

    QColor bg = m_backgroundColor->value().value<QColor>();
    bg.setAlphaF(m_backgroundOpacity->value().toReal() *0.01);

    painter->setPen(m_foregroundColor->value().value<QColor>());

    painter->fillRect(QGraphicsRectItem::rect(), bg);
    painter->drawText(rect(), textFlags(), renderText());


    if ((Qt::PenStyle)m_lineStyle->value().toInt() == Qt::NoPen || m_lineWeight->value().toInt() <= 0) {
        painter->setPen(QPen(Qt::lightGray));
    } else {
        painter->setPen(QPen(m_lineColor->value().value<QColor>(), m_lineWeight->value().toInt(), (Qt::PenStyle)m_lineStyle->value().toInt()));
    }

    painter->drawRect(rect());


    drawHandles(painter);

    // restore an values before we started just in case
    painter->setFont(f);
    painter->setPen(p);
}

void KReportDesignerItemField::buildXML(QDomDocument *doc, QDomElement *parent)
{
    QDomElement entity = doc->createElement(QLatin1String("report:") + typeName());

    // properties
    addPropertyAsAttribute(&entity, nameProperty());
    addPropertyAsAttribute(&entity, m_controlSource);
    addPropertyAsAttribute(&entity, m_verticalAlignment);
    addPropertyAsAttribute(&entity, m_horizontalAlignment);
    addPropertyAsAttribute(&entity, m_wordWrap);
    addPropertyAsAttribute(&entity, m_canGrow);
    addPropertyAsAttribute(&entity, m_itemValue);

    entity.setAttribute(QLatin1String("report:z-index"), zValue());

    // bounding rect
    buildXMLRect(doc, &entity, this);

    //text style info
    buildXMLTextStyle(doc, &entity, textStyle());

    //Line Style
    buildXMLLineStyle(doc, &entity, lineStyle());


#if 0 //Field Totals
    if (m_trackTotal) {
        QDomElement tracktotal = doc->createElement("tracktotal");
        if (m_trackBuiltinFormat)
            tracktotal.setAttribute("builtin", "true");
        if (_useSubTotal)
            tracktotal.setAttribute("subtotal", "true");
        tracktotal.appendChild(doc.createTextNode(_trackTotalFormat->value().toString()));
        entity.appendChild(tracktotal);
    }
#endif

    parent->appendChild(entity);
}

void KReportDesignerItemField::slotPropertyChanged(KPropertySet &s, KProperty &p)
{
    Q_UNUSED(s);

    if (p.name() == "name") {
        //For some reason p.oldValue returns an empty string
        if (!designer()->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(oldName());
        } else {
            setOldName(p.value().toString());
        }
    }

    updateRenderText(m_controlSource->value().toString(), m_itemValue->value().toString(), QLatin1String("field"));

    KReportDesignerItemRectBase::propertyChanged(s, p);
    if (designer())designer()->setModified(true);
}

void KReportDesignerItemField::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    //kreportpluginDebug() << m_reportDesigner->fieldKeys() <<  m_reportDesigner->fieldNames();
    m_controlSource->setListData(designer()->fieldKeys(), designer()->fieldNames());
    KReportDesignerItemRectBase::mousePressEvent(event);
}


