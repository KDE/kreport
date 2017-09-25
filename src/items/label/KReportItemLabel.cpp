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
#include <KPropertyListData>

#include <QFontDatabase>
#include <QPalette>
#include <QDomNodeList>

KReportItemLabel::KReportItemLabel()
{
    createProperties();
}

KReportItemLabel::KReportItemLabel(const QDomNode & element)
    : KReportItemLabel()
{
    nameProperty()->setValue(element.toElement().attribute(QLatin1String("report:name")));
    text->setValue(element.toElement().attribute(QLatin1String("report:caption")));
    setZ(element.toElement().attribute(QLatin1String("report:z-index")).toDouble());
    horizontalAlignment->setValue(element.toElement().attribute(QLatin1String("report:horizontal-align")));
    verticalAlignment->setValue(element.toElement().attribute(QLatin1String("report:vertical-align")));

    parseReportRect(element.toElement());

    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;
    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == QLatin1String("report:text-style")) {
            KReportTextStyleData ts;
            if (parseReportTextStyleData(node.toElement(), &ts)) {
                backgroundColor->setValue(ts.backgroundColor);
                foregroundColor->setValue(ts.foregroundColor);
                backgroundOpacity->setValue(ts.backgroundOpacity);
                font->setValue(ts.font);

            }
        } else if (n == QLatin1String("report:line-style")) {
            KReportLineStyle ls;
            if (parseReportLineStyleData(node.toElement(), &ls)) {
                lineWeight->setValue(ls.weight());
                lineColor->setValue(ls.color());
                lineStyle->setValue(static_cast<int>(ls.penStyle()));
            }
        } else {
            kreportpluginWarning() << "while parsing label element encountered unknown element: " << n;
        }
    }
}

KReportItemLabel::~KReportItemLabel()
{
}

QString KReportItemLabel::textValue() const
{
    return text->value().toString();
}

void KReportItemLabel::setText(const QString& t)
{
    text->setValue(t);
}

void KReportItemLabel::createProperties()
{
    text = new KProperty("caption", QLatin1String("Label"), tr("Caption"));
    KPropertyListData *listData = new KPropertyListData(
        { QLatin1String("left"), QLatin1String("center"), QLatin1String("right") },
        QVariantList{ tr("Left"), tr("Center"), tr("Right") });
    horizontalAlignment = new KProperty("horizontal-align", listData, QLatin1String("left"),
                                          tr("Horizontal Alignment"));

    listData = new KPropertyListData(
        { QLatin1String("top"), QLatin1String("center"), QLatin1String("bottom") },
        QVariantList{ tr("Top"), tr("Center"), tr("Bottom") });
    verticalAlignment = new KProperty("vertical-align", listData, QLatin1String("center"),
                                        tr("Vertical Alignment"));

    font = new KProperty("font", QFontDatabase::systemFont(QFontDatabase::GeneralFont), tr("Font"), tr("Font"));

    backgroundColor = new KProperty("background-color", QColor(Qt::white), tr("Background Color"));
    foregroundColor = new KProperty("foreground-color", QColor(Qt::black), tr("Foreground Color"));

    backgroundOpacity = new KProperty("background-opacity", QVariant(0), tr("Background Opacity"));
    backgroundOpacity->setOption("max", 100);
    backgroundOpacity->setOption("min", 0);
    backgroundOpacity->setOption("unit", QLatin1String("%"));

    lineWeight = new KProperty("line-weight", 1.0, tr("Line Weight"));
    lineWeight->setOption("step", 1.0);
    lineColor = new KProperty("line-color", QColor(Qt::black), tr("Line Color"));
    lineStyle = new KProperty("line-style", static_cast<int>(Qt::NoPen), tr("Line Style"), QString(), KProperty::LineStyle);

    propertySet()->addProperty(text);
    propertySet()->addProperty(horizontalAlignment);
    propertySet()->addProperty(verticalAlignment);
    propertySet()->addProperty(font);
    propertySet()->addProperty(backgroundColor);
    propertySet()->addProperty(foregroundColor);
    propertySet()->addProperty(backgroundOpacity);
    propertySet()->addProperty(lineWeight);
    propertySet()->addProperty(lineColor);
    propertySet()->addProperty(lineStyle);
}

Qt::Alignment KReportItemLabel::textFlags() const
{
    Qt::Alignment align;
    QString t;
    t = horizontalAlignment->value().toString();
    if (t == QLatin1String("center"))
        align = Qt::AlignHCenter;
    else if (t == QLatin1String("right"))
        align = Qt::AlignRight;
    else
        align = Qt::AlignLeft;

    t = verticalAlignment->value().toString();
    if (t == QLatin1String("center"))
        align |= Qt::AlignVCenter;
    else if (t == QLatin1String("bottom"))
        align |= Qt::AlignBottom;
    else
        align |= Qt::AlignTop;

    return align;
}

KReportTextStyleData KReportItemLabel::textStyleValue() const
{
    KReportTextStyleData d;
    d.backgroundColor = backgroundColor->value().value<QColor>();
    d.foregroundColor = foregroundColor->value().value<QColor>();
    d.font = font->value().value<QFont>();
    d.backgroundOpacity = backgroundOpacity->value().toInt();
    return d;
}

KReportLineStyle KReportItemLabel::lineStyleValue() const
{
    KReportLineStyle ls;
    ls.setWeight(lineWeight->value().toReal());
    ls.setColor(lineColor->value().value<QColor>());
    ls.setPenStyle((Qt::PenStyle)lineStyle->value().toInt());
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
    tb->setFont(fontValue());
    tb->setText(textValue());
    tb->setFlags(textFlags());
    tb->setTextStyle(textStyleValue());
    tb->setLineStyle(lineStyleValue());

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

