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

#ifndef KREPORTDESIGNERITEMRECTBASE_H
#define KREPORTDESIGNERITEMRECTBASE_H

#include <QGraphicsRectItem>

#include "KReportDesignerItemBase.h"
#include "kreport_export.h"

class KReportDesigner;
class KReportPosition;
class KReportSize;

class KPropertySet;

const int KREPORT_ITEM_RECT_DEFAULT_WIDTH = 100;
const int KREPORT_ITEM_RECT_DEFAULT_HEIGHT = 100;

/**
*/
class KREPORT_EXPORT KReportDesignerItemRectBase : public QGraphicsRectItem, public KReportDesignerItemBase
{
public:
    explicit KReportDesignerItemRectBase(KReportDesigner*);

    virtual ~KReportDesignerItemRectBase();

    QRectF pointRect() const;

    virtual void enterInlineEditingMode();
    virtual void exitInlineEditingMode();

protected:
    void init(KReportPosition*, KReportSize*, KPropertySet*, KReportDesigner *r);

    int m_dpiX;
    int m_dpiY;
    qreal m_userHeight;
    qreal m_userWidth;
    qreal m_pressX;
    qreal m_pressY;

    enum UpdatePropertyFlag {
        UpdateProperty,
        DontUpdateProperty
    };

    void setSceneRect(const QPointF& topLeft, const QSizeF& size, UpdatePropertyFlag update = UpdateProperty);
    void setSceneRect(const QRectF& rect, UpdatePropertyFlag update = UpdateProperty);

    void drawHandles(QPainter*);
    QRectF sceneRect();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void propertyChanged(const KPropertySet &s, const KProperty &p);

    virtual void move(const QPointF&);
    QRectF properRect(const KReportDesigner &d, qreal minWidth, qreal minHeight) const;
private:
    int grabHandle(QPointF);
    QPointF properPressPoint(const KReportDesigner &d) const;
    int m_grabAction;

    KReportPosition* m_ppos;
    KReportSize* m_psize;
    KPropertySet* m_pset;

};

#endif
