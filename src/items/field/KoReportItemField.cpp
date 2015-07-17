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

#include "KoReportItemField.h"
#include "common/renderobjects.h"
#include "kreportplugin_debug.h"
#ifdef KREPORT_SCRIPTING
#include "renderer/scripting/krscripthandler.h"
#endif

#include <KPropertySet>

#include <QPalette>
#include <QDomNodeList>
#include <QFontMetrics>
#include <QApplication>

KoReportItemField::KoReportItemField()
{
    createProperties();
}

KoReportItemField::KoReportItemField(const QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;

    m_name->setValue(element.toElement().attribute(QLatin1String("report:name")));
    m_controlSource->setValue(element.toElement().attribute(QLatin1String("report:item-data-source")));
    m_itemValue->setValue(element.toElement().attribute(QLatin1String("report:value")));
    Z = element.toElement().attribute(QLatin1String("report:z-index")).toDouble();
    m_horizontalAlignment->setValue(element.toElement().attribute(QLatin1String("report:horizontal-align")));
    m_verticalAlignment->setValue(element.toElement().attribute(QLatin1String("report:vertical-align")));

    m_canGrow->setValue(element.toElement().attribute(QLatin1String("report:can-grow")));
    m_wordWrap->setValue(element.toElement().attribute(QLatin1String("report:word-wrap")));

    parseReportRect(element.toElement(), &m_pos, &m_size);

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
            KRLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), &ls)) {
                m_lineWeight->setValue(ls.weight);
                m_lineColor->setValue(ls.lineColor);
                m_lineStyle->setValue(QPen(ls.style));
            }
        } else {
            kreportpluginWarning() << "while parsing field element encountered unknow element: " << n;
        }
    }
}

KoReportItemField::~KoReportItemField()
{
    delete m_set;
}

