/* This file is part of the KDE project
   Copyright Shreya Pandit <shreya@shreyapandit.com>
   Copyright 2011 Adam Pigg <adam@piggz.co.uk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KREPORTDESIGNERITEMWEB_H
#define KREPORTDESIGNERITEMWEB_H

#include <KReportDesignerItemRectBase.h>
#include "KReportItemWeb.h"

class QGraphicsScene;

/**
*/
class KReportDesignerItemWeb : public KReportItemWeb, public KReportDesignerItemRectBase
{
    Q_OBJECT
public:
    KReportDesignerItemWeb(KReportDesigner *rw, QGraphicsScene *scene, const QPointF &pos);
    KReportDesignerItemWeb(const QDomNode &element, KReportDesigner *rw, QGraphicsScene *scene);
    virtual ~KReportDesignerItemWeb();

    virtual void buildXML(QDomDocument *doc, QDomElement *parent);
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    virtual KReportDesignerItemWeb *clone();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    void init(QGraphicsScene *scene);

private Q_SLOTS:
    void slotPropertyChanged(KPropertySet &, KProperty &);
};

#endif
