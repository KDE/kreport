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
    explicit KReportDesignerItemRectBase(KReportDesigner *r, KReportItemBase *b);

    ~KReportDesignerItemRectBase() override;

    QRectF pointRect() const;

    virtual void enterInlineEditingMode();
    virtual void exitInlineEditingMode();

protected:
    int m_dpiX;
    int m_dpiY;

    enum UpdatePropertyFlag {
        UpdateProperty,
        DontUpdateProperty
    };

    void setSceneRect(const QPointF& topLeft, const QSizeF& size, UpdatePropertyFlag update = UpdateProperty);
    void setSceneRect(const QRectF& rect, UpdatePropertyFlag update = UpdateProperty);

    void drawHandles(QPainter*);
    QRectF sceneRect();
    void mousePressEvent(QGraphicsSceneMouseEvent * event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent * event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void propertyChanged(const KPropertySet &s, const KProperty &p);

    void move(const QPointF&) override;
    QRectF properRect(const KReportDesigner &d, qreal minWidth, qreal minHeight) const;
private:
    int grabHandle(const QPointF &pos);
    QPointF properPressPoint(const KReportDesigner &d) const;

    class Private;
    Private * const d;
};

#endif
