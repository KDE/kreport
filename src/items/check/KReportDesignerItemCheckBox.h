/* This file is part of the KDE project
 * Copyright (C) 2009-2010 by Adam Pigg (adam@piggz.co.uk)
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

#ifndef KREPORTDESIGNERITEMCHECK_H
#define KREPORTDESIGNERITEMCHECK_H

#include "KReportDesignerItemRectBase.h"
#include "KReportItemCheck.h"

#include <KPropertySet>
#include <QGraphicsRectItem>

const int KREPORT_ITEM_CHECK_DEFAULT_WIDTH = 15;
const int KREPORT_ITEM_CHECK_DEFAULT_HEIGHT = 15;

class KReportDesignerItemCheckBox : public KReportItemCheckBox, public KReportDesignerItemRectBase
{
    Q_OBJECT
public:
    KReportDesignerItemCheckBox(KReportDesigner *designer, QGraphicsScene *scene,
                                const QPointF &pos);
    KReportDesignerItemCheckBox(const QDomNode &element, KReportDesigner *designer,
                                QGraphicsScene *scene);

    ~KReportDesignerItemCheckBox() override;

    void buildXML(QDomDocument *doc, QDomElement *parent) override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    KReportDesignerItemCheckBox* clone() override;

private:
    void init(QGraphicsScene *scene);

private Q_SLOTS:
    void slotPropertyChanged(KPropertySet &, KProperty &);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event) override;
};

#endif // KREPORTDESIGNERITEMCHECK_H
