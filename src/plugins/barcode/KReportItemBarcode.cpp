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

#include "KReportItemBarcode.h"

#include <KProperty>
#include <KPropertyListData>
#include <KPropertySet>

#include <QDomNode>

#include "kreportplugin_debug.h"

#include "barcodes.h"

KReportItemBarcode::KReportItemBarcode()
    : minWidthData(0), minWidthTotal(0), minHeight(0)
{
    createProperties();
}

KReportItemBarcode::KReportItemBarcode(const QDomNode & element)
    : KReportItemBarcode()
{
    nameProperty()->setValue(element.toElement().attribute(QLatin1String("report:name")));
    controlSource->setValue(element.toElement().attribute(QLatin1String("report:item-data-source")));
    itemValue->setValue(element.toElement().attribute(QLatin1String("report:value")));
    setZ(element.toElement().attribute(QLatin1String("report:z-index")).toDouble());
    horizontalAlignment->setValue(element.toElement().attribute(QLatin1String("report:horizontal-align")));
    maxLength->setValue(element.toElement().attribute(QLatin1String("report:barcode-max-length")).toInt());
    format->setValue(element.toElement().attribute(QLatin1String("report:barcode-format")));
    parseReportRect(element.toElement());

}

void KReportItemBarcode::setMaxLengthValue(int i)
{
    if (i > 0) {
        if (maxLength->value().toInt() != i) {
            maxLength->setValue(i);
        }
        if (format->value().toString() == QLatin1String("3of9")) {
            int C = i; // number of characters
            int N = 2; // narrow mult for wide line
            int X = 1; // narrow line width
            int I = 1; // interchange line width
            minWidthData = (((C + 2) * ((3 * N) + 6) * X) + ((C + 1) * I)) / 100.0;
            //m_minHeight = m_minWidthData * 0.15;
            /*if(min_height < 0.25)*/
            minHeight = 0.25;
            minWidthTotal = minWidthData + 0.22; // added a little buffer to make sure we don't loose any
            // of our required quiet zone in conversions
        } else if (format->value().toString() == QLatin1String("3of9+")) {
            int C = i * 2; // number of characters
            int N = 2; // narrow mult for wide line
            int X = 1; // 1px narrow line
            int I = 1; // 1px narrow line interchange
            minWidthData = (((C + 2) * ((3 * N) + 6) * X) + ((C + 1) * I)) / 100.0;
            //m_minHeight = m_minWidthData * 0.15;
            /*if(min_height < 0.25)*/
            minHeight = 0.25;
            minWidthTotal = minWidthData + 0.22; // added a little buffer to make sure we don't loose any
            // of our required quiet zone in conversions
        } else if (format->value().toString() == QLatin1String("i2of5")) {
            int C = i * 2; // number of characters
            int N = 2; // narrow mult for wide line
            int X = 1; // 1px narrow line
            minWidthTotal = ((C * (2.0*N + 3.0) + 6.0 + N) * X);
            minHeight = 0.25;
            minWidthTotal = minWidthData + 0.22;
        } else if (format->value().toString() == QLatin1String("128")) {
            int C = i; // assuming 1:1 ratio of data passed in to data actually used in encoding
            int X = 1; // 1px wide
            minWidthData = (((11 * C) + 35) * X) / 100.0;       // assuming CODE A or CODE B
            //m_minHeight = m_minWidthData * 0.15;
            /*if(min_height < 0.25)*/
            minHeight = 0.25;
            minWidthTotal = minWidthData + 0.22; // added a little bugger to make sure we don't loose any
            // of our required quiet zone in conversions
        } else if (format->value().toString() == QLatin1String("upc-a")) {
            minWidthData = 0.95;
            minWidthTotal = 1.15;
            minHeight = 0.25;
        } else if (format->value().toString() == QLatin1String("upc-e")) {
            minWidthData = 0.52;
            minWidthTotal = 0.70;
            minHeight = 0.25;
        } else if (format->value().toString() == QLatin1String("ean13")) {
            minWidthData = 0.95;
            minWidthTotal = 1.15;
            minHeight = 0.25;
        } else if (format->value().toString() == QLatin1String("ean8")) {
            minWidthData = 0.67;
            minWidthTotal = 0.90;
            minHeight = 0.25;
        } else {
            kreportpluginWarning() << "Unknown format encountered: " << format->value().toString();
        }
    }
}

