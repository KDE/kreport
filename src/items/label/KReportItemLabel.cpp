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

#include "KReportItemLabel.h"
#include "KReportRenderObjects.h"
#include "kreportplugin_debug.h"

#include <KPropertySet>

#include <QFontDatabase>
#include <QPalette>
#include <QDomNodeList>

KReportItemLabel::KReportItemLabel()
{
    createProperties();
}

KReportItemLabel::KReportItemLabel(const QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;

    nameProperty()->setValue(element.toElement().attribute(QLatin1String("report:name")));
    m_text->setValue(element.toElement().attribute(QLatin1String("report:caption")));
    setZ(element.toElement().attribute(QLatin1String("report:z-index")).toDouble());
    m_horizontalAlignment->setValue(element.toElement().attribute(QLatin1String("report:horizontal-align")));
    m_verticalAlignment->setValue(element.toElement().attribute(QLatin1String("report:vertical-align")));

    parseReportRect(element.toElement());

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == QLatin1String("report:text-style")) {
            KRTextStyleData ts;
            if (parseReportTextStyleData(node.toElement(), &ts)) {
                m_backgroundColor->setValue(ts.backgroundColor);
                m_foregroundColor->setValue(ts.foregroundColor);
                m_backgroundOpacity->setValue(ts.backgroundOpacity);
                m_font->setValue(ts.font);

            }
        } else if (n == QLatin1String("report:line-style")) {
            KReportLineStyle ls;
            if (parseReportLineStyleData(node.toElement(), &ls)) {
                m_lineWeight->setValue(ls.width());
                m_lineColor->setValue(ls.color());
                m_lineStyle->setValue(QPen(ls.penStyle()));
            }
        } else {
            kreportpluginWarning() << "while parsing label element encountered unknow element: " << n;
        }
    }
}

KReportItemLabel::~KReportItemLabel()
{
}

QString KReportItemLabel::text() const
{
    return m_text->value().toString();
}

void KReportItemLabel::setText(const QString& t)
{
    m_text->setValue(t);
}

void KReportItemLabel::createProperties()
{
    m_text = new KProperty("caption", QLatin1String("Label"), tr("Caption"));
    QStringList keys, strings;

    keys << QLatin1String("left") << QLatin1String("center") << QLatin1String("right");
    strings << tr("Left") << tr("Center") << tr("Right");
    m_horizontalAlignment = new KProperty("horizontal-align", keys, strings, QLatin1String("left"), tr("Horizontal Alignment"));

    keys.clear();
    strings.clear();
    keys << QLatin1String("top") << QLatin1String("center") << QLatin1String("bottom");
    strings << tr("Top") << tr("Center") << tr("Bottom");
    m_verticalAlignment = new KProperty("vertical-align", keys, strings, QLatin1String("center"), tr("Vertical Alignment"));

    m_font = new KProperty("font", QFontDatabase::systemFont(QFontDatabase::GeneralFont), tr("Font"), tr("Font"));

    m_backgroundColor = new KProperty("background-color", QColor(Qt::white), tr("Background Color"));
    m_foregroundColor = new KProperty("foreground-color", QColor(Qt::black), tr("Foreground Color"));

    m_backgroundOpacity = new KProperty("background-opacity", QVariant(0), tr("Background Opacity"));
    m_backgroundOpacity->setOption("max", 100);
    m_backgroundOpacity->setOption("min", 0);
    m_backgroundOpacity->setOption("unit", QLatin1String("%"));

    m_lineWeight = new KProperty("line-weight", 1.0, tr("Line Weight"));
    m_lineColor = new KProperty("line-color", QColor(Qt::black), tr("Line Color"));
    m_lineStyle = new KProperty("line-style", QPen(Qt::NoPen), tr("Line Style"), tr("Line Style"), KProperty::LineStyle);

    propertySet()->addProperty(m_text);
    propertySet()->addProperty(m_horizontalAlignment);
    propertySet()->addProperty(m_verticalAlignment);
    propertySet()->addProperty(m_font);
    propertySet()->addProperty(m_backgroundColor);
    propertySet()->addProperty(m_foregroundColor);
    propertySet()->addProperty(m_backgroundOpacity);
    propertySet()->addProperty(m_lineWeight);
    propertySet()->addProperty(m_lineColor);
    propertySet()->addProperty(m_lineStyle);
}

Qt::Alignment KReportItemLabel::textFlags() const
{
    Qt::Alignment align;
    QString t;
    t = m_horizontalAlignment->value().toString();
    if (t == QLatin1String("center"))
        align = Qt::AlignHCenter;
    else if (t == QLatin1String("right"))
        align = Qt::AlignRight;
    else
        align = Qt::AlignLeft;

    t = m_verticalAlignment->value().toString();
    if (t == QLatin1String("center"))
        align |= Qt::AlignVCenter;
    else if (t == QLatin1String("bottom"))
        align |= Qt::AlignBottom;
    else
        align |= Qt::AlignTop;

    return align;
}

KRTextStyleData KReportItemLabel::textStyle() const
{
    KRTextStyleData d;
    d.backgroundColor = m_backgroundColor->value().value<QColor>();
    d.foregroundColor = m_foregroundColor->value().value<QColor>();
    d.font = m_font->value().value<QFont>();
    d.backgroundOpacity = m_backgroundOpacity->value().toInt();
    return d;
}

KReportLineStyle KReportItemLabel::lineStyle() const
{
    KReportLineStyle ls;
    ls.setWidth(m_lineWeight->value().toInt());
    ls.setColor(m_lineColor->value().value<QColor>());
    ls.setPenStyle((Qt::PenStyle)m_lineStyle->value().toInt());
    return ls;
}

// RTTI
QString KReportItemLabel::typeName() const
{
    return QLatin1String("label");
}

int KReportItemLabel::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                        const QVariant &data, KReportScriptHandler *script)
{
    Q_UNUSED(data)
    Q_UNUSED(script)

    OROTextBox * tb = new OROTextBox();
    tb->setPosition(scenePosition(position()) + offset);
    tb->setSize(sceneSize(size()));
    tb->setFont(font());
    tb->setText(text());
    tb->setFlags(textFlags());
    tb->setTextStyle(textStyle());
    tb->setLineStyle(lineStyle());

    if (page) {
        page->insertPrimitive(tb);
    }

    if (section) {
        OROPrimitive *clone = tb->clone();
        clone->setPosition(scenePosition(position()));
        section->addPrimitive(clone);
    }

    if (!page) {
        delete tb;
    }

    return 0; //Item doesn't stretch the section height
}

