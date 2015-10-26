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

#include "KoReportDesignerItemWeb.h"
#include <KoReportDesignerItemBase.h>
#include <KoReportDesigner.h>

#include <KProperty>
#include <KPropertySet>

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDomDocument>
#include <QPainter>
#include "kreportplugin_debug.h"

void KReportDesignerItemWeb::init(QGraphicsScene *scene, KoReportDesigner *d) //done,compared,add function if necessary
{
    kreportpluginDebug();
    if (scene)
        scene->addItem(this);

    connect(m_set, SIGNAL(propertyChanged(KPropertySet&,KProperty&)), this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));
    KoReportDesignerItemRectBase::init(&m_pos, &m_size, m_set, d);
    setZValue(Z);
}

KReportDesignerItemWeb::KReportDesignerItemWeb(KoReportDesigner *rw, QGraphicsScene *scene,
                                                 const QPointF &pos)     //done,compared
    : KoReportDesignerItemRectBase(rw)
{
    Q_UNUSED(pos);
    init(scene, rw);
    setSceneRect(properRect(*rw, KREPORT_ITEM_RECT_DEFAULT_WIDTH, KREPORT_ITEM_RECT_DEFAULT_WIDTH));
    m_name->setValue(m_reportDesigner->suggestEntityName(typeName()));
}

KReportDesignerItemWeb::KReportDesignerItemWeb(QDomNode *element, KoReportDesigner *rw,
                                                 QGraphicsScene *scene)      //done,compared
    : KoReportItemWeb(element), KoReportDesignerItemRectBase(rw)
{
    init(scene, rw);
    setSceneRect(m_pos.toScene(), m_size.toScene());
}

KReportDesignerItemWeb *KReportDesignerItemWeb::clone() //done,compared
{
    QDomDocument d;
    QDomElement e = d.createElement("clone");
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
    painter->drawText(rect(), 0, dataSourceAndObjectTypeName(itemDataSource(), "web-view"));

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
    addPropertyAsAttribute(&entity, m_name);
    entity.setAttribute("report:z-index", zValue());
    buildXMLRect(doc, &entity, &m_pos, &m_size);
    parent->appendChild(entity);
}

void KReportDesignerItemWeb::slotPropertyChanged(KPropertySet &s, KProperty &p)
{
    if (p.name() == "Name") {
        if (!m_reportDesigner->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(m_oldName);
        }
        else {
            m_oldName = p.value().toString();
        }
    }

    KoReportDesignerItemRectBase::propertyChanged(s, p);
    if (m_reportDesigner) {
        m_reportDesigner->setModified(true);
    }
}

void KReportDesignerItemWeb::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_controlSource->setListData(m_reportDesigner->fieldKeys(), m_reportDesigner->fieldNames());
    KoReportDesignerItemRectBase::mousePressEvent(event);
}