void KReportItemBarcode::createProperties()
{
    controlSource
        = new KProperty("item-data-source", new KPropertyListData, QVariant(), tr("Data Source"));

    itemValue = new KProperty("value", QString(), tr("Value"),
                                tr("Value used if not bound to a field"));

    KPropertyListData *listData = new KPropertyListData(
        { QLatin1String("left"), QLatin1String("center"), QLatin1String("right") },
        QVariantList{ tr("Left"), tr("Center"), tr("Right") });
    horizontalAlignment = new KProperty("horizontal-align", listData, QLatin1String("left"),
                                          tr("Horizontal Alignment"));

    listData = new KPropertyListData(
    QStringList()
         << QLatin1String("3of9") << QLatin1String("3of9+") << QLatin1String("128")
         << QLatin1String("ean8") << QLatin1String("ean13") << QLatin1String("i2of5")
         << QLatin1String("upc-a") << QLatin1String("upc-e"),
    QStringList()
            << tr("Code 3 of 9", "Barcode symbology, keep short")
            << tr("Code 3 of 9 Ext.", "3 of 3 Extended: Barcode symbology, keep short")
            << tr("Code 128", "Barcode symbology, keep short")
            << tr("EAN-8", "Barcode symbology, keep short")
            << tr("EAN-13", "Barcode symbology, keep short")
            << tr("Interleaved 2 of 5", "Interleaved barcode 2 of 5: barcode symbology, keep short")
            << tr("UPC-A", "Barcode symbology, keep short")
            << tr("UPC-E", "Barcode symbology, keep short")
    );
    format = new KProperty("barcode-format", listData, QLatin1String("3of9"),
                             tr("Barcode Format"));

    maxLength = new KProperty("barcode-max-length", 5, tr("Max Length"),
                                tr("Maximum Barcode Length"));

    propertySet()->addProperty(controlSource);
    propertySet()->addProperty(itemValue);
    propertySet()->addProperty(format);
    propertySet()->addProperty(horizontalAlignment);
    propertySet()->addProperty(maxLength);
}

KReportItemBarcode::~KReportItemBarcode()
{
}

int KReportItemBarcode::horizontalAlignmentValue() const
{
    QByteArray a = horizontalAlignment->value().toByteArray();

    if (a == "left")
        return 0;
    else if (a == "center")
        return 1;
    else if (a == "right")
        return 2;
    else
        return 0;
}

QString KReportItemBarcode::itemDataSource() const
{
    return controlSource->value().toString();
}

QString KReportItemBarcode::formatValue() const
{
    return format->value().toString();
}

int KReportItemBarcode::maxLengthValue() const
{
    return maxLength->value().toInt();
}

void KReportItemBarcode::setFormatValue(const QString& f)
{
    format->setValue(f);
}

void KReportItemBarcode::setHorizontalAlignmentValue(int value)
{
    switch (value) {
    case 1:
        horizontalAlignment->setValue(QStringLiteral("center"));
        break;
    case 2:
        horizontalAlignment->setValue(QStringLiteral("right"));
        break;
    case 0:
    default:
        horizontalAlignment->setValue(QStringLiteral("left"));
    }
}

//RTTI
QString KReportItemBarcode::typeName() const
{
    return QLatin1String("barcode");
}

int KReportItemBarcode::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                          const QVariant &data, KReportScriptHandler *script)
{
    Q_UNUSED(section);
    Q_UNUSED(script);

    QPointF pos = scenePosition(position());
    QSizeF siz = sceneSize(size());
    pos += offset;

    QRectF rect = QRectF(pos, siz);

    QString val;

    if (controlSource->value().toString().isEmpty()) {
        val = itemValue->value().toString();
    } else {
        val = data.toString();
    }

    if (page) {
        QByteArray fmt = format->value().toByteArray();
        int align = horizontalAlignmentValue();
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
            kreportpluginWarning() << "Unknown barcode format:" << fmt;
        }
    }
    return 0;
}
