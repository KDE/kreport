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
#include "KReportUtils.h"
#include "KReportUtils_p.h"
#include "kreport_debug.h"

#include <KPropertySet>

#include <QCoreApplication>
#include <QDomNode>

KReportItemLine::KReportItemLine()
{
    createProperties();
}

KReportItemLine::KReportItemLine(const QDomNode & element)
    : KReportItemLine()
{
    nameProperty()->setValue(KReportUtils::readNameAttribute(element.toElement()));
    setZ(KReportUtils::readZAttribute(element.toElement()));

    const QPointF s(KReportUnit::parseValue(element.toElement().attribute(
                        QLatin1String("svg:x1"), DEFAULT_ELEMENT_POS_STRING)),
                    KReportUnit::parseValue(element.toElement().attribute(
                        QLatin1String("svg:y1"), DEFAULT_ELEMENT_POS_STRING)));
    const QPointF e(KReportUnit::parseValue(element.toElement().attribute(
                        QLatin1String("svg:x2"), DEFAULT_ELEMENT_POS_STRING)),
                    KReportUnit::parseValue(element.toElement().attribute(
                        QLatin1String("svg:y2"),
                        QString::number(POINT_TO_CM(DEFAULT_ELEMENT_POS_PT.y()
                                                    + DEFAULT_ELEMENT_SIZE_PT.height()))
                            + QLatin1String("cm"))));
    setStartPosition(s);
    setEndPosition(e);

    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;
    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == QLatin1String("report:line-style")) {
            KReportLineStyle ls;
            if (parseReportLineStyleData(node.toElement(), &ls)) {
                m_lineWeight->setValue(ls.weight());
                m_lineColor->setValue(ls.color());
                m_lineStyle->setValue(static_cast<int>(ls.penStyle()));
            }
        } else {
            kreportWarning() << "while parsing line element encountered unknown element: " << n;
        }
    }
}

KReportItemLine::~KReportItemLine()
{
}

void KReportItemLine::createProperties()
{
    m_start = new KProperty("startposition", QPointF(), QCoreApplication::translate("StartPosition", "Start Position"));
    m_end = new KProperty("endposition", QPointF(), QCoreApplication::translate("EndPosition", "End Position"));

    m_lineWeight = new KProperty("line-weight", 1.0, tr("Line Weight"));
    m_lineWeight->setOption("step", 1.0);
    m_lineColor = new KProperty("line-color", QColor(Qt::black), tr("Line Color"));
    m_lineStyle = new KProperty("line-style", static_cast<int>(Qt::SolidLine), tr("Line Style"),
                                QString(), KProperty::LineStyle);

    //Remove the unused properies from KReportItemBase
    propertySet()->property("size").setVisible(false);
    propertySet()->property("position").setVisible(false);

    propertySet()->addProperty(m_start);
    propertySet()->addProperty(m_end);
    propertySet()->addProperty(m_lineWeight);
    propertySet()->addProperty(m_lineColor);
    propertySet()->addProperty(m_lineStyle);
}

KReportLineStyle KReportItemLine::lineStyle() const
{
    KReportLineStyle ls;
    ls.setWeight(m_lineWeight->value().toReal());
    ls.setColor(m_lineColor->value().value<QColor>());
    ls.setPenStyle((Qt::PenStyle)m_lineStyle->value().toInt());
    return ls;
}

qreal KReportItemLine::weight() const
{
    return m_lineWeight->value().toReal();
}

void KReportItemLine::setWeight(qreal w)
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
    QPointF s = scenePosition(startPosition());
    QPointF e = scenePosition(endPosition());

    s += offset;
    e += offset;

    ln->setStartPoint(s);
    ln->setEndPoint(e);
    ln->setLineStyle(lineStyle());
    if (page) page->insertPrimitive(ln);

    OROLine *l2 = dynamic_cast<OROLine*>(ln->clone());
    if (l2) {
        l2->setStartPoint(startPosition());
        l2->setEndPoint(endPosition());

        if (section) section->addPrimitive(l2);
    }
    return 0;
}

void KReportItemLine::setUnit(const KReportUnit &u)
{
    if (unit() == u) {
        return;
    }
    KReportUnit oldunit = unit();
    KReportItemBase::setUnit(u);
    
    // convert values
    m_start->setValue(KReportUnit::convertFromUnitToUnit(m_start->value().toPointF(), oldunit, u),
                      KProperty::ValueOption::IgnoreOld);
    m_end->setValue(KReportUnit::convertFromUnitToUnit(m_end->value().toPointF(), oldunit, u),
                    KProperty::ValueOption::IgnoreOld);

    m_start->setOption("suffix", u.symbol());
    m_end->setOption("suffix", u.symbol());
}

QPointF KReportItemLine::startPosition() const
{
    return unit().convertToPoint(m_start->value().toPointF());
}

void KReportItemLine::setStartPosition(const QPointF &ptPos)
{
    m_start->setValue(unit().convertFromPoint(ptPos));
}

QPointF KReportItemLine::endPosition() const
{
    return unit().convertToPoint(m_end->value().toPointF());
}

void KReportItemLine::setEndPosition(const QPointF &ptPos)
{
    m_end->setValue(unit().convertFromPoint(ptPos));
}
