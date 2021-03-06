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

#include "KReportDesignerItemLine.h"
#include "KReportDesignerItemBase.h"
#include "KReportDesigner.h"
#include "KReportDesignerSectionScene.h"
#include "KReportUtils.h"
#include "KReportLineStyle.h"

#include <QDomDocument>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

//
// class ReportEntityLine
//
void KReportDesignerItemLine::init(QGraphicsScene* s, KReportDesigner *r)
{
    setPos(0, 0);
    setUnit(r->pageUnit());

    nameProperty()->setValue(r->suggestEntityName(typeName()));
    
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);

    setPen(QPen(Qt::black, 5));
    setAcceptHoverEvents(true);

    if (s)
        s->addItem(this);

    setZValue(z());
}

KReportDesignerItemLine::KReportDesignerItemLine(KReportDesigner * d, QGraphicsScene * scene, const QPointF &pos)
        : KReportDesignerItemBase(d, this)
{
    init(scene, d);
    setLineScene(QLineF(pos, QPointF(20,20)+pos));
    
}

KReportDesignerItemLine::KReportDesignerItemLine(KReportDesigner * d, QGraphicsScene * scene, const QPointF &startPos, const QPointF &endPos)
        : KReportDesignerItemBase(d, this)
{
    init(scene, d);
    setLineScene(QLineF(startPos, endPos));
}

KReportDesignerItemLine::KReportDesignerItemLine(const QDomNode & entity, KReportDesigner * d, QGraphicsScene * scene)
        : KReportItemLine(entity), KReportDesignerItemBase(d, this)
{
    init(scene, d);
    QPointF s = scenePosition(startPosition());
    QPointF e = scenePosition(endPosition());
    
    setLine ( s.x(), s.y(), e.x(), e.y() );
}

KReportDesignerItemLine::~KReportDesignerItemLine()
{
}

KReportDesignerItemLine* KReportDesignerItemLine::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement(QLatin1String("clone"));
    QDomNode n;
    buildXML(&d, &e);
    n = e.firstChild();
    return new KReportDesignerItemLine(n, designer(), nullptr);
}

void KReportDesignerItemLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                             QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen p = painter->pen();
    painter->setPen(QPen(m_lineColor->value().value<QColor>(), m_lineWeight->value().toInt(), (Qt::PenStyle)m_lineStyle->value().toInt()));
    painter->drawLine(line());
    if (isSelected()) {

        // draw a selected border for visual purposes
        painter->setPen(QPen(QColor(128, 128, 255), 0, Qt::DotLine));
        QPointF pt = line().p1();
        painter->fillRect(pt.x(), pt.y() - 2, 5, 5, QColor(128, 128, 255));
        pt = line().p2();
        painter->fillRect(pt.x() - 4, pt.y() - 2, 5, 5, QColor(128, 128, 255));

        painter->setPen(p);
    }
}

void KReportDesignerItemLine::buildXML(QDomDocument *doc, QDomElement *parent)
{
    QDomElement entity = doc->createElement(QLatin1String("report:") + typeName());

    // properties
    addPropertyAsAttribute(&entity, nameProperty());
    entity.setAttribute(QLatin1String("report:z-index"), zValue());
    KReportUtils::setAttribute(&entity, QLatin1String("svg:x1"), startPosition().x());
    KReportUtils::setAttribute(&entity, QLatin1String("svg:y1"), startPosition().y());
    KReportUtils::setAttribute(&entity, QLatin1String("svg:x2"), endPosition().x());
    KReportUtils::setAttribute(&entity, QLatin1String("svg:y2"), endPosition().y());

    buildXMLLineStyle(doc, &entity, lineStyle());

    parent->appendChild(entity);
}

void KReportDesignerItemLine::propertyChanged(KPropertySet &s, KProperty &p)
{
    Q_UNUSED(s);

    if (p.name() == "startposition" || p.name() == "endposition") {
        QPointF s = scenePosition(startPosition());
        QPointF e = scenePosition(endPosition());
        
        setLine ( s.x(), s.y(), e.x(), e.y() );
    }
    else if (p.name() == "name") {
        //For some reason p.oldValue returns an empty string
        if (!designer()->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(oldName());
        } else {
            setOldName(p.value().toString());
        }
    }
    if (designer())
        designer()->setModified(true);

    update();
}

void KReportDesignerItemLine::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    designer()->changeSet(propertySet());
    setSelected(true);
    QGraphicsLineItem::mousePressEvent(event);
}

QVariant KReportDesignerItemLine::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}

void KReportDesignerItemLine::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsLineItem::mouseReleaseEvent(event);
}

void KReportDesignerItemLine::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    int x;
    int y;

    KReportDesignerSectionScene *section = qobject_cast<KReportDesignerSectionScene*>(scene());
    if (!section) {
        return;
    }
        
    QPointF p = section->gridPoint(event->scenePos());
    //kreportDebug() << p;
    x = p.x();
    y = p.y();

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > scene()->width()) x = scene()->width();
    if (y > scene()->height()) y = scene()->height();

    switch (m_grabAction) {
    case 1:
        setStartPosition(positionFromScene(QPointF(x,y)));
        break;
    case 2:
        setEndPosition(positionFromScene(QPointF(x,y)));
        break;
    default:
        QPointF d = mapToItem(this, section->gridPoint(event->scenePos())) - mapToItem(this, section->gridPoint(event->lastScenePos()));

        if (((line().p1() + d).x() >= 0) &&
                ((line().p2() + d).x() >= 0) &&
                ((line().p1() + d).y() >= 0) &&
                ((line().p2() + d).y() >= 0)  &&
                ((line().p1() + d).x() <= scene()->width()) &&
                ((line().p2() + d).x() <= scene()->width()) &&
                ((line().p1() + d).y() <= scene()->height()) &&
                ((line().p2() + d).y() <= scene()->height()))
            setLineScene(QLineF(line().p1() + d, line().p2() + d));
        break;
    }
}

int KReportDesignerItemLine::grabHandle(const QPointF &pos)
{
    if (QRectF(line().p1().x(), line().p1().y() - 2, 5, 5).contains(pos)) {
        // we are over point 1
        return 1;
    } else if (QRectF(line().p2().x() - 4, line().p2().y() - 2, 5, 5).contains(pos)) {
        // we are over point 2
        return 2;
    } else {
        return 0;
    }

}

void KReportDesignerItemLine::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
    if (isSelected()) {
        m_grabAction = grabHandle(event->pos());
        switch (m_grabAction) {
        case 1: //Point 1
            setCursor(Qt::SizeAllCursor);
            break;
        case 2: //Point 2
            setCursor(Qt::SizeAllCursor);
            break;
        default:
            unsetCursor();
        }
    }
}

void KReportDesignerItemLine::setLineScene(const QLineF &line)
{
    setStartPosition(positionFromScene(line.p1()));
    setEndPosition(positionFromScene(line.p2()));
    setLine(line);
}

void KReportDesignerItemLine::move(const QPointF& m)
{
    QPointF original = scenePosition(position());
    original += m;
    
    setPosition(positionFromScene(original));
}
