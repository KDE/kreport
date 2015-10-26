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

//
// Class ReportCanvas
//
//     Overrides the drawForeground() method to do the grid.
//

#ifndef KREPORTDESIGNERSECTIONSCENE_H
#define KREPORTDESIGNERSECTIONSCENE_H

#include <QGraphicsScene>

#include "KReportUnit.h"

typedef QList<QGraphicsItem*> QGraphicsItemList;
class KReportDesigner;
class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneMouseEvent;
class KReportDesignerItemRectBase;

class KReportDesignerSectionScene : public QGraphicsScene
{
    Q_OBJECT
public:
    KReportDesignerSectionScene(qreal w, qreal h, KReportDesigner* rd);
    virtual ~KReportDesignerSectionScene();
    KReportDesigner* document() const {
        return m_rd;
    }
    QPointF gridPoint(const QPointF&);
    void raiseSelected();
    void lowerSelected();
    QGraphicsItemList itemsOrdered() const;
    qreal gridSize() const {
        return m_pixelIncrementX;
    }

protected:
    virtual void drawBackground(QPainter* painter, const QRectF & clip);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * e);
    virtual void focusOutEvent(QFocusEvent * focusEvent);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * contextMenuEvent);

Q_SIGNALS:
    void clicked();
    void lostFocus();

private:
    qreal lowestZValue();
    qreal highestZValue();
    void exitInlineEditingModeInItems(KReportDesignerItemRectBase *rectUnderCursor);

    KReportDesigner * m_rd;

    KReportUnit m_unit;
    int m_minorSteps;
    qreal m_majorX;
    qreal m_majorY;
    qreal m_pixelIncrementX;
    qreal m_pixelIncrementY;
    int m_dpiX;
    int m_dpiY;
};

#endif
