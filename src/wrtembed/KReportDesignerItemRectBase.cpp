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

#include "KReportDesignerItemRectBase.h"
#include "KReportDesignerSectionView.h"
#include "KReportDesigner.h"
#include "KReportDesignerSectionScene.h"
#include "KReportUtils_p.h"

#include <KPropertySet>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>

class Q_DECL_HIDDEN KReportDesignerItemRectBase::Private
{
public:
    Private();
    ~Private();
    
    int grabAction = 0;
};

KReportDesignerItemRectBase::Private::Private()
{
}

KReportDesignerItemRectBase::Private::~Private()
{
}

KReportDesignerItemRectBase::KReportDesignerItemRectBase(KReportDesigner *r, KReportItemBase *b)
        : QGraphicsRectItem(), KReportDesignerItemBase(r, b), d(new KReportDesignerItemRectBase::Private)
{
    m_dpiX = KReportPrivate::dpiX();
    m_dpiY = KReportPrivate::dpiY();

    d->grabAction = 0;
    setAcceptHoverEvents(true);

    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
}

KReportDesignerItemRectBase::~KReportDesignerItemRectBase()
{
    delete d;
}

QRectF KReportDesignerItemRectBase::sceneRect()
{
    return QRectF(KReportItemBase::scenePosition(item()->position()), KReportItemBase::sceneSize(item()->size()));
}

QRectF KReportDesignerItemRectBase::pointRect() const
{
    return QRectF(item()->position(), item()->size());
}

void KReportDesignerItemRectBase::setSceneRect(const QPointF& topLeft, const QSizeF& size, UpdatePropertyFlag update)
{
    setSceneRect(QRectF(topLeft, size), update);
}

void KReportDesignerItemRectBase::setSceneRect(const QRectF& rect, UpdatePropertyFlag update)
{
    QGraphicsRectItem::setPos(rect.x(), rect.y());
    setRect(0, 0, rect.width(), rect.height());
    if (update == UpdateProperty) {
        item()->setPosition(KReportItemBase::positionFromScene(QPointF(rect.x(), rect.y())));
        item()->setSize(KReportItemBase::sizeFromScene(QSizeF(rect.width(), rect.height())));
    }
    this->update();
}

void KReportDesignerItemRectBase::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    //Update and show properties
    item()->setPosition(KReportItemBase::positionFromScene(QPointF(sceneRect().x(), sceneRect().y())));
    designer()->changeSet(item()->propertySet());
    setSelected(true);
    scene()->update();

    QGraphicsItem::mousePressEvent(event);
}

void KReportDesignerItemRectBase::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    //Keep the size and position in sync
    item()->setPosition(KReportItemBase::positionFromScene(pos()));
    item()->setSize(KReportItemBase::sizeFromScene(QSizeF(rect().width(), rect().height())));

    QGraphicsItem::mouseReleaseEvent(event);
}

void KReportDesignerItemRectBase::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    //kreportDebug() << m_grabAction;

    qreal w, h;

    KReportDesignerSectionScene *section = qobject_cast<KReportDesignerSectionScene*>(scene());
    if (!section) {
        return;
    }
    
    QPointF p  = section->gridPoint(event->scenePos());
    w = p.x() - scenePos().x();
    h = p.y() - scenePos().y();

    //! @todo use an enum for the directions

    switch (d->grabAction) {
    case 1:
        if (sceneRect().y() - p.y() + rect().height() > 0 && sceneRect().x() - p.x() + rect().width() >= 0)
            setSceneRect(QPointF(p.x(), p.y()), QSizeF(sceneRect().x() - p.x() + rect().width(), sceneRect().y() - p.y() + rect().height()));
        break;
    case 2:
        if (sceneRect().y() - p.y() + rect().height() >= 0)
            setSceneRect(QPointF(sceneRect().x(), p.y()), QSizeF(rect().width(), sceneRect().y() - p.y() + rect().height()));
        break;
    case 3:
        if (sceneRect().y() - p.y() + rect().height() >= 0 && w >= 0)
            setSceneRect(QPointF(sceneRect().x(), p.y()), QSizeF(w, sceneRect().y() - p.y() + rect().height()));
        break;
    case 4:
        if (w >= 0)
            setSceneRect(QPointF(sceneRect().x(), sceneRect().y()), QSizeF(w, (rect().height())));
        break;
    case 5:
        if (h >= 0 && w >= 0)
            setSceneRect(QPointF(sceneRect().x(), sceneRect().y()), QSizeF(w, h));
        break;
    case 6:
        if (h >= 0)
            setSceneRect(QPointF(sceneRect().x(), sceneRect().y()), QSizeF((rect().width()), h));
        break;
    case 7:
        if (sceneRect().x() - p.x() + rect().width() >= 0 && h >= 0)
            setSceneRect(QPointF(p.x(), sceneRect().y()), QSizeF(sceneRect().x() - p.x() + rect().width(), h));
        break;
    case 8:
        if (sceneRect().x() - p.x() + rect().width() >= 0)
            setSceneRect(QPointF(p.x(), sceneRect().y()), QSizeF(sceneRect().x() - p.x() + rect().width(), rect().height()));
        break;
    default:
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void KReportDesignerItemRectBase::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
    //m_grabAction = 0;

    if (isSelected()) {
        d->grabAction = grabHandle(event->pos());
        switch (d->grabAction) {
        case 1:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case 2:
            setCursor(Qt::SizeVerCursor);
            break;
        case 3:
            setCursor(Qt::SizeBDiagCursor);
            break;
        case 4:
            setCursor(Qt::SizeHorCursor);
            break;
        case 5:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case 6:
            setCursor(Qt::SizeVerCursor);
            break;
        case 7:
            setCursor(Qt::SizeBDiagCursor);
            break;
        case 8:
            setCursor(Qt::SizeHorCursor);
            break;
        default:
            unsetCursor();
        }
    }
    //kreportDebug() << m_grabAction;
}

