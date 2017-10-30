 /* This file is part of the KDE project
    Copyright (C) 2015 by Adam Pigg <adam@piggz.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "KReportPageSize.h"
#include <QCoreApplication>
#include <QPrinterInfo>

struct KReportPageSizeInfo {
    const char *name;
    struct { const char *source; const char *comment; } description;
    QPageSize::PageSizeId pageSize;
};

static const KReportPageSizeInfo pageSizeInfo[] = {
    { "A3", QT_TRANSLATE_NOOP3("KReportPageFormat", "A3", ""), QPageSize::A3},
    { "A4", QT_TRANSLATE_NOOP3("KReportPageFormat", "A4", ""), QPageSize::A4},
    { "A5", QT_TRANSLATE_NOOP3("KReportPageFormat", "A5", ""), QPageSize::A5},
    { "Letter", QT_TRANSLATE_NOOP3("KReportPageFormat", "US Letter",
                                   "Usually don't translate literally, use \"US Letter\""),
                QPageSize::Letter},
    { "Legal", QT_TRANSLATE_NOOP3("KReportPageFormat", "US Legal",
                                  "Usually don't translate literally, use \"US Legal\""),
                QPageSize::Legal},
    { "Custom", QT_TRANSLATE_NOOP3("KReportPageFormat", "Custom", ""), QPageSize::Custom},
    { "B6", QT_TRANSLATE_NOOP3("KReportPageFormat", "B6", ""), QPageSize::B6},
    { "B5", QT_TRANSLATE_NOOP3("KReportPageFormat", "B5", ""), QPageSize::B5},
    { "B4", QT_TRANSLATE_NOOP3("KReportPageFormat", "B4", ""), QPageSize::B4},
    { "Executive", QT_TRANSLATE_NOOP3("KReportPageFormat", "Executive", ""), QPageSize::Executive},
    { "A0", QT_TRANSLATE_NOOP3("KReportPageFormat", "A0", ""), QPageSize::A0},
    { "A1", QT_TRANSLATE_NOOP3("KReportPageFormat", "A1", ""), QPageSize::A1},
    { "A2", QT_TRANSLATE_NOOP3("KReportPageFormat", "A2", ""), QPageSize::A2},
    { "A6", QT_TRANSLATE_NOOP3("KReportPageFormat", "A6", ""), QPageSize::A6},
    { "A7", QT_TRANSLATE_NOOP3("KReportPageFormat", "A7", ""), QPageSize::A7},
    { "A8", QT_TRANSLATE_NOOP3("KReportPageFormat", "A8", ""), QPageSize::A8},
    { "A9", QT_TRANSLATE_NOOP3("KReportPageFormat", "A9", ""), QPageSize::A9},
    { "B0", QT_TRANSLATE_NOOP3("KReportPageFormat", "B0", ""), QPageSize::B0},
    { "B1", QT_TRANSLATE_NOOP3("KReportPageFormat", "B1", ""), QPageSize::B1},
    { "B10", QT_TRANSLATE_NOOP3("KReportPageFormat", "B10", ""), QPageSize::B10},
    { "B2", QT_TRANSLATE_NOOP3("KReportPageFormat", "B2", ""), QPageSize::B2},
    { "B3", QT_TRANSLATE_NOOP3("KReportPageFormat", "B3", ""), QPageSize::B3},
    { "C5E", QT_TRANSLATE_NOOP3("KReportPageFormat", "C5E", ""), QPageSize::C5E},
    { "Comm10", QT_TRANSLATE_NOOP3("KReportPageFormat", "Comm10", ""), QPageSize::Comm10E},
    { "DL", QT_TRANSLATE_NOOP3("KReportPageFormat", "ISO DL", ""), QPageSize::DLE},
    { "Folio", QT_TRANSLATE_NOOP3("KReportPageFormat", "US Folio", ""), QPageSize::Folio},
    { "Ledger", QT_TRANSLATE_NOOP3("KReportPageFormat", "US Ledger", ""), QPageSize::Ledger},
    { "Tabloid", QT_TRANSLATE_NOOP3("KReportPageFormat", "US Tabloid", ""), QPageSize::Tabloid},
    { "EnvelopeDL", QT_TRANSLATE_NOOP3("KReportPageFormat", "DL Envelope", ""), QPageSize::EnvelopeDL},
    { "EnvelopeC6", QT_TRANSLATE_NOOP3("KReportPageFormat", "C6 Envelope", ""), QPageSize::EnvelopeC6},
    { "EnvelopeC65", QT_TRANSLATE_NOOP3("KReportPageFormat", "C6/5 Envolope", ""), QPageSize::EnvelopeC65},
    { "EnvelopeC5", QT_TRANSLATE_NOOP3("KReportPageFormat", "C5 Envelope", ""), QPageSize::EnvelopeC5},
    { "EnvelopeC4", QT_TRANSLATE_NOOP3("KReportPageFormat", "C4 Envelope", ""), QPageSize::EnvelopeC4},
    { "EnvelopeMonarch", QT_TRANSLATE_NOOP3("KReportPageFormat", "Monarch Envelope", ""), QPageSize::EnvelopeMonarch},
    { "EnvelopePersonal", QT_TRANSLATE_NOOP3("KReportPageFormat", "Personal Envelope", ""), QPageSize::EnvelopePersonal},
    { "Envelope9", QT_TRANSLATE_NOOP3("KReportPageFormat", "9 Envelope", ""), QPageSize::Envelope9},
    { "Envelope10", QT_TRANSLATE_NOOP3("KReportPageFormat", "10 Envelope", ""), QPageSize::Envelope10},
    { "Envelope11", QT_TRANSLATE_NOOP3("KReportPageFormat", "11 Envelope", ""), QPageSize::Envelope11},
    { "Envelope12", QT_TRANSLATE_NOOP3("KReportPageFormat", "12 Envelope", ""), QPageSize::Envelope12},
    { "Postcard", QT_TRANSLATE_NOOP3("KReportPageFormat", "Postcard", ""), QPageSize::Postcard},
    { "DoublePostcard", QT_TRANSLATE_NOOP3("KReportPageFormat", "Double Postcard", ""), QPageSize::DoublePostcard},

    { "", {"", ""}, QPageSize::LastPageSize}

};

QStringList KReportPageSize::pageFormatNames()
{
    QStringList lst;
    for (int i = 0; pageSizeInfo[i].pageSize != QPageSize::LastPageSize ;i++) {
        lst << QCoreApplication::translate("KReportPageFormat",
                                           pageSizeInfo[ i ].description.source,
                                           pageSizeInfo[ i ].description.comment);
    }
    return lst;
}

QStringList KReportPageSize::pageFormatKeys()
{
    QStringList lst;
    for (int i = 0; pageSizeInfo[i].pageSize != QPageSize::LastPageSize ;i++) {
        lst << QLatin1String(pageSizeInfo[ i ].name);
    }
    return lst;
}

QPageSize::PageSizeId KReportPageSize::defaultSize()
{
    QPrinterInfo printerInfo = QPrinterInfo::defaultPrinter();
    QPageSize size = printerInfo.defaultPageSize();
    if (size.isValid()) {
        return size.id();
    }
    return QPageSize::A4;
}

QString KReportPageSize::pageSizeKey(QPageSize::PageSizeId id)
{
    for (int i = 0; pageSizeInfo[i].pageSize != QPageSize::LastPageSize ;i++) {
        if (pageSizeInfo[i].pageSize == id) {
	  return QLatin1String(pageSizeInfo[i].name);
	}
    }
    return QString();
}

QPageSize::PageSizeId KReportPageSize::pageSize(const QString &size)
{
    for (int i = 0; pageSizeInfo[i].pageSize != QPageSize::LastPageSize ;i++) {
        if (QLatin1String(pageSizeInfo[i].name) == size) {
	  return pageSizeInfo[i].pageSize;
	}
    }
    return defaultSize();
}
