/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright 2002, 2003 David Faure <faure@kde.org>
   Copyright 2003 Nicolas GOUTTE <goutte@kde.org>
   Copyright 2007 Thomas Zander <zander@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KReportPageFormat.h"

#include "kreport_debug.h"
#include <QPageSize>
#include <QPrinterInfo>

// paper formats ( mm )
#define PG_A3_WIDTH             297.0
#define PG_A3_HEIGHT            420.0
#define PG_A4_WIDTH             210.0
#define PG_A4_HEIGHT            297.0
#define PG_A5_WIDTH             148.0
#define PG_A5_HEIGHT            210.0
#define PG_B5_WIDTH             182.0
#define PG_B5_HEIGHT            257.0
#define PG_US_LETTER_WIDTH      216.0
#define PG_US_LETTER_HEIGHT     279.0
#define PG_US_LEGAL_WIDTH       216.0
#define PG_US_LEGAL_HEIGHT      356.0
#define PG_US_EXECUTIVE_WIDTH   191.0
#define PG_US_EXECUTIVE_HEIGHT  254.0

// To ignore the clang warning we get because we have a
// for (int i = 0; pageFormatInfo[i].format != -1 ;i++)
// construct and pageFormatInfo has (KReportPageFormat::Format) - 1
#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#endif

struct PageFormatInfo {
    KReportPageFormat::Format format;
    QPrinter::PageSize qprinter;
    const char* shortName; // Short name
    const char* descriptiveName; // Full name, which will be translated
    qreal width; // in mm
    qreal height; // in mm
};

// NOTES:
// - the width and height of non-ISO formats are rounded
// http://en.wikipedia.org/wiki/Paper_size can help
// - the comments "should be..." indicates the exact values if the inch sizes would be multiplied by 25.4 mm/inch

const PageFormatInfo pageFormatInfo[] = {
    { KReportPageFormat::IsoA3Size,       QPrinter::A3,        "A3",        QT_TRANSLATE_NOOP("Page size", "ISO A3"),       297.0,  420.0 },
    { KReportPageFormat::IsoA4Size,       QPrinter::A4,        "A4",        QT_TRANSLATE_NOOP("Page size", "ISO A4"),       210.0,  297.0 },
    { KReportPageFormat::IsoA5Size,       QPrinter::A5,        "A5",        QT_TRANSLATE_NOOP("Page size", "ISO A5"),       148.0,  210.0 },
    { KReportPageFormat::UsLetterSize,    QPrinter::Letter,    "Letter",    QT_TRANSLATE_NOOP("Page size", "US Letter"),    215.9,  279.4 },
    { KReportPageFormat::UsLegalSize,     QPrinter::Legal,     "Legal",     QT_TRANSLATE_NOOP("Page size", "US Legal"),     215.9,  355.6 },
    { KReportPageFormat::ScreenSize,      QPrinter::A4,        "Screen",    QT_TRANSLATE_NOOP("Page size", "Screen"), PG_A4_HEIGHT, PG_A4_WIDTH }, // Custom, so fall back to A4
    { KReportPageFormat::CustomSize,      QPrinter::A4,        "Custom",    QT_TRANSLATE_NOOP("Page size", "Custom"), PG_A4_WIDTH, PG_A4_HEIGHT }, // Custom, so fall back to A4
    { KReportPageFormat::IsoB5Size,       QPrinter::B5,        "B5",        QT_TRANSLATE_NOOP("Page size", "ISO B5"),       182.0,  257.0 },
    { KReportPageFormat::UsExecutiveSize, QPrinter::Executive, "Executive", QT_TRANSLATE_NOOP("Page size", "US Executive"), 191.0,  254.0 }, // should be 190.5 mm x 254.0 mm
    { KReportPageFormat::IsoA0Size,       QPrinter::A0,        "A0",        QT_TRANSLATE_NOOP("Page size", "ISO A0"),       841.0, 1189.0 },
    { KReportPageFormat::IsoA1Size,       QPrinter::A1,        "A1",        QT_TRANSLATE_NOOP("Page size", "ISO A1"),       594.0,  841.0 },
    { KReportPageFormat::IsoA2Size,       QPrinter::A2,        "A2",        QT_TRANSLATE_NOOP("Page size", "ISO A2"),       420.0,  594.0 },
    { KReportPageFormat::IsoA6Size,       QPrinter::A6,        "A6",        QT_TRANSLATE_NOOP("Page size", "ISO A6"),       105.0,  148.0 },
    { KReportPageFormat::IsoA7Size,       QPrinter::A7,        "A7",        QT_TRANSLATE_NOOP("Page size", "ISO A7"),        74.0,  105.0 },
    { KReportPageFormat::IsoA8Size,       QPrinter::A8,        "A8",        QT_TRANSLATE_NOOP("Page size", "ISO A8"),        52.0,   74.0 },
    { KReportPageFormat::IsoA9Size,       QPrinter::A9,        "A9",        QT_TRANSLATE_NOOP("Page size", "ISO A9"),        37.0,   52.0 },
    { KReportPageFormat::IsoB0Size,       QPrinter::B0,        "B0",        QT_TRANSLATE_NOOP("Page size", "ISO B0"),      1030.0, 1456.0 },
    { KReportPageFormat::IsoB1Size,       QPrinter::B1,        "B1",        QT_TRANSLATE_NOOP("Page size", "ISO B1"),       728.0, 1030.0 },
    { KReportPageFormat::IsoB10Size,      QPrinter::B10,       "B10",       QT_TRANSLATE_NOOP("Page size", "ISO B10"),       32.0,   45.0 },
    { KReportPageFormat::IsoB2Size,       QPrinter::B2,        "B2",        QT_TRANSLATE_NOOP("Page size", "ISO B2"),       515.0,  728.0 },
    { KReportPageFormat::IsoB3Size,       QPrinter::B3,        "B3",        QT_TRANSLATE_NOOP("Page size", "ISO B3"),       364.0,  515.0 },
    { KReportPageFormat::IsoB4Size,       QPrinter::B4,        "B4",        QT_TRANSLATE_NOOP("Page size", "ISO B4"),       257.0,  364.0 },
    { KReportPageFormat::IsoB6Size,       QPrinter::B6,        "B6",        QT_TRANSLATE_NOOP("Page size", "ISO B6"),       128.0,  182.0 },
    { KReportPageFormat::IsoC5Size,       QPrinter::C5E,       "C5",        QT_TRANSLATE_NOOP("Page size", "ISO C5"),       163.0,  229.0 }, // Some sources tells: 162 mm x 228 mm
    { KReportPageFormat::UsComm10Size,    QPrinter::Comm10E,   "Comm10",    QT_TRANSLATE_NOOP("Page size", "US Common 10"), 105.0,  241.0 }, // should be 104.775 mm x 241.3 mm
    { KReportPageFormat::IsoDLSize,       QPrinter::DLE,       "DL",        QT_TRANSLATE_NOOP("Page size", "ISO DL"),       110.0,  220.0 },
    { KReportPageFormat::UsFolioSize,     QPrinter::Folio,     "Folio",     QT_TRANSLATE_NOOP("Page size", "US Folio"),     210.0,  330.0 }, // should be 209.54 mm x 330.2 mm
    { KReportPageFormat::UsLedgerSize,    QPrinter::Ledger,    "Ledger",    QT_TRANSLATE_NOOP("Page size", "US Ledger"),    432.0,  279.0 }, // should be 431.8 mm x 297.4 mm
    { KReportPageFormat::UsTabloidSize,   QPrinter::Tabloid,   "Tabloid",   QT_TRANSLATE_NOOP("Page size", "US Tabloid"),   279.0,  432.0 },  // should be 297.4 mm x 431.8 mm
    {(KReportPageFormat::Format) - 1, (QPrinter::PageSize) - 1,   "",   "",   -1,  -1 }
};

