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

#include "KReportDesignerItemWeb.h"
#include <KReportDesignerItemBase.h>
#include <KReportDesigner.h>

#include <KProperty>
#include <KPropertySet>

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDomDocument>
#include <QPainter>
#include "kreportplugin_debug.h"

void KReportDesignerItemWeb::init(QGraphicsScene *scene)
{
    if (scene)
        scene->addItem(this);

    connect(propertySet(), SIGNAL(propertyChanged(KPropertySet&,KProperty&)), this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));

    setZValue(z());
}

KReportDesignerItemWeb::KReportDesignerItemWeb(KReportDesigner *rw, QGraphicsScene *scene,
                                                 const QPointF &pos) : KReportDesignerItemRectBase(rw, this)
{
    Q_UNUSED(pos);
    init(scene);
    setSceneRect(properRect(*rw, KREPORT_ITEM_RECT_DEFAULT_WIDTH, KREPORT_ITEM_RECT_DEFAULT_WIDTH));
    nameProperty()->setValue(designer()->suggestEntityName(typeName()));
}

KReportDesignerItemWeb::KReportDesignerItemWeb(const QDomNode &element, KReportDesigner *rw,
                                                 QGraphicsScene *scene)      
    : KReportItemWeb(element), KReportDesignerItemRectBase(rw, this)
{
    init(scene);
    setSceneRect(KReportItemBase::scenePosition(item()->position()), KReportItemBase::sceneSize(item()->size()));
}

KReportDesignerItemWeb *KReportDesignerItemWeb::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement(QLatin1String("clone"));
    QDomNode n;
    buildXML(&d, &e);
    n = e.firstChild();
    return new KReportDesignerItemWeb(n, designer(), 0);
}

KReportDesignerItemWeb::~KReportDesignerItemWeb() //done,compared
{
    // do we need to clean anything up?
}

void KReportDesignerItemWeb::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawRect(QGraphicsRectItem::rect());
    painter->drawText(rect(), 0, dataSourceAndObjectTypeName(itemDataSource(), QLatin1String("web-view")));

    painter->setBackgroundMode(Qt::TransparentMode);

    drawHandles(painter);
}

void KReportDesignerItemWeb::buildXML(QDomDocument *doc, QDomElement *parent)
{
    Q_UNUSED(doc);
    Q_UNUSED(parent);
    QDomElement entity = doc->createElement(QLatin1String("report:") + typeName());

    // properties
    addPropertyAsAttribute(&entity, m_controlSource);
    addPropertyAsAttribute(&entity, nameProperty());
    entity.setAttribute(QLatin1String("report:z-index"), zValue());
    buildXMLRect(doc, &entity, this);
    parent->appendChild(entity);
}

void KReportDesignerItemWeb::slotPropertyChanged(KPropertySet &s, KProperty &p)
{
    if (p.name() == "name") {
        if (!designer()->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(oldName());
        }
        else {
            setOldName(p.value().toString());
        }
    }

    KReportDesignerItemRectBase::propertyChanged(s, p);
    if (designer()) {
        designer()->setModified(true);
    }
}

void KReportDesignerItemWeb::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_controlSource->setListData(designer()->fieldKeys(), designer()->fieldNames());
    KReportDesignerItemRectBase::mousePressEvent(event);
}
