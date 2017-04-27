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

#include "KReportDesignerItemText.h"
#include "KReportDesignerItemBase.h"
#include "KReportDesigner.h"
#include "KReportLineStyle.h"

#include <KPropertySet>
#include <QDomDocument>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "kreportplugin_debug.h"

//
// class ReportEntityText
//
// methods (constructors)

void KReportDesignerItemText::init(QGraphicsScene *scene)
{
    //setFlags(ItemIsSelectable | ItemIsMovable);
    if (scene)
        scene->addItem(this);

    connect(propertySet(), SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));

    m_controlSource->setListData(designer()->fieldKeys(), designer()->fieldNames());
    setZValue(z());

    updateRenderText(m_controlSource->value().toString(), m_itemValue->value().toString(),
                     QLatin1String("textarea"));
}

KReportDesignerItemText::KReportDesignerItemText(KReportDesigner * rw, QGraphicsScene * scene, const QPointF &pos)
        : KReportDesignerItemRectBase(rw, this)
{
    Q_UNUSED(pos);
    init(scene);
    setSceneRect(properRect(*rw, getTextRect().width(), getTextRect().height()));
    nameProperty()->setValue(designer()->suggestEntityName(typeName()));
}

KReportDesignerItemText::KReportDesignerItemText(const QDomNode & element, KReportDesigner *d, QGraphicsScene *scene)
        : KReportItemText(element), KReportDesignerItemRectBase(d, this)
{
    init(scene);
    setSceneRect(KReportItemBase::scenePosition(item()->position()), KReportItemBase::sceneSize(item()->size()));
}

KReportDesignerItemText* KReportDesignerItemText::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement(QLatin1String("clone"));
    QDomNode n;
    buildXML(&d, &e);
    n = e.firstChild();
    return new KReportDesignerItemText(n, designer(), nullptr);
}

KReportDesignerItemText::~KReportDesignerItemText

()
{}

QRect KReportDesignerItemText::getTextRect() const
{
    return QFontMetrics(font()).boundingRect(int (x()), int (y()), 0, 0, textFlags(), renderText());
}

void KReportDesignerItemText::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget)

    // store any values we plan on changing so we can restore them
    QFont f = painter->font();
    QPen  p = painter->pen();

    painter->setFont(font());
    painter->setBackgroundMode(Qt::TransparentMode);

    QColor bg = m_backgroundColor->value().value<QColor>();
    bg.setAlphaF(m_backgroundOpacity->value().toReal()*0.01);

    painter->setPen(m_foregroundColor->value().value<QColor>());

    painter->fillRect(rect(),  bg);
    painter->drawText(rect(), textFlags(), renderText());

    if ((Qt::PenStyle)m_lineStyle->value().toInt() == Qt::NoPen || m_lineWeight->value().toInt() <= 0) {
        painter->setPen(QPen(Qt::lightGray));
    } else {
        painter->setPen(QPen(m_lineColor->value().value<QColor>(), m_lineWeight->value().toInt(), (Qt::PenStyle)m_lineStyle->value().toInt()));
    }
    painter->drawRect(rect());

    painter->setPen(m_foregroundColor->value().value<QColor>());

    drawHandles(painter);

    // restore an values before we started just in case
    painter->setFont(f);
    painter->setPen(p);
}

void KReportDesignerItemText::buildXML(QDomDocument *doc, QDomElement *parent)
{
    //kreportpluginDebug();
    QDomElement entity = doc->createElement(QLatin1String("report:") + typeName());

    // properties
    addPropertyAsAttribute(&entity, nameProperty());
    addPropertyAsAttribute(&entity, m_controlSource);
    addPropertyAsAttribute(&entity, m_verticalAlignment);
    addPropertyAsAttribute(&entity, m_horizontalAlignment);
    entity.setAttribute(QLatin1String("report:bottom-padding"), m_bottomPadding);
    entity.setAttribute(QLatin1String("report:z-index"), z());
    addPropertyAsAttribute(&entity, m_itemValue);

    // bounding rect
    buildXMLRect(doc, &entity, this);

    //text style info
    buildXMLTextStyle(doc, &entity, textStyle());

    //Line Style
    buildXMLLineStyle(doc, &entity, lineStyle());

    parent->appendChild(entity);
}

void KReportDesignerItemText::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    m_controlSource->setListData(designer()->fieldKeys(), designer()->fieldNames());
    KReportDesignerItemRectBase::mousePressEvent(event);
}


void KReportDesignerItemText::slotPropertyChanged(KPropertySet &s, KProperty &p)
{
    Q_UNUSED(s);

    if (p.name() == "name") {
        //For some reason p.oldValue returns an empty string
        if (!designer()->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(oldName());
        } else {
            setOldName(p.value().toString());
        }
    }

    KReportDesignerItemRectBase::propertyChanged(s, p);
    if (designer()) {
        designer()->setModified(true);
    }

    updateRenderText(m_controlSource->value().toString(), m_itemValue->value().toString(),
                     QLatin1String("textarea"));
}