QPrinter::PageSize KReportPageFormat::printerPageSize(KReportPageFormat::Format format)
{
    if (format == ScreenSize) {
        kreportWarning() << "You use the page layout SCREEN. Printing in ISO A4 Landscape.";
        return QPrinter::A4;
    }
    if (format == CustomSize) {
        kreportWarning() << "The used page layout (Custom) is not supported by KQPrinter. Printing in A4.";
        return QPrinter::A4;
    }
    return pageFormatInfo[ format ].qprinter;
}

qreal KReportPageFormat::width(Format format, Orientation orientation)
{
    if (orientation == Landscape)
        return height(format, Portrait);
    return pageFormatInfo[ format ].width;
}

qreal KReportPageFormat::height(Format format, Orientation orientation)
{
    if (orientation == Landscape)
        return width(format, Portrait);
    return pageFormatInfo[ format ].height;
}

KReportPageFormat::Format KReportPageFormat::guessFormat(qreal width, qreal height)
{
    for (int i = 0; pageFormatInfo[i].format != -1 ;i++) {
        // We have some tolerance. 1pt is a third of a mm, this is
        // barely noticeable for a page size.
        if (qAbs(width - pageFormatInfo[i].width) < 1.0 && qAbs(height - pageFormatInfo[i].height) < 1.0)
            return pageFormatInfo[i].format;
    }
    return CustomSize;
}

QString KReportPageFormat::formatString(Format format)
{
    return QString::fromLatin1(pageFormatInfo[ format ].shortName);
}

KReportPageFormat::Format KReportPageFormat::formatFromString(const QString & string)
{
    for (int i = 0; pageFormatInfo[i].format != -1 ;i++) {
        if (string == QString::fromLatin1(pageFormatInfo[ i ].shortName))
            return pageFormatInfo[ i ].format;
    }
    // We do not know the format name, so we have a custom format
    return CustomSize;
}

KReportPageFormat::Format KReportPageFormat::defaultFormat()
{
    QPrinterInfo printerInfo = QPrinterInfo::defaultPrinter();
    QPageSize size = printerInfo.defaultPageSize();
    if (size.isValid()) {
        const QPageSize::PageSizeId qprinter = size.id();
        for (int i = 0; pageFormatInfo[i].format != -1 ;i++) {
            if (pageFormatInfo[ i ].qprinter == static_cast<QPrinter::PageSize>(qprinter)){
                return static_cast<Format>(i);
            }
        }
    }
    return IsoA4Size;
}

QString KReportPageFormat::name(Format format)
{
    return QObject::tr(pageFormatInfo[ format ].descriptiveName, "Page size");
}

QStringList KReportPageFormat::localizedPageFormatNames()
{
    QStringList lst;
    for (int i = 0; pageFormatInfo[i].format != -1 ;i++) {
        lst << QObject::tr(pageFormatInfo[ i ].descriptiveName, "Page size");
    }
    return lst;
}

QStringList KReportPageFormat::pageFormatNames()
{
    QStringList lst;
    for (int i = 0; pageFormatInfo[i].format != -1 ;i++) {
        lst << QLatin1String(pageFormatInfo[ i ].shortName);
    }
    return lst;
}