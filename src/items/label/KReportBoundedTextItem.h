/* This file is part of the KDE project
  Copyright (C) 2014 Adam Pigg <adam@piggz.co.uk>
  Copyright (C) 2016 Jarosław Staniek <staniek@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef KREPORT_BOUNDEDTEXTITEM_H
#define KREPORT_BOUNDEDTEXTITEM_H

#include <QGraphicsTextItem>
#include <QFont>

/**
 * @brief Subclass of QGraphicsTextItem which simply forces
 * its boundingRect to be the same as its parent.
 * By default a QGraphicsTextItem will size to its text and
 * we want it to size to the parent item.
 *
 */
class BoundedTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    explicit BoundedTextItem(QGraphicsItem *parent);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *o, QWidget *w) override;
    void setBackgroudColor(const QColor &bc);
    void setForegroundColor(const QColor &fc);

    //! @return background opacity, 0..1.0
    qreal backgroudOpacity() const;

    //! Sets background opacity, 0..1.0
    void setBackgroudOpacity(qreal opacity);

    void setDisplayFont(const QFont &f);


protected:
    void keyReleaseEvent ( QKeyEvent * event ) override;

private:
    QColor m_backgroundColor;
    QColor m_foregroundColor;
    QFont m_font;

    qreal m_backgroundOpacity;

Q_SIGNALS:
    void exitEditMode();

};

#endif // KREPORT_BOUNDEDTEXTITEM_H
