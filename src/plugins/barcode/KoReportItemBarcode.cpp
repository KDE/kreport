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

#include "KoReportItemBarcode.h"

#include <KProperty>
#include <KPropertySet>

#include <QDomNode>

#include "kreportplugin_debug.h"

#include "barcodes.h"

KoReportItemBarcode::KoReportItemBarcode()
{
    createProperties();
}

KoReportItemBarcode::KoReportItemBarcode(const QDomNode & element)
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
    m_maxLength->setValue(element.toElement().attribute(QLatin1String("report:barcode-max-length")));
    m_format->setValue(element.toElement().attribute(QLatin1String("report:barcode-format")));
    parseReportRect(element.toElement(), &m_pos, &m_size);

}

void KoReportItemBarcode::setMaxLength(int i)
{
    if (i > 0) {
        if (m_maxLength->value().toInt() != i) {
            m_maxLength->setValue(i);
        }
        if (m_format->value().toString() == QLatin1String("3of9")) {
            int C = i; // number of characters
            int N = 2; // narrow mult for wide line
            int X = 1; // narrow line width
            int I = 1; // interchange line width
            m_minWidthData = (((C + 2) * ((3 * N) + 6) * X) + ((C + 1) * I)) / 100.0;
            m_minHeight = m_minWidthData * 0.15;
            /*if(min_height < 0.25)*/ m_minHeight = 0.25;
            m_minWidthTotal = m_minWidthData + 0.22; // added a little buffer to make sure we don't loose any
            // of our required quiet zone in conversions
        } else if (m_format->value().toString() == QLatin1String("3of9+")) {
            int C = i * 2; // number of characters
            int N = 2; // narrow mult for wide line
            int X = 1; // 1px narrow line
            int I = 1; // 1px narrow line interchange
            m_minWidthData = (((C + 2) * ((3 * N) + 6) * X) + ((C + 1) * I)) / 100.0;
            m_minHeight = m_minWidthData * 0.15;
            /*if(min_height < 0.25)*/ m_minHeight = 0.25;
            m_minWidthTotal = m_minWidthData + 0.22; // added a little buffer to make sure we don't loose any
            // of our required quiet zone in conversions
        } else if (m_format->value().toString() == QLatin1String("i2of5")) {
            int C = i * 2; // number of characters
            int N = 2; // narrow mult for wide line
            int X = 1; // 1px narrow line
            m_minWidthTotal = ((C * (2.0*N + 3.0) + 6.0 + N) * X);
            m_minHeight = 0.25;
            m_minWidthTotal = m_minWidthData + 0.22;
        } else if (m_format->value().toString() == QLatin1String("128")) {
            int C = i; // assuming 1:1 ratio of data passed in to data actually used in encoding
            int X = 1; // 1px wide
            m_minWidthData = (((11 * C) + 35) * X) / 100.0;       // assuming CODE A or CODE B
            m_minHeight = m_minWidthData * 0.15;
            /*if(min_height < 0.25)*/ m_minHeight = 0.25;
            m_minWidthTotal = m_minWidthData + 0.22; // added a little bugger to make sure we don't loose any
            // of our required quiet zone in conversions
        } else if (m_format->value().toString() == QLatin1String("upc-a")) {
            m_minWidthData = 0.95;
            m_minWidthTotal = 1.15;
            m_minHeight = 0.25;
        } else if (m_format->value().toString() == QLatin1String("upc-e")) {
            m_minWidthData = 0.52;
            m_minWidthTotal = 0.70;
            m_minHeight = 0.25;
        } else if (m_format->value().toString() == QLatin1String("ean13")) {
            m_minWidthData = 0.95;
            m_minWidthTotal = 1.15;
            m_minHeight = 0.25;
        } else if (m_format->value().toString() == QLatin1String("ean8")) {
            m_minWidthData = 0.67;
            m_minWidthTotal = 0.90;
            m_minHeight = 0.25;
        } else {
            kreportpluginDebug() << "Unknown format encountered: " << m_format->value().toString();
        }
    }
}

