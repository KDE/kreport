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

#include "KReportItemLine.h"
#include "KReportRenderObjects.h"
#include "kreport_debug.h"

#include <KPropertySet>

#include <QDomNode>

KReportItemLine::KReportItemLine()
{
    createProperties();
}

KReportItemLine::KReportItemLine(const QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;
    QPointF _s, _e;

    m_name->setValue(element.toElement().attribute(QLatin1String("report:name")));
    Z = element.toElement().attribute(QLatin1String("report:z-index")).toDouble();

    _s.setX(KReportUnit::parseValue(element.toElement().attribute(QLatin1String("svg:x1"), QLatin1String("1cm"))));
    _s.setY(KReportUnit::parseValue(element.toElement().attribute(QLatin1String("svg:y1"), QLatin1String("1cm"))));
    _e.setX(KReportUnit::parseValue(element.toElement().attribute(QLatin1String("svg:x2"), QLatin1String("1cm"))));
    _e.setY(KReportUnit::parseValue(element.toElement().attribute(QLatin1String("svg:y2"), QLatin1String("2cm"))));
    m_start.setPointPos(_s);
    m_end.setPointPos(_e);

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == QLatin1String("report:line-style")) {
            KReportLineStyle ls;
            if (parseReportLineStyleData(node.toElement(), &ls)) {
                m_lineWeight->setValue(ls.width());
                m_lineColor->setValue(ls.color());
                m_lineStyle->setValue(int(ls.penStyle()));
            }
        } else {
            kreportWarning() << "while parsing line element encountered unknow element: " << n;
        }
    }
}

KReportItemLine::~KReportItemLine()
{
    delete m_set;
}

void KReportItemLine::createProperties()
{
    m_set = new KPropertySet(0, QLatin1String("Line"));

    m_lineWeight = new KProperty("line-weight", 1, tr("Line Weight"));
    m_lineColor = new KProperty("line-color", QColor(Qt::black), tr("Line Color"));
    m_lineStyle = new KProperty("line-style", (int)Qt::SolidLine, tr("Line Style"), tr("Line Style"), KProperty::LineStyle);
    m_start.setName(QLatin1String("Start"));
    m_end.setName(QLatin1String("End"));

    m_set->addProperty(m_name);
    m_set->addProperty(m_start.property());
    m_set->addProperty(m_end.property());
    m_set->addProperty(m_lineWeight);
    m_set->addProperty(m_lineColor);
    m_set->addProperty(m_lineStyle);
}

KReportLineStyle KReportItemLine::lineStyle() const
{
    KReportLineStyle ls;
    ls.setWidth(m_lineWeight->value().toInt());
    ls.setColor(m_lineColor->value().value<QColor>());
    ls.setPenStyle((Qt::PenStyle)m_lineStyle->value().toInt());
    return ls;
}

int KReportItemLine::weight() const
{
    return m_lineWeight->value().toInt();
}

void KReportItemLine::setWeight(int w)
{
    m_lineWeight->setValue(w);
}

QString KReportItemLine::typeName() const
{
    return QLatin1String("line");
}

int KReportItemLine::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                       const QVariant &data, KReportScriptHandler *script)
{
    Q_UNUSED(script)
    Q_UNUSED(data)

    OROLine * ln = new OROLine();
    QPointF s = m_start.toScene();
    QPointF e = m_end.toScene();

    s += offset;
    e += offset;

    ln->setStartPoint(s);
    ln->setEndPoint(e);
    ln->setLineStyle(lineStyle());
    if (page) page->addPrimitive(ln);

    OROLine *l2 = dynamic_cast<OROLine*>(ln->clone());
    l2->setStartPoint(m_start.toPoint());
    l2->setEndPoint(m_end.toPoint());
    if (section) section->addPrimitive(l2);

    return 0;
}

void KReportItemLine::setUnit(const KReportUnit &u)
{
    m_start.setUnit(u);
    m_end.setUnit(u);
}

KReportPosition KReportItemLine::startPosition() const
{
    return m_start;
}

KReportPosition KReportItemLine::endPosition() const
{
    return m_end;
}

