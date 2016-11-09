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

#ifndef KREPORTDESIGNERITEMLINE_H
#define KREPORTDESIGNERITEMLINE_H

#include "KReportDesignerItemBase.h"
#include "KReportItemLine.h"

#include <QGraphicsLineItem>

class KReportDesigner;
class KPropertySet;

class KReportDesignerItemLine : public KReportItemLine, public QGraphicsLineItem, public KReportDesignerItemBase
{
    Q_OBJECT
public:
    KReportDesignerItemLine(KReportDesigner *, QGraphicsScene * scene, const QPointF &pos);
    KReportDesignerItemLine(KReportDesigner * d, QGraphicsScene * scene, const QPointF &startPos, const QPointF &endPos);
    KReportDesignerItemLine(const QDomNode & element, KReportDesigner *, QGraphicsScene * scene);

    virtual void buildXML(QDomDocument *doc, QDomElement *parent);
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget = 0);
    virtual KReportDesignerItemLine* clone();

    void setLineScene(QLineF);

    virtual void move(const QPointF&);

private:
    KReportDesigner* m_rd;
    void init(QGraphicsScene*, KReportDesigner *);
    int grabHandle(QPointF pos);

    int m_grabAction;

protected:
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void propertyChanged(KPropertySet &s, KProperty &p);
};

#endif