void KoReportItemBarcode::createProperties()
{
    m_set = new KPropertySet(0, QLatin1String("Barcode"));

    QStringList keys, strings;

    m_controlSource = new KProperty("item-data-source", QStringList(), QStringList(),
                                    QString(), tr("Data Source"));

    m_itemValue = new KProperty("value", QString(), tr("Value"),
                                tr("Value used if not bound to a field"));

    keys << QLatin1String("left") << QLatin1String("center") << QLatin1String("right");
    strings << tr("Left") << tr("Center") << tr("Right");
    m_horizontalAlignment = new KProperty("horizontal-align", keys, strings,
                                          QLatin1String("left"), tr("Horizontal Alignment"));

    keys.clear();
    strings.clear();
    keys << QLatin1String("3of9") << QLatin1String("3of9+") << QLatin1String("128")
         << QLatin1String("ean8") << QLatin1String("ean13") << QLatin1String("i2of5")
         << QLatin1String("upc-a") << QLatin1String("upc-e");
    strings << tr("Code 3 of 9", "Barcode symbology, keep short")
            << tr("Code 3 of 9 Ext.", "Barcode symbology, keep short")
            << tr("Code 128", "Barcode symbology, keep short")
            << tr("EAN-8", "Barcode symbology, keep short")
            << tr("EAN-13", "Barcode symbology, keep short")
            << tr("Interleaved 2 of 5", "Barcode symbology, keep short")
            << tr("UPC-A", "Barcode symbology, keep short")
            << tr("UPC-E", "Barcode symbology, keep short");
    m_format = new KProperty("barcode-format", keys, strings, QLatin1String("3of9"),
                             tr("Barcode Format"));

    m_maxLength = new KProperty("barcode-max-length", 5, tr("Max Length"),
                                tr("Maximum Barcode Length"));

    addDefaultProperties();
    m_set->addProperty(m_controlSource);
    m_set->addProperty(m_itemValue);
    m_set->addProperty(m_format);
    m_set->addProperty(m_horizontalAlignment);
    m_set->addProperty(m_maxLength);
}

KoReportItemBarcode::~KoReportItemBarcode()
{
    delete m_set;
}

int KoReportItemBarcode::alignment()
{
    QByteArray a = m_horizontalAlignment->value().toByteArray();

    if (a == "left")
        return 0;
    else if (a == "center")
        return 1;
    else if (a == "right")
        return 2;
    else
        return 0;
}

QString KoReportItemBarcode::itemDataSource() const
{
    return m_controlSource->value().toString();
}

QString KoReportItemBarcode::format()
{
    return m_format->value().toString();
}

int KoReportItemBarcode::maxLength()
{
    return m_maxLength->value().toInt();
}

void KoReportItemBarcode::setFormat(const QString& f)
{
    m_format->setValue(f);
}

void KoReportItemBarcode::setAlignment(int)
{
    //! @todo Barcode alignment
}

//RTTI
QString KoReportItemBarcode::typeName() const
{
    return QLatin1String("barcode");
}

int KoReportItemBarcode::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                          const QVariant &data, KRScriptHandler *script)
{
    Q_UNUSED(section);
    Q_UNUSED(script);

    QPointF pos = m_pos.toScene();
    QSizeF size = m_size.toScene();
    pos += offset;

    QRectF rect = QRectF(pos, size);

    QString val;

    if (m_controlSource->value().toString().isEmpty()) {
        val = m_itemValue->value().toString();
    } else {
        val = data.toString();
    }

    if (page) {
        QByteArray fmt = m_format->value().toByteArray();
        int align = alignment();
        if (fmt == "3of9")
            render3of9(page, rect, val, align);
        else if (fmt == "3of9+")
            renderExtended3of9(page, rect, val, align);
        else if (fmt == "i2of5")
            renderI2of5(page, rect, val, align);
        else if (fmt == "128")
            renderCode128(page, rect, val, align);
        else if (fmt == "ean13")
            renderCodeEAN13(page, rect, val, align);
        else if (fmt == "ean8")
            renderCodeEAN8(page, rect, val, align);
        else if (fmt == "upc-a")
            renderCodeUPCA(page, rect, val, align);
        else if (fmt == "upc-e")
            renderCodeUPCE(page, rect, val, align);
        else {
            kreportpluginDebug() << "Unknown barcode format:" << fmt;
        }
    }
    return 0;
}