void KoReportItemField::createProperties()
{
    m_set = new KPropertySet(0, QLatin1String("Field"));

    QStringList keys, strings;

    m_controlSource = new KProperty("item-data-source", QStringList(), QStringList(), QString(), tr("Data Source"));
    m_controlSource->setOption("extraValueAllowed", QLatin1String("true"));

    m_itemValue = new KProperty("value", QString(), tr("Value"), tr("Value used if not bound to a field"));

    keys << QLatin1String("left") << QLatin1String("center") << QLatin1String("right");
    strings << tr("Left") << tr("Center") << tr("Right");
    m_horizontalAlignment = new KProperty("horizontal-align", keys, strings, QLatin1String("left"), tr("Horizontal Alignment"));

    keys.clear();
    strings.clear();
    keys << QLatin1String("top") << QLatin1String("center") << QLatin1String("bottom");
    strings << tr("Top") << tr("Center") << tr("Bottom");
    m_verticalAlignment = new KProperty("vertical-align", keys, strings, QLatin1String("center"), tr("Vertical Alignment"));

    m_font = new KProperty("font", QApplication::font(), tr("Font"));

    m_backgroundColor = new KProperty("background-color", QColor(Qt::white), tr("Background Color"));
    m_foregroundColor = new KProperty("foreground-color", QPalette().color(QPalette::Foreground), tr("Foreground Color"));

    m_backgroundOpacity = new KProperty("background-opacity", QVariant(0), tr("Background Opacity"));
    m_backgroundOpacity->setOption("max", 100);
    m_backgroundOpacity->setOption("min", 0);
    m_backgroundOpacity->setOption("unit", QLatin1String("%"));

    m_lineWeight = new KProperty("line-weight", 1, tr("Line Weight"));
    m_lineColor = new KProperty("line-color", QColor(Qt::black), tr("Line Color"));
    m_lineStyle = new KProperty("line-style", QPen(Qt::NoPen), tr("Line Style"), tr("Line Style"), KProperty::LineStyle);

    m_wordWrap = new KProperty("word-wrap", QVariant(false), tr("Word Wrap"));
    m_canGrow = new KProperty("can-grow", QVariant(false), tr("Can Grow"));

    //! @todo I do not think we need these
#if 0 //Field Totals
    m_trackTotal = new KProperty("trackTotal", QVariant(false), futureI18n("Track Total"));
    m_trackBuiltinFormat = new KProperty("trackBuiltinFormat", QVariant(false), futureI18n("Track Builtin Format"));
    _useSubTotal = new KProperty("useSubTotal", QVariant(false), futureI18n("Use Sub Total"_);
    _trackTotalFormat = new KProperty("trackTotalFormat", QString(), futureI18n("Track Total Format"));
#endif

    addDefaultProperties();
    m_set->addProperty(m_controlSource);
    m_set->addProperty(m_itemValue);
    m_set->addProperty(m_horizontalAlignment);
    m_set->addProperty(m_verticalAlignment);
    m_set->addProperty(m_font);
    m_set->addProperty(m_backgroundColor);
    m_set->addProperty(m_foregroundColor);
    m_set->addProperty(m_backgroundOpacity);
    m_set->addProperty(m_lineWeight);
    m_set->addProperty(m_lineColor);
    m_set->addProperty(m_lineStyle);
    m_set->addProperty(m_wordWrap);
    m_set->addProperty(m_canGrow);

    //_set->addProperty ( _trackTotal );
    //_set->addProperty ( _trackBuiltinFormat );
    //_set->addProperty ( _useSubTotal );
    //_set->addProperty ( _trackTotalFormat );
}

int KoReportItemField::textFlags() const
{
    int flags;
    QString t;
    t = m_horizontalAlignment->value().toString();
    if (t == QLatin1String("center"))
        flags = Qt::AlignHCenter;
    else if (t == QLatin1String("right"))
        flags = Qt::AlignRight;
    else
        flags = Qt::AlignLeft;

    t = m_verticalAlignment->value().toString();
    if (t == QLatin1String("center"))
        flags |= Qt::AlignVCenter;
    else if (t == QLatin1String("bottom"))
        flags |= Qt::AlignBottom;
    else
        flags |= Qt::AlignTop;

    if (m_wordWrap->value().toBool() == true) {
        flags |= Qt::TextWordWrap;
    }
    return flags;
}

KRTextStyleData KoReportItemField::textStyle() const
{
    KRTextStyleData d;
    d.backgroundColor = m_backgroundColor->value().value<QColor>();
    d.foregroundColor = m_foregroundColor->value().value<QColor>();
    d.font = m_font->value().value<QFont>();
    d.backgroundOpacity = m_backgroundOpacity->value().toInt();

    return d;
}

QString KoReportItemField::itemDataSource() const
{
    return m_controlSource->value().toString();
}

void KoReportItemField::setItemDataSource(const QString& t)
{
    if (m_controlSource->value() != t) {
        m_controlSource->setValue(t);
    }
    //kreportpluginDebug() << "Field: " << entityName() << "is" << itemDataSource();
}

KRLineStyleData KoReportItemField::lineStyle() const
{
    KRLineStyleData ls;
    ls.weight = m_lineWeight->value().toInt();
    ls.lineColor = m_lineColor->value().value<QColor>();
    ls.style = (Qt::PenStyle)m_lineStyle->value().toInt();
    return ls;
}
// RTTI
QString KoReportItemField::typeName() const
{
    return QLatin1String("field");
}

int KoReportItemField::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                        const QVariant &data, KRScriptHandler *script)
{
    OROTextBox * tb = new OROTextBox();
    tb->setPosition(m_pos.toScene() + offset);
    tb->setSize(m_size.toScene());
    tb->setFont(font());
    tb->setFlags(textFlags());
    tb->setTextStyle(textStyle());
    tb->setLineStyle(lineStyle());
    tb->setCanGrow(m_canGrow->value().toBool());
    tb->setWordWrap(m_wordWrap->value().toBool());

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
                tb->setRequiresPostProcessing();
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
            str = m_itemValue->value().toString();
    }

    tb->setText(str);

    //Work out the size of the text
    if (tb->canGrow()) {
        QRect r;
        if (tb->wordWrap()) {
            //Grow vertically
            QFontMetrics metrics(font());
            QRect temp(tb->position().x(), tb->position().y(), tb->size().width(), 5000); // a large vertical height
            r = metrics.boundingRect(temp, tb->flags(), str);
        } else {
            //Grow Horizontally
            QFontMetrics metrics(font());
            QRect temp(tb->position().x(), tb->position().y(), 5000, tb->size().height()); // a large vertical height
            r = metrics.boundingRect(temp, tb->flags(), str);
        }
        tb->setSize(r.size() + QSize(4,4));
    }

    if (page) {
        page->addPrimitive(tb);
    }

    if (section) {
        OROPrimitive *clone = tb->clone();
        clone->setPosition(m_pos.toScene());
        section->addPrimitive(clone);
    }
    int height = m_pos.toScene().y() + tb->size().height();
    //If there is no page to add the item to, delete it now because it wont be deleted later
    if (!page) {
        delete tb;
    }
    return height;
}
