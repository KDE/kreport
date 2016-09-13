/* This file is part of the KDE project
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
#ifndef REPORTENTITYCHART_H
#define REPORTENTITYCHART_H


#include "KReportItemChart.h"
#include "KReportDesignerItemRectBase.h"

/**
*/
class KReportDesignerItemChart : public KReportItemChart, public KReportDesignerItemRectBase
{
    Q_OBJECT
public:
    KReportDesignerItemChart(KoReportDesigner *, QGraphicsScene* scene, const QPointF &pos);
    KReportDesignerItemChart(QDomNode *element, KoReportDesigner *, QGraphicsScene* scene);

    virtual ~KReportDesignerItemChart();
    virtual void buildXML(QDomDocument *doc, QDomElement *parent);
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual KReportDesignerItemChart* clone();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);

private:
    void init(QGraphicsScene*, KoReportDesigner *d);

private Q_SLOTS:
    void slotPropertyChanged(KPropertySet &, KProperty &);
    void slotReportDataChanged();

};

#endif
