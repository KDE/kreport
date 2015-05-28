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

#include "KoReportItemLine.h"

#include <KPropertySet>
#include "renderobjects.h"

KoReportItemLine::KoReportItemLine()
{
    createProperties();
}

KoReportItemLine::KoReportItemLine(QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;
    QPointF _s, _e;

    m_name->setValue(element.toElement().attribute(QLatin1String("report:name")));
    Z = element.toElement().attribute(QLatin1String("report:z-index")).toDouble();

    _s.setX(KoUnit::parseValue(element.toElement().attribute(QLatin1String("svg:x1"), QLatin1String("1cm"))));
    _s.setY(KoUnit::parseValue(element.toElement().attribute(QLatin1String("svg:y1"), QLatin1String("1cm"))));
    _e.setX(KoUnit::parseValue(element.toElement().attribute(QLatin1String("svg:x2"), QLatin1String("1cm"))));
    _e.setY(KoUnit::parseValue(element.toElement().attribute(QLatin1String("svg:y2"), QLatin1String("2cm"))));
    m_start.setPointPos(_s);
    m_end.setPointPos(_e);

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == QLatin1String("report:line-style")) {
            KRLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), ls)) {
                m_lineWeight->setValue(ls.weight);
                m_lineColor->setValue(ls.lineColor);
                m_lineStyle->setValue(QPen(ls.style));
            }
        } else {
            qWarning() << "while parsing line element encountered unknow element: " << n;
        }
    }
}

KoReportItemLine::~KoReportItemLine()
{
    delete m_set;
}

void KoReportItemLine::createProperties()
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

KRLineStyleData KoReportItemLine::lineStyle()
{
    KRLineStyleData ls;
    ls.weight = m_lineWeight->value().toInt();
    ls.lineColor = m_lineColor->value().value<QColor>();
    ls.style = (Qt::PenStyle)m_lineStyle->value().toInt();
    return ls;
}

unsigned int KoReportItemLine::weight() const
{
    return m_lineWeight->value().toInt();
}
void KoReportItemLine::setWeight(int w)
{
    m_lineWeight->setValue(w);
}

QString KoReportItemLine::typeName() const
{
    return QLatin1String("line");
}

int KoReportItemLine::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                       const QVariant &data, KRScriptHandler *script)
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

void KoReportItemLine::setUnit(const KoUnit &u)
{
    m_start.setUnit(u);
    m_end.setUnit(u);
}

KRPos KoReportItemLine::startPosition() const
{
    return m_start;
}

KRPos KoReportItemLine::endPosition() const
{
    return m_end;
}

