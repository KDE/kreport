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
    KReportDesignerItemLine(KReportDesigner *, QGraphicsScene *scene, const QPointF &pos);
    KReportDesignerItemLine(KReportDesigner *d, QGraphicsScene *scene, const QPointF &startPos,
                            const QPointF &endPos);
    KReportDesignerItemLine(const QDomNode &element, KReportDesigner *, QGraphicsScene *scene);

    void buildXML(QDomDocument *doc, QDomElement *parent) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    KReportDesignerItemLine *clone() override;

    void setLineScene(QLineF);

    void move(const QPointF &m) override;

private:
    KReportDesigner* m_rd;
    void init(QGraphicsScene*, KReportDesigner *);
    int grabHandle(QPointF pos);

    int m_grabAction;

protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent * event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent * event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private Q_SLOTS:
    void slotPropertyChanged(KPropertySet &, KProperty &);
};

#endif
