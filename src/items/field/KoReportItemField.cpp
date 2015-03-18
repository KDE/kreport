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
#include "KoReportItemField.h"

#include "common/renderobjects.h"
#include "renderer/scripting/krscripthandler.h"

#include <kproperty/Set.h>
#include <QPalette>
#include <QFontMetrics>
#include <QApplication>

KoReportItemField::KoReportItemField()
{
    createProperties();
}

KoReportItemField::KoReportItemField(QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;

    m_name->setValue(element.toElement().attribute("report:name"));
    m_controlSource->setValue(element.toElement().attribute("report:item-data-source"));
    m_itemValue->setValue(element.toElement().attribute("report:value"));
    Z = element.toElement().attribute("report:z-index").toDouble();
    m_horizontalAlignment->setValue(element.toElement().attribute("report:horizontal-align"));
    m_verticalAlignment->setValue(element.toElement().attribute("report:vertical-align"));

    m_canGrow->setValue(element.toElement().attribute("report:can-grow"));
    m_wordWrap->setValue(element.toElement().attribute("report:word-wrap"));

    parseReportRect(element.toElement(), &m_pos, &m_size);

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == "report:text-style") {
            KRTextStyleData ts;
            if (parseReportTextStyleData(node.toElement(), ts)) {
                m_backgroundColor->setValue(ts.backgroundColor);
                m_foregroundColor->setValue(ts.foregroundColor);
                m_backgroundOpacity->setValue(ts.backgroundOpacity);
                m_font->setValue(ts.font);

            }
        } else if (n == "report:line-style") {
            KRLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), ls)) {
                m_lineWeight->setValue(ls.weight);
                m_lineColor->setValue(ls.lineColor);
                m_lineStyle->setValue(QPen(ls.style));
            }
        } else {
            qWarning() << "while parsing field element encountered unknow element: " << n;
        }
    }
}

KoReportItemField::~KoReportItemField()
{
    delete m_set;
}

void KoReportItemField::createProperties()
{
    m_set = new KoProperty::Set(0, "Field");

    QStringList keys, strings;

    m_controlSource = new KoProperty::Property("item-data-source", QStringList(), QStringList(), QString(), tr("Data Source"));
    m_controlSource->setOption("extraValueAllowed", "true");

    m_itemValue = new KoProperty::Property("value", QString(), tr("Value"), tr("Value used if not bound to a field"));

    keys << "left" << "center" << "right";
    strings << tr("Left") << tr("Center") << tr("Right");
    m_horizontalAlignment = new KoProperty::Property("horizontal-align", keys, strings, "left", tr("Horizontal Alignment"));

    keys.clear();
    strings.clear();
    keys << "top" << "center" << "bottom";
    strings << tr("Top") << tr("Center") << tr("Bottom");
    m_verticalAlignment = new KoProperty::Property("vertical-align", keys, strings, "center", tr("Vertical Alignment"));

    m_font = new KoProperty::Property("Font", QApplication::font(), "Font", tr("Font"));


    m_backgroundColor = new KoProperty::Property("background-color", QColor(Qt::white), tr("Background Color"));
    m_foregroundColor = new KoProperty::Property("foreground-color", QPalette().color(QPalette::Foreground), tr("Foreground Color"));

    m_backgroundOpacity = new KoProperty::Property("background-opacity", QVariant(0), tr("Background Opacity"));
    m_backgroundOpacity->setOption("max", 100);
    m_backgroundOpacity->setOption("min", 0);
    m_backgroundOpacity->setOption("unit", "%");

    m_lineWeight = new KoProperty::Property("line-weight", 1, tr("Line Weight"));
    m_lineColor = new KoProperty::Property("line-color", QColor(Qt::black), tr("Line Color"));
    m_lineStyle = new KoProperty::Property("line-style", QPen(Qt::NoPen), tr("Line Style"), tr("Line Style"), KoProperty::LineStyle);

    m_wordWrap = new KoProperty::Property("word-wrap", QVariant(false), tr("Word Wrap"));
    m_canGrow = new KoProperty::Property("can-grow", QVariant(false), tr("Can Grow"));

#if 0 //Field Totals
    //TODO I do not think we need these
    m_trackTotal = new KoProperty::Property("TrackTotal", QVariant(false), futureI18n("Track Total"));
    m_trackBuiltinFormat = new KoProperty::Property("TrackBuiltinFormat", QVariant(false), futureI18n("Track Builtin Format"));
    _useSubTotal = new KoProperty::Property("UseSubTotal", QVariant(false), futureI18n("Use Sub Total"_);
    _trackTotalFormat = new KoProperty::Property("TrackTotalFormat", QString(), futureI18n("Track Total Format"));
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
    if (t == "center")
        flags = Qt::AlignHCenter;
    else if (t == "right")
        flags = Qt::AlignRight;
    else
        flags = Qt::AlignLeft;

    t = m_verticalAlignment->value().toString();
    if (t == "center")
        flags |= Qt::AlignVCenter;
    else if (t == "bottom")
        flags |= Qt::AlignBottom;
    else
        flags |= Qt::AlignTop;

    if (m_wordWrap->value().toBool() == true) {
        flags |= Qt::TextWordWrap;
    }
    return flags;
}

KRTextStyleData KoReportItemField::textStyle()
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
    //kDebug() << "Field: " << entityName() << "is" << itemDataSource();
}

KRLineStyleData KoReportItemField::lineStyle()
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
    return "field";
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
        if (ids.left(1) == "=" && script) { //Everything after = is treated as code
            if (!ids.contains("PageTotal()")) {
                QVariant v = script->evaluate(ids.mid(1));
                str = v.toString();
            } else {
                str = ids.mid(1);
                tb->setRequiresPostProcessing();
            }
        } else if (ids.left(1) == "$") { //Everything past $ is treated as a string
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


