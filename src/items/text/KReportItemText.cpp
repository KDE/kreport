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

#include "KReportItemText.h"
#include "KReportRenderObjects.h"
#include "kreportplugin_debug.h"

#include <KPropertySet>

#include <QPrinter>
#include <QApplication>
#include <QPalette>
#include <QFontMetrics>
#include <QDomNodeList>

KReportItemText::KReportItemText()
    : m_bottomPadding(0.0)
{
    createProperties();
}

KReportItemText::KReportItemText(const QDomNode & element)
  : KReportItemText()
{
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;

    m_name->setValue(element.toElement().attribute(QLatin1String("report:name")));
    m_controlSource->setValue(element.toElement().attribute(QLatin1String("report:item-data-source")));
    m_itemValue->setValue(element.toElement().attribute(QLatin1String("report:value")));
    Z = element.toElement().attribute(QLatin1String("report:z-index")).toDouble();
    m_horizontalAlignment->setValue(element.toElement().attribute(QLatin1String("report:horizontal-align")));
    m_verticalAlignment->setValue(element.toElement().attribute(QLatin1String("report:vertical-align")));
    m_bottomPadding = element.toElement().attribute(QLatin1String("report:bottom-padding")).toDouble();

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
            KReportLineStyle ls;
            if (parseReportLineStyleData(node.toElement(), &ls)) {
                m_lineWeight->setValue(ls.width());
                m_lineColor->setValue(ls.color());
                m_lineStyle->setValue(static_cast<int>(ls.penStyle()));
            }
        } else {
            kreportpluginWarning() << "while parsing field element encountered unknown element: " << n;
        }
    }

}

KReportItemText::~KReportItemText()
{
    delete m_set;
}

Qt::Alignment KReportItemText::textFlags() const
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

void KReportItemText::createProperties()
{
    m_set = new KPropertySet;

    //connect ( set, SIGNAL ( propertyChanged ( KPropertySet &, KProperty & ) ), this, SLOT ( propertyChanged ( KPropertySet &, KProperty & ) ) );

    QStringList keys, strings;

    //_query = new KProperty ( "Query", QStringList(), QStringList(), "Data Source", "Query" );
    m_controlSource = new KProperty("item-data-source", QStringList(), QStringList(), QString(), tr("Data Source"));

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
    m_foregroundColor = new KProperty("foreground-color", QColor(Qt::black), tr("Foreground Color"));

    m_lineWeight = new KProperty("line-weight", 1.0, tr("Line Weight"));
    m_lineWeight->setOption("step", 1.0);
    m_lineColor = new KProperty("line-color", QColor(Qt::black), tr("Line Color"));
    m_lineStyle = new KProperty("line-style", static_cast<int>(Qt::NoPen), tr("Line Style"), tr("Line Style"), KProperty::LineStyle);
    m_backgroundOpacity = new KProperty("background-opacity", QVariant(0), tr("Background Opacity"));
    m_backgroundOpacity->setOption("max", 100);
    m_backgroundOpacity->setOption("min", 0);
    m_backgroundOpacity->setOption("unit", QLatin1String("%"));

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

}

QString KReportItemText::itemDataSource() const
{
    return m_controlSource->value().toString();
}

qreal KReportItemText::bottomPadding() const
{
    return m_bottomPadding;
}

void KReportItemText::setBottomPadding(qreal bp)
{
    if (m_bottomPadding != bp) {
        m_bottomPadding = bp;
    }
}

KRTextStyleData KReportItemText::textStyle() const
{
    KRTextStyleData d;
    d.backgroundColor = m_backgroundColor->value().value<QColor>();
    d.foregroundColor = m_foregroundColor->value().value<QColor>();
    d.font = m_font->value().value<QFont>();
    d.backgroundOpacity = m_backgroundOpacity->value().toInt();
    return d;
}

