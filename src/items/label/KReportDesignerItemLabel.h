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

#ifndef KREPORTDESIGNERITEMLABEL_H
#define KREPORTDESIGNERITEMLABEL_H

#include "KReportItemLabel.h"
#include "KReportDesignerItemRectBase.h"
#include "KReportBoundedTextItem.h"

#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <KPropertySet>

class KReportDesignerItemLabel : public KReportItemLabel, public KReportDesignerItemRectBase
{
  Q_OBJECT
public:
    KReportDesignerItemLabel(KReportDesigner *, QGraphicsScene * scene, const QPointF &pos);
    KReportDesignerItemLabel(const QDomNode & element, KReportDesigner *, QGraphicsScene * scene);
    ~KReportDesignerItemLabel() override;

    void buildXML(QDomDocument *doc, QDomElement *parent) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    KReportDesignerItemLabel* clone() override;

public Q_SLOTS:
    void enterInlineEditingMode() override;
    void exitInlineEditingMode() override;

protected:
    void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event ) override;
    void keyReleaseEvent ( QKeyEvent * event ) override;

private:
    void init(QGraphicsScene*, KReportDesigner*);
    QRectF getTextRect() const;
    BoundedTextItem *m_inlineEdit;

private Q_SLOTS:
    void slotPropertyChanged(KPropertySet &, KProperty &);

};

#endif