void KReportDesignerItemRectBase::drawHandles(QPainter *painter)
{
    if (isSelected()) {
        // draw a selected border for visual purposes
        painter->setPen(QPen(QColor(128, 128, 255), 0, Qt::DotLine));

        painter->drawRect(rect());

        const QRectF r = rect();
        double halfW = (r.width() / 2);
        double halfH = (r.height() / 2);
        QPointF center = r.center();

        center += QPointF(0.75,0.75);

        painter->fillRect(center.x() - halfW, center.y() - halfH , 5, 5, QColor(128, 128, 255));
        painter->fillRect(center.x() - 2, center.y() - halfH , 5, 5, QColor(128, 128, 255));
        painter->fillRect(center.x() + halfW - 4, center.y() - halfH, 5, 5, QColor(128, 128, 255));

        painter->fillRect(center.x() + (halfW - 4), center.y() - 2, 5, 5, QColor(128, 128, 255));

        painter->fillRect(center.x() +  halfW - 4 , center.y() + halfH - 4 , 5, 5, QColor(128, 128, 255));
        painter->fillRect(center.x() - 2, center.y() + halfH - 4, 5, 5, QColor(128, 128, 255));
        painter->fillRect(center.x() - halfW, center.y() + halfH - 4 , 5, 5, QColor(128, 128, 255));

        painter->fillRect(center.x() - halfW, center.y() - 2, 5, 5, QColor(128, 128, 255));

    }
}

/**
 @return 1 2 3
  8 0 4
  7 6 5
*/
int KReportDesignerItemRectBase::grabHandle(QPointF pos)
{
    QRectF r = boundingRect();
    int halfW = (int)(r.width() / 2);
    int halfH = (int)(r.height() / 2);
    QPointF center = r.center();

    if (QRectF(center.x() - (halfW), center.y() - (halfH), 5, 5).contains(pos)) {
        // we are over the top-left handle
        return 1;
    } else if (QRectF(center.x() - 2, center.y() - (halfH), 5, 5).contains(pos)) {
        // top-middle handle
        return 2;
    } else if (QRectF(center.x() + (halfW - 4), center.y() - (halfH), 5, 5).contains(pos)) {
        // top-right
        return 3;
    } else if (QRectF(center.x() + (halfW - 4), center.y() - 2, 5, 5).contains(pos)) {
        // middle-right
        return 4;
    } else if (QRectF(center.x() + (halfW - 4), center.y() + (halfH - 4), 5, 5).contains(pos)) {
        // bottom-left
        return 5;
    } else if (QRectF(center.x() - 2, center.y() + (halfH - 4), 5, 5).contains(pos)) {
        // bottom-middle
        return 6;
    } else if (QRectF(center.x() - (halfW), center.y() + (halfH - 4), 5, 5).contains(pos)) {
        // bottom-right
        return 7;
    } else if (QRectF(center.x() - (halfW), center.y() - 2, 5, 5).contains(pos)) {
        // middle-right
        return 8;
    }
    return 0;
}

