/*
 * Kexi Report Plugin
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
#include "KoReportDesignerItemChart.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <KoReportDesignerItemBase.h>
#include "KoReportDesigner.h"

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <klocalizedstring.h>

#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <koproperty/EditorView.h>

void KoReportDesignerItemChart::init(QGraphicsScene* scene, KoReportDesigner *designer)
{
    m_reportDesigner = designer;
    setPos(0, 0);

    if (scene)
        scene->addItem(this);

    connect(m_set, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
            this, SLOT(slotPropertyChanged(KoProperty::Set&, KoProperty::Property&)));

    KoReportDesignerItemRectBase::init(&m_pos, &m_size, m_set);
    setZValue(Z);

    connect(m_reportDesigner, SIGNAL(reportDataChanged()), this, SLOT(slotReportDataChanged()));
}

KoReportDesignerItemChart::KoReportDesignerItemChart(KoReportDesigner * rd, QGraphicsScene* scene, const QPointF &pos)
        : KoReportDesignerItemRectBase(rd)
{
    init(scene, rd);
    m_size.setSceneSize(QSizeF(m_dpiX, m_dpiY));
    setSceneRect(m_pos.toScene(), m_size.toScene());
    m_pos.setScenePos(pos);
    m_name->setValue(m_reportDesigner->suggestEntityName("chart"));
}

KoReportDesignerItemChart::KoReportDesignerItemChart(QDomNode & element, KoReportDesigner * rd, QGraphicsScene* scene) : KoReportDesignerItemRectBase(rd), KoReportItemChart(element)
{
    init(scene, rd);
    populateData();
    setSceneRect(m_pos.toScene(), m_size.toScene());

}

KoReportDesignerItemChart::~KoReportDesignerItemChart()
{
}

void KoReportDesignerItemChart::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // store any values we plan on changing so we can restore them
    QFont f = painter->font();
    QPen  p = painter->pen();
    QColor bg = Qt::white;

    painter->fillRect(QGraphicsRectItem::rect(), bg);

    if (m_chartWidget) {
        m_chartWidget->setFixedSize(m_size.toScene().toSize());
        painter->drawImage(rect().left(), rect().top(),
                           QPixmap::grabWidget(m_chartWidget).toImage(),
                           0, 0, rect().width(), rect().height());
    }
    bg.setAlpha(255);

    painter->setBackground(bg);
    painter->setPen(Qt::black);
    painter->drawText(rect(), 0, dataSourceAndObjectTypeName(m_dataSource->value().toString(), "chart"));
    painter->setPen(QPen(QColor(224, 224, 224)));
    painter->drawRect(rect());
    painter->setBackgroundMode(Qt::TransparentMode);

    drawHandles(painter);

    // restore an values before we started just in case
    painter->setFont(f);
    painter->setPen(p);
}

KoReportDesignerItemChart* KoReportDesignerItemChart::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement("clone");;
    QDomNode n;
    buildXML(d, e);
    n = e.firstChild();
    return new KoReportDesignerItemChart(n, designer(), 0);
}

void KoReportDesignerItemChart::buildXML(QDomDocument & doc, QDomElement & parent)
{
    QDomElement entity = doc.createElement("report:chart");

    // properties
    addPropertyAsAttribute(&entity, m_name);
    addPropertyAsAttribute(&entity, m_dataSource);
    addPropertyAsAttribute(&entity, m_chartType);
    addPropertyAsAttribute(&entity, m_chartSubType);
    addPropertyAsAttribute(&entity, m_threeD);
    addPropertyAsAttribute(&entity, m_colorScheme);
    addPropertyAsAttribute(&entity, m_aa);
    addPropertyAsAttribute(&entity, m_xTitle);
    addPropertyAsAttribute(&entity, m_yTitle);
    addPropertyAsAttribute(&entity, m_backgroundColor);
    addPropertyAsAttribute(&entity, m_displayLegend);
    addPropertyAsAttribute(&entity, m_linkChild);
    addPropertyAsAttribute(&entity, m_linkMaster);
    entity.setAttribute("report:z-index", zValue());

    // bounding rect
    buildXMLRect(doc, entity, &m_pos, &m_size);

    parent.appendChild(entity);
}

void KoReportDesignerItemChart::slotPropertyChanged(KoProperty::Set &s, KoProperty::Property &p)
{       
    if (p.name() == "Name") {
        //For some reason p.oldValue returns an empty string
        if (!m_reportDesigner->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(m_oldName);
        } else {
            m_oldName = p.value().toString();
        }
    } else if (p.name() == "three-dimensions") {
        set3D(p.value().toBool());
    } else if (p.name() == "antialiased") {
        setAA(p.value().toBool());
    } else if (p.name() == "color-scheme") {
        setColorScheme(p.value().toString());
    } else if (p.name() == "data-source") {
        populateData();
    } else if (p.name() == "title-x-axis" ||   p.name() == "title-y-axis") {
        setAxis(m_xTitle->value().toString(), m_yTitle->value().toString());
    } else if (p.name() == "background-color") {
        setBackgroundColor(p.value().value<QColor>());
    } else if (p.name() == "display-legend") {
        setLegend(p.value().toBool());
    } else if (p.name() == "chart-type") {
        if (m_chartWidget) {
            m_chartWidget->setType((KDChart::Widget::ChartType) m_chartType->value().toInt());
        }
    } else if (p.name() == "chart-sub-type") {
        if (m_chartWidget) {
            m_chartWidget->setSubType((KDChart::Widget::SubType) m_chartSubType->value().toInt());
        }
    }

    KoReportDesignerItemRectBase::propertyChanged(s, p);
    if (m_reportDesigner) m_reportDesigner->setModified(true);
}

void KoReportDesignerItemChart::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if (m_reportDesigner->reportData()) {
        QStringList ql = m_reportDesigner->reportData()->dataSources();
        QStringList qn = m_reportDesigner->reportData()->dataSourceNames();
        m_dataSource->setListData(ql, qn);
    }
    KoReportDesignerItemRectBase::mousePressEvent(event);
}

void KoReportDesignerItemChart::slotReportDataChanged()
{
    setConnection(m_reportDesigner->reportData());
}
