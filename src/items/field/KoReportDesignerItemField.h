/*
 * OpenRPT report writer and rendering engine
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


#ifndef __KOREPORTDESIGNERITEMFIELD_H__
#define __KOREPORTDESIGNERITEMFIELD_H__

#include <QGraphicsRectItem>
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include "KoReportItemField.h"
#include <KoReportDesignerItemRectBase.h>
//
// ReportEntityField
//
class KoReportDesignerItemField : public KoReportItemField, public KoReportDesignerItemRectBase 
{
  Q_OBJECT
public:
    //Used when creating new basic field
    KoReportDesignerItemField(KoReportDesigner *, QGraphicsScene * scene, const QPointF &pos);
    //Used when loading from file
    KoReportDesignerItemField(QDomNode & element, KoReportDesigner *, QGraphicsScene * scene);
    virtual ~KoReportDesignerItemField();
    virtual void buildXML(QDomDocument & doc, QDomElement & parent);

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget = 0);
    virtual KoReportDesignerItemField* clone();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);

private:
    void init(QGraphicsScene*, KoReportDesigner * d);
    QRect getTextRect() const;

private Q_SLOTS:
    void slotPropertyChanged(KoProperty::Set &, KoProperty::Property &);
};

#endif
