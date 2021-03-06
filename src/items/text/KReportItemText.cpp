/* This file is part of the KDE project
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2019 Jarosław Staniek <staniek@kde.org>
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
#include "KReportUtils.h"
#include "KReportUtils_p.h"
#include "kreportplugin_debug.h"

#include <KPropertyListData>
#include <KPropertySet>

#include <QPrinter>
#include <QApplication>
#include <QPalette>
#include <QFontMetrics>
#include <QDomNodeList>
#include <QRegularExpression>

KReportItemText::KReportItemText()
    : m_bottomPadding(0.0)
{
    createProperties();
}

KReportItemText::KReportItemText(const QDomNode & element)
  : KReportItemText()
{
    nameProperty()->setValue(KReportUtils::readNameAttribute(element.toElement()));
    setItemDataSource(element.toElement().attribute(QLatin1String("report:item-data-source")));
    m_itemValue->setValue(element.toElement().attribute(QLatin1String("report:value")));
    setZ(element.toElement().attribute(QLatin1String("report:z-index")).toDouble());
    m_horizontalAlignment->setValue(element.toElement().attribute(QLatin1String("report:horizontal-align")));
    m_verticalAlignment->setValue(element.toElement().attribute(QLatin1String("report:vertical-align")));
    m_bottomPadding = element.toElement().attribute(QLatin1String("report:bottom-padding")).toDouble();

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
                m_backgroundColor->setValue(ts.backgroundColor);
                m_foregroundColor->setValue(ts.foregroundColor);
                m_backgroundOpacity->setValue(ts.backgroundOpacity);
                m_font->setValue(ts.font);

            }
        } else if (n == QLatin1String("report:line-style")) {
            KReportLineStyle ls;
            if (parseReportLineStyleData(node.toElement(), &ls)) {
                m_lineWeight->setValue(ls.weight());
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
    createDataSourceProperty();

    m_itemValue = new KProperty("value", QString(), tr("Value"), tr("Value used if not bound to a field"));

    KPropertyListData *listData = new KPropertyListData(
        { QLatin1String("left"), QLatin1String("center"), QLatin1String("right") },
        QVariantList{ tr("Left"), tr("Center"), tr("Right") });
    m_horizontalAlignment = new KProperty("horizontal-align", listData, QLatin1String("left"),
                                          tr("Horizontal Alignment"));

    listData = new KPropertyListData(
        { QLatin1String("top"), QLatin1String("center"), QLatin1String("bottom") },
        QVariantList{ tr("Top"), tr("Center"), tr("Bottom") });
    m_verticalAlignment = new KProperty("vertical-align", listData, QLatin1String("center"),
                                        tr("Vertical Alignment"));

    m_font = new KProperty("font", QApplication::font(), tr("Font"));

    m_backgroundColor = new KProperty("background-color", QColor(Qt::white), tr("Background Color"));
    m_foregroundColor = new KProperty("foreground-color", QColor(Qt::black), tr("Foreground Color"));

    m_lineWeight = new KProperty("line-weight", 1.0, tr("Line Weight"));
    m_lineWeight->setOption("step", 1.0);
    m_lineColor = new KProperty("line-color", QColor(Qt::black), tr("Line Color"));
    m_lineStyle = new KProperty("line-style", static_cast<int>(Qt::NoPen), tr("Line Style"), QString(), KProperty::LineStyle);
    m_backgroundOpacity = new KProperty("background-opacity", QVariant(0), tr("Background Opacity"));
    m_backgroundOpacity->setOption("max", 100);
    m_backgroundOpacity->setOption("min", 0);
    m_backgroundOpacity->setOption("suffix", QLatin1String("%"));

    propertySet()->addProperty(m_itemValue);
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

KReportTextStyleData KReportItemText::textStyle() const
{
    KReportTextStyleData d;
    d.backgroundColor = m_backgroundColor->value().value<QColor>();
    d.foregroundColor = m_foregroundColor->value().value<QColor>();
    d.font = m_font->value().value<QFont>();
    d.backgroundOpacity = m_backgroundOpacity->value().toInt();
    return d;
}

KReportLineStyle KReportItemText::lineStyle() const
{
    KReportLineStyle ls;
    ls.setWeight(m_lineWeight->value().toReal());
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
        qstrValue = data.toString();
    } else {
        qstrValue = m_itemValue->value().toString();
    }

    QPointF pos = scenePosition(position());
    QSizeF siz = sceneSize(size());
    pos += offset;

    QRectF trf(pos, siz);
    qreal intStretch = trf.top() - offset.y();

    if (!qstrValue.isEmpty()) {
        QRectF rect = trf;

        int pos = 0;
        QChar separator;
        QRegularExpression re(QLatin1String("\\s"));
        const QFontMetricsF fm(font(), KReportPrivate::highResolutionPrinter());

        const int intRectWidth
            = (int)((size().width() / 72) * KReportPrivate::highResolutionPrinter()->resolution());
        int     intLineCounter  = 0;
        qreal   intBaseTop      = trf.top();
        qreal   intRectHeight   = trf.height();

        while (!qstrValue.isEmpty()) {
            QRegularExpressionMatch match = re.match(qstrValue);
            int idx = match.capturedStart(pos);
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
                        page->insertPrimitive(tb);
                    }

                    if (section) {
                        OROTextBox *tb2 = dynamic_cast<OROTextBox*>(tb->clone());
                        if (tb2) {
                            tb2->setPosition(scenePosition(position()));
                            section->addPrimitive(tb2);
                        }
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
                    page->insertPrimitive(tb);
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
