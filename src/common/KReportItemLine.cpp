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

#include <QCoreApplication>
#include <QDomNode>

KReportItemLine::KReportItemLine()
{
    createProperties();
}

KReportItemLine::KReportItemLine(const QDomNode & element)
    : KReportItemLine()
{
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;
    QPointF _s, _e;

    nameProperty()->setValue(element.toElement().attribute(QLatin1String("report:name")));
    setZ(element.toElement().attribute(QLatin1String("report:z-index")).toDouble());

    _s.setX(KReportUnit::parseValue(element.toElement().attribute(QLatin1String("svg:x1"), QLatin1String("1cm"))));
    _s.setY(KReportUnit::parseValue(element.toElement().attribute(QLatin1String("svg:y1"), QLatin1String("1cm"))));
    _e.setX(KReportUnit::parseValue(element.toElement().attribute(QLatin1String("svg:x2"), QLatin1String("1cm"))));
    _e.setY(KReportUnit::parseValue(element.toElement().attribute(QLatin1String("svg:y2"), QLatin1String("2cm"))));
    start->setValue(_s);
    end->setValue(_e);

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == QLatin1String("report:line-style")) {
            KReportLineStyle ls;
            if (parseReportLineStyleData(node.toElement(), &ls)) {
                lineWeight->setValue(ls.weight());
                lineColor->setValue(ls.color());
                lineStyle->setValue(static_cast<int>(ls.penStyle()));
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
    start = new KProperty("startposition", QPointF(), QCoreApplication::translate("StartPosition", "Start Position"));
    end = new KProperty("endposition", QPointF(), QCoreApplication::translate("EndPosition", "End Position"));

    lineWeight = new KProperty("line-weight", 1.0, tr("Line Weight"));
    lineWeight->setOption("step", 1.0);
    lineColor = new KProperty("line-color", QColor(Qt::black), tr("Line Color"));
    lineStyle = new KProperty("line-style", static_cast<int>(Qt::SolidLine), tr("Line Style"),
                                QString(), KProperty::LineStyle);

    //Remove the unused properies from KReportItemBase
    propertySet()->removeProperty("size");
    propertySet()->removeProperty("position");

    propertySet()->addProperty(start);
    propertySet()->addProperty(end);
    propertySet()->addProperty(lineWeight);
    propertySet()->addProperty(lineColor);
    propertySet()->addProperty(lineStyle);
}

KReportLineStyle KReportItemLine::lineStyleValue() const
{
    KReportLineStyle ls;
    ls.setWeight(lineWeight->value().toReal());
    ls.setColor(lineColor->value().value<QColor>());
    ls.setPenStyle((Qt::PenStyle)lineStyle->value().toInt());
    return ls;
}

qreal KReportItemLine::weightValue() const
{
    return lineWeight->value().toReal();
}

void KReportItemLine::setWeightValue(qreal w)
{
    lineWeight->setValue(w);
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
    QPointF s = scenePosition(start->value().toPointF());
    QPointF e = scenePosition(end->value().toPointF());

    s += offset;
    e += offset;

    ln->setStartPoint(s);
    ln->setEndPoint(e);
    ln->setLineStyle(lineStyleValue());
    if (page) page->insertPrimitive(ln);

    OROLine *l2 = dynamic_cast<OROLine*>(ln->clone());
    if (l2) {
        l2->setStartPoint(start->value().toPointF());
        l2->setEndPoint(end->value().toPointF());

        if (section) section->addPrimitive(l2);
    }
    return 0;
}

void KReportItemLine::setUnit(const KReportUnit &u)
{
    start->setOption("unit", u.symbol());
    end->setOption("unit", u.symbol());
}

QPointF KReportItemLine::startPosition() const
{
    return start->value().toPointF();
}

QPointF KReportItemLine::endPosition() const
{
    return end->value().toPointF();
}

