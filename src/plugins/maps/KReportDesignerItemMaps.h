/*
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2011 by Radoslaw Wicik (radoslaw@wicik.pl)
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

#ifndef __REPORTENTITYIMAGE_H__
#define __REPORTENTITYIMAGE_H__


#include <KPropertySet>

#include <KReportDesignerItemRectBase.h>
#include "KoReportItemMaps.h"

class KReportDesignerItemMaps : public KReportItemMaps, public KReportDesignerItemRectBase
{
  Q_OBJECT
public:
    KReportDesignerItemMaps(KoReportDesigner *, QGraphicsScene* scene, const QPointF &pos);
    KReportDesignerItemMaps(QDomNode *element, KoReportDesigner *, QGraphicsScene* scene);

    virtual ~KReportDesignerItemMaps();
    virtual void buildXML(QDomDocument *doc, QDomElement *parent);
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual KReportDesignerItemMaps* clone();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);

private:
    void init(QGraphicsScene*, KoReportDesigner*);

private Q_SLOTS:
    void slotPropertyChanged(KPropertySet &, KProperty &);

};

#endif
