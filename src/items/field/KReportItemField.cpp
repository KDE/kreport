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

#include "KReportItemField.h"
#include "KReportRenderObjects.h"
#include "kreportplugin_debug.h"
#ifdef KREPORT_SCRIPTING
#include "renderer/scripting/KReportScriptHandler.h"
#endif

#include <KPropertyListData>
#include <KPropertySet>

#include <QPalette>
#include <QDomNodeList>
#include <QFontMetrics>
#include <QApplication>

KReportItemField::KReportItemField()
{
    createProperties();
}

KReportItemField::KReportItemField(const QDomNode & element)
    : KReportItemField()
{
    nameProperty()->setValue(element.toElement().attribute(QLatin1String("report:name")));
    controlSource->setValue(element.toElement().attribute(QLatin1String("report:item-data-source")));
    itemValue->setValue(element.toElement().attribute(QLatin1String("report:value")));
    setZ(element.toElement().attribute(QLatin1String("report:z-index")).toDouble());
    horizontalAlignment->setValue(element.toElement().attribute(QLatin1String("report:horizontal-align")));
    verticalAlignment->setValue(element.toElement().attribute(QLatin1String("report:vertical-align")));

    canGrow->setValue(element.toElement().attribute(QLatin1String("report:can-grow")));
    wordWrap->setValue(element.toElement().attribute(QLatin1String("report:word-wrap")));

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
            kreportpluginWarning() << "while parsing field element encountered unknown element: " << n;
        }
    }
}

KReportItemField::~KReportItemField()
{
}

void KReportItemField::createProperties()
{
    controlSource = new KProperty("item-data-source", new KPropertyListData, QVariant(), tr("Data Source"));
    controlSource->setOption("extraValueAllowed", QLatin1String("true"));

    itemValue = new KProperty("value", QString(), tr("Value"), tr("Value used if not bound to a field"));

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

    font = new KProperty("font", QApplication::font(), tr("Font"));

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

    wordWrap = new KProperty("word-wrap", QVariant(false), tr("Word Wrap"));
    canGrow = new KProperty("can-grow", QVariant(false), tr("Can Grow"));

    //! @todo I do not think we need these
#if 0 //Field Totals
    m_trackTotal = new KProperty("trackTotal", QVariant(false), futureI18n("Track Total"));
    m_trackBuiltinFormat = new KProperty("trackBuiltinFormat", QVariant(false), futureI18n("Track Builtin Format"));
    _useSubTotal = new KProperty("useSubTotal", QVariant(false), futureI18n("Use Sub Total"_));
    _trackTotalFormat = new KProperty("trackTotalFormat", QString(), futureI18n("Track Total Format"));
#endif

    propertySet()->addProperty(controlSource);
    propertySet()->addProperty(itemValue);
    propertySet()->addProperty(horizontalAlignment);
    propertySet()->addProperty(verticalAlignment);
    propertySet()->addProperty(font);
    propertySet()->addProperty(backgroundColor);
    propertySet()->addProperty(foregroundColor);
    propertySet()->addProperty(backgroundOpacity);
    propertySet()->addProperty(lineWeight);
    propertySet()->addProperty(lineColor);
    propertySet()->addProperty(lineStyle);
    propertySet()->addProperty(wordWrap);
    propertySet()->addProperty(canGrow);

    //_set->addProperty ( _trackTotal );
    //_set->addProperty ( _trackBuiltinFormat );
    //_set->addProperty ( _useSubTotal );
    //_set->addProperty ( _trackTotalFormat );
}

int KReportItemField::textFlags() const
{
    int flags;
    QString t;
    t = horizontalAlignment->value().toString();
    if (t == QLatin1String("center"))
        flags = Qt::AlignHCenter;
    else if (t == QLatin1String("right"))
        flags = Qt::AlignRight;
    else
        flags = Qt::AlignLeft;

    t = verticalAlignment->value().toString();
    if (t == QLatin1String("center"))
        flags |= Qt::AlignVCenter;
    else if (t == QLatin1String("bottom"))
        flags |= Qt::AlignBottom;
    else
        flags |= Qt::AlignTop;

    if (wordWrap->value().toBool() == true) {
        flags |= Qt::TextWordWrap;
    }
    return flags;
}

KReportTextStyleData KReportItemField::textStyleValue() const
{
    KReportTextStyleData d;
    d.backgroundColor = backgroundColor->value().value<QColor>();
    d.foregroundColor = foregroundColor->value().value<QColor>();
    d.font = font->value().value<QFont>();
    d.backgroundOpacity = backgroundOpacity->value().toInt();

    return d;
}

QString KReportItemField::itemDataSource() const
{
    return controlSource->value().toString();
}

void KReportItemField::setItemDataSource(const QString& t)
{
    if (controlSource->value() != t) {
        controlSource->setValue(t);
    }
    //kreportpluginDebug() << "Field: " << entityName() << "is" << itemDataSource();
}

KReportLineStyle KReportItemField::lineStyleValue() const
{
    KReportLineStyle ls;
    ls.setWeight(lineWeight->value().toReal());
    ls.setColor(lineColor->value().value<QColor>());
    ls.setPenStyle((Qt::PenStyle)lineStyle->value().toInt());
    return ls;
}
// RTTI
QString KReportItemField::typeName() const
{
    return QLatin1String("field");
}

int KReportItemField::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                        const QVariant &data, KReportScriptHandler *script)
{
    OROTextBox * tb = new OROTextBox();
    tb->setPosition(scenePosition(position()) + offset);
    tb->setSize(sceneSize(size()));
    tb->setFont(fontValue());
    tb->setFlags(textFlags());
    tb->setTextStyle(textStyleValue());
    tb->setLineStyle(lineStyleValue());
    tb->setCanGrow(canGrow->value().toBool());
    tb->setWordWrap(wordWrap->value().toBool());

    QString str;

    QString ids = itemDataSource();
    if (!ids.isEmpty()) {
#ifdef KREPORT_SCRIPTING
        if (ids.left(1) == QLatin1String("=") && script) { //Everything after = is treated as code
            if (!ids.contains(QLatin1String("PageTotal()"))) {
                QVariant v = script->evaluate(ids.mid(1));
                str = v.toString();
            } else {
                str = ids.mid(1);
                tb->setRequiresPostProcessing(true);
            }
        } else
#else
        Q_UNUSED(script);
#endif
        if (ids.left(1) == QLatin1String("$")) { //Everything past $ is treated as a string
            str = ids.mid(1);
        } else {
            str = data.toString();
        }
    } else {
            str = itemValue->value().toString();
    }

    tb->setText(str);

    //Work out the size of the text
    if (tb->canGrow()) {
        QRect r;
        QFontMetrics metrics(fontValue());
        if (tb->wordWrap()) {
            //Grow vertically
            QRect temp(tb->position().x(), tb->position().y(), tb->size().width(), 5000); // a large vertical height
            r = metrics.boundingRect(temp, tb->flags(), str);
        } else {
            //Grow Horizontally
            QRect temp(tb->position().x(), tb->position().y(), 5000, tb->size().height()); // a large vertical height
            r = metrics.boundingRect(temp, tb->flags(), str);
        }
        tb->setSize(r.size() + QSize(4,4));
    }

    if (page) {
        page->insertPrimitive(tb);
    }

    if (section) {
        OROPrimitive *clone = tb->clone();
        clone->setPosition(scenePosition(position()));
        section->addPrimitive(clone);
    }
    int height = scenePosition(position()).y() + tb->size().height();
    //If there is no page to add the item to, delete it now because it wont be deleted later
    if (!page) {
        delete tb;
    }
    return height;
}