QVariant KReportDesignerItemRectBase::itemChange(GraphicsItemChange change, const QVariant &value)
{
    KReportDesignerSectionScene *section = qobject_cast<KReportDesignerSectionScene*>(scene());
    if (section) {
            
        if (change == ItemPositionChange) {
            QPointF newPos = value.toPointF();
            
            newPos = section->gridPoint(newPos);
            if (newPos.x() < 0)
                newPos.setX(0);
            else if (newPos.x() > (scene()->width() - rect().width()))
                newPos.setX(scene()->width() - rect().width());

            if (newPos.y() < 0)
                newPos.setY(0);
            else if (newPos.y() > (scene()->height() - rect().height()))
                newPos.setY(scene()->height() - rect().height());

            return newPos;
        } else if (change == ItemPositionHasChanged) {
            item()->setPosition(KReportItemBase::positionFromScene(value.toPointF()));
            //TODO dont update property
            //m_ppos->setScenePos(value.toPointF(), KReportPosition::DontUpdateProperty);
        } else if (change == ItemSceneHasChanged && item()) {
            QPointF newPos = pos();

            newPos = section->gridPoint(newPos);
            if (newPos.x() < 0)
                newPos.setX(0);
            else if (newPos.x() > (scene()->width() - rect().width()))
                newPos.setX(scene()->width() - rect().width());

            if (newPos.y() < 0)
                newPos.setY(0);
            else if (newPos.y() > (scene()->height() - rect().height()))
                newPos.setY(scene()->height() - rect().height());

            setSceneRect(newPos, KReportItemBase::sceneSize(item()->size()), KReportDesignerItemRectBase::DontUpdateProperty);
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void KReportDesignerItemRectBase::propertyChanged(const KPropertySet &s, const KProperty &p)
{
    Q_UNUSED(s)

    if (p.name() == "position") {
        item()->setPosition(p.value().toPointF()); //TODO dont update property
    } else if (p.name() == "size") {
        item()->setSize(p.value().toSizeF()); //TODO dont update property
    }

    setSceneRect(KReportItemBase::scenePosition(item()->position()), KReportItemBase::sceneSize(item()->size()), DontUpdateProperty);
}

void KReportDesignerItemRectBase::move(const QPointF& /*m*/)
{
//! @todo
}

QPointF KReportDesignerItemRectBase::properPressPoint(const KReportDesigner &d) const
{
    const QPointF pressPoint = d.getPressPoint();
    const QPointF releasePoint = d.getReleasePoint();
    if (releasePoint.x() < pressPoint.x() && releasePoint.y() < pressPoint.y()) {
        return releasePoint;
    }
    if (releasePoint.x() < pressPoint.x() && releasePoint.y() > pressPoint.y()) {
        return QPointF(releasePoint.x(), pressPoint.y());
    }
    if (releasePoint.x() > pressPoint.x() && releasePoint.y() < pressPoint.y()) {
        return QPointF(pressPoint.x(), releasePoint.y());
    }
    return QPointF(pressPoint);
}

QRectF KReportDesignerItemRectBase::properRect(const KReportDesigner &d, qreal minWidth, qreal minHeight) const
{
    QPointF tempPressPoint = properPressPoint(d);
    qreal currentPressX = tempPressPoint.x();
    qreal currentPressY = tempPressPoint.y();
    const qreal width = qMax(d.countSelectionWidth(), minWidth);
    const qreal height = qMax(d.countSelectionHeight(), minHeight);

    qreal tempReleasePointX = tempPressPoint.x() + width;
    qreal tempReleasePointY = tempPressPoint.y() + height;

    if (tempReleasePointX > scene()->width()) {
        int offsetWidth = tempReleasePointX - scene()->width();
        currentPressX = tempPressPoint.x() - offsetWidth;
    }
    if (tempReleasePointY > scene()->height()) {
        int offsetHeight = tempReleasePointY - scene()->height();
        currentPressY = tempPressPoint.y() - offsetHeight;
    }
    return (QRectF(QPointF(currentPressX, currentPressY), QSizeF(width, height)));
}

void KReportDesignerItemRectBase::enterInlineEditingMode()
{
}

void KReportDesignerItemRectBase::exitInlineEditingMode()
{
}

void KReportDesignerItemBase::updateRenderText(const QString &itemDataSource, const QString &itemStaticValue, const QString &itemType)
{
    if (itemDataSource.isEmpty()) {
        if (itemType.isEmpty()) {
            setRenderText(itemStaticValue);
        } else {
            setRenderText(dataSourceAndObjectTypeName(itemStaticValue, itemType));
        }
    } else {
        if (itemType.isEmpty()) {
            setRenderText(itemDataSource);
        } else {
            setRenderText(dataSourceAndObjectTypeName(itemDataSource, itemType));
        }
    }
}
