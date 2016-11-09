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

#include "KReportItemCheck.h"
#include "KReportRenderObjects.h"
#include "kreportplugin_debug.h"
#ifdef KREPORT_SCRIPTING
#include "renderer/scripting/KReportScriptHandler.h"
#endif

#include <KPropertySet>

#include <QPalette>
#include <QDomNodeList>

KReportItemCheckBox::KReportItemCheckBox()
{
    createProperties();
}

KReportItemCheckBox::KReportItemCheckBox(const QDomNode &element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;

    nameProperty()->setValue(element.toElement().attribute(QLatin1String("report:name")));
    m_controlSource->setValue(element.toElement().attribute(QLatin1String("report:item-data-source")));
    setZ(element.toElement().attribute(QLatin1String("report:z-index")).toDouble());
    m_foregroundColor->setValue(QColor(element.toElement().attribute(QLatin1String("fo:foreground-color"))));
    m_checkStyle->setValue(element.toElement().attribute(QLatin1String("report:check-style")));
    m_staticValue->setValue(QVariant(element.toElement().attribute(QLatin1String("report:value"))).toBool());

    parseReportRect(element.toElement());

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == QLatin1String("report:line-style")) {
            KReportLineStyle ls;
            if (parseReportLineStyleData(node.toElement(), &ls)) {
                m_lineWeight->setValue(ls.width());
                m_lineColor->setValue(ls.color());
                m_lineStyle->setValue(QPen(ls.penStyle()));
            }
        } else {
            kreportpluginWarning() << "while parsing check element encountered unknow element: " << n;
        }
    }

}

KReportItemCheckBox::~KReportItemCheckBox()
{
}

void KReportItemCheckBox::createProperties()
{
    QStringList keys, strings;

    keys << QLatin1String("Cross") << QLatin1String("Tick") << QLatin1String("Dot");
    strings << tr("Cross") << tr("Tick") << tr("Dot");
    m_checkStyle = new KProperty("check-style", keys, strings, QLatin1String("Cross"), tr("Style"));

    m_controlSource = new KProperty("item-data-source", QStringList(), QStringList(), QString(), tr("Data Source"));
    m_controlSource->setOption("extraValueAllowed", QLatin1String("true"));

    m_foregroundColor = new KProperty("foreground-color", QColor(Qt::black), tr("Foreground Color"));

    m_lineWeight = new KProperty("line-weight", 1, tr("Line Weight"));
    m_lineColor = new KProperty("line-color", QColor(Qt::black), tr("Line Color"));
    m_lineStyle = new KProperty("line-style", QPen(Qt::SolidLine), tr("Line Style"), tr("Line Style"), KProperty::LineStyle);
    m_staticValue = new KProperty("value", QVariant(false), tr("Value"), tr("Value used if not bound to a field"));

    propertySet()->addProperty(m_controlSource);
    propertySet()->addProperty(m_staticValue);
    propertySet()->addProperty(m_checkStyle);
    propertySet()->addProperty(m_foregroundColor);
    propertySet()->addProperty(m_lineWeight);
    propertySet()->addProperty(m_lineColor);
    propertySet()->addProperty(m_lineStyle);
}

KReportLineStyle KReportItemCheckBox::lineStyle()
{
    KReportLineStyle ls;
    ls.setWidth(m_lineWeight->value().toInt());
    ls.setColor(m_lineColor->value().value<QColor>());
    ls.setPenStyle((Qt::PenStyle)m_lineStyle->value().toInt());
    return ls;
}

QString KReportItemCheckBox::itemDataSource() const
{
    return m_controlSource->value().toString();
}

// RTTI
QString KReportItemCheckBox::typeName() const
{
    return QLatin1String("check");
}

int KReportItemCheckBox::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                        const QVariant &data, KReportScriptHandler *script)
{
    OROCheck *chk = new OROCheck();

    chk->setPosition(scenePosition(position()) + offset);
    chk->setSize(sceneSize(size()));

    chk->setLineStyle(lineStyle());
    chk->setForegroundColor(m_foregroundColor->value().value<QColor>());
    chk->setCheckType(m_checkStyle->value().toString());

    QString str;
    bool v = false;
    QString cs = itemDataSource();

    //kreportpluginDebug() << "ControlSource:" << cs;
    if (!cs.isEmpty()) {
#ifdef KREPORT_SCRIPTING
        if (cs.left(1) == QLatin1String("=") && script) {
            str = script->evaluate(cs.mid(1)).toString();
        } else
#else
        Q_UNUSED(script);
#endif
        {
            str = data.toString();
        }

        str = str.toLower();

        //kreportpluginDebug() << "Check Value:" << str;
        if (str == QLatin1String("t") || str == QLatin1String("y") || str == QLatin1String("true") || str == QLatin1String("1"))
            v = true;

    } else {
        v = value();
    }

    chk->setValue(v);

    if (page) {
        page->addPrimitive(chk);
    }

    if (section) {
        OROCheck *chk2 = dynamic_cast<OROCheck*>(chk->clone());
        chk2->setPosition(scenePosition(position()));
        section->addPrimitive(chk2);
    }

    if (!page) {
        delete chk;
    }

    return 0; //Item doesn't stretch the section height
}

bool KReportItemCheckBox::value() const
{
    return m_staticValue->value().toBool();
}

void KReportItemCheckBox::setValue(bool v)
{
    m_staticValue->setValue(v);
}