KReportLineStyle KReportItemText::lineStyle() const
{
    KReportLineStyle ls;
    ls.setWidth(m_lineWeight->value().toReal());
    ls.setColor(m_lineColor->value().value<QColor>());
    ls.setPenStyle((Qt::PenStyle)m_lineStyle->value().toInt());
    return ls;
}

// RTTI
QString KReportItemText::typeName() const
{
    return QLatin1String("text");
}

int KReportItemText::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                       const QVariant &data, KReportScriptHandler *script)

{
    Q_UNUSED(script);

    QString qstrValue;

    QString cs = itemDataSource();

    if (!cs.isEmpty()) {
        if (cs.left(1) == QLatin1String("$")) { //Everything past $ is treated as a string
            qstrValue = cs.mid(1);
        } else {
            qstrValue = data.toString();
        }
    } else {
        qstrValue = m_itemValue->value().toString();
    }

    QPointF pos = m_pos.toScene();
    QSizeF size = m_size.toScene();
    pos += offset;

    QRectF trf(pos, size);
    qreal intStretch = trf.top() - offset.y();

    if (qstrValue.length()) {
        QRectF rect = trf;

        int pos = 0;
        QChar separator;
        QRegExp re(QLatin1String("\\s"));
        QPrinter prnt(QPrinter::HighResolution);
        QFontMetrics fm(font(), &prnt);

        // int   intRectWidth    = (int)(trf.width() * prnt.resolution()) - 10;
        int     intRectWidth    = (int)((m_size.toPoint().width() / 72) * prnt.resolution());
        int     intLineCounter  = 0;
        qreal   intBaseTop      = trf.top();
        qreal   intRectHeight   = trf.height();

        while (qstrValue.length()) {
            int idx = re.indexIn(qstrValue, pos);
            if (idx == -1) {
                idx = qstrValue.length();
                separator = QLatin1Char('\n');
            } else
                separator = qstrValue.at(idx);

            if (fm.boundingRect(qstrValue.left(idx)).width() < intRectWidth || pos == 0) {
                pos = idx + 1;
                if (separator == QLatin1Char('\n')) {
                    QString line = qstrValue.left(idx);

                    qstrValue.remove(0, idx + 1);

                    pos = 0;

                    rect.setTop(intBaseTop + (intLineCounter * intRectHeight));
                    rect.setBottom(rect.top() + intRectHeight);

                    OROTextBox * tb = new OROTextBox();
                    tb->setPosition(rect.topLeft());
                    tb->setSize(rect.size());
                    tb->setFont(font());
                    tb->setText(line);
                    tb->setFlags(textFlags());
                    tb->setTextStyle(textStyle());
                    tb->setLineStyle(lineStyle());

                    if (page) {
                        page->addPrimitive(tb);
                    }

                    if (section) {
                        OROTextBox *tb2 = dynamic_cast<OROTextBox*>(tb->clone());
                        tb2->setPosition(m_pos.toPoint());
                        section->addPrimitive(tb2);
                    }

                    if (!page) {
                        delete tb;
                    }

                    intStretch += intRectHeight;
                    intLineCounter++;
                }
            } else {
                QString line = qstrValue.left(pos - 1);
                qstrValue.remove(0, pos);
                pos = 0;

                rect.setTop(intBaseTop + (intLineCounter * intRectHeight));
                rect.setBottom(rect.top() + intRectHeight);

                OROTextBox * tb = new OROTextBox();
                tb->setPosition(rect.topLeft());
                tb->setSize(rect.size());
                tb->setFont(font());
                tb->setText(line);
                tb->setFlags(textFlags());
                tb->setTextStyle(textStyle());
                tb->setLineStyle(lineStyle());
                if (page) {
                    page->addPrimitive(tb);
                } else {
                    delete tb;
                }

                intStretch += intRectHeight;
                intLineCounter++;
            }
        }

        intStretch += (m_bottomPadding / 100.0);
    }

    return intStretch; //Item returns its required section height
}
