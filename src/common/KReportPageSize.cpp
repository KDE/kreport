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
    QPageSize::PageSizeId pageSize;
};

static const KReportPageSizeInfo pageSizeInfo[] = {
    { "A3", QPageSize::A3},
    { "A4", QPageSize::A4},
    { "A5", QPageSize::A5},
    { "Letter", QPageSize::Letter},
    { "Legal", QPageSize::Legal},
    { "Custom", QPageSize::Custom},
    { "B6", QPageSize::B6},
    { "B5", QPageSize::B5},
    { "B4", QPageSize::B4},
    { "Executive", QPageSize::Executive},
    { "A0", QPageSize::A0},
    { "A1", QPageSize::A1},
    { "A2", QPageSize::A2},
    { "A6", QPageSize::A6},
    { "A7", QPageSize::A7},
    { "A8", QPageSize::A8},
    { "A9", QPageSize::A9},
    { "B0", QPageSize::B0},
    { "B1", QPageSize::B1},
    { "B10", QPageSize::B10},
    { "B2", QPageSize::B2},
    { "B3", QPageSize::B3},
    { "C5E", QPageSize::C5E},
    { "Comm10", QPageSize::Comm10E},
    { "DL", QPageSize::DLE},
    { "Folio", QPageSize::Folio},
    { "Ledger", QPageSize::Ledger},
    { "Tabloid", QPageSize::Tabloid},
    { "EnvelopeDL", QPageSize::EnvelopeDL},
    { "EnvelopeC6", QPageSize::EnvelopeC6},
    { "EnvelopeC65", QPageSize::EnvelopeC65},
    { "EnvelopeC5", QPageSize::EnvelopeC5},
    { "EnvelopeC4", QPageSize::EnvelopeC4},
    { "EnvelopeMonarch", QPageSize::EnvelopeMonarch},
    { "EnvelopePersonal", QPageSize::EnvelopePersonal},
    { "Envelope9", QPageSize::Envelope9},
    { "Envelope10", QPageSize::Envelope10},
    { "Envelope11", QPageSize::Envelope11},
    { "Envelope12", QPageSize::Envelope12},
    { "Postcard", QPageSize::Postcard},
    { "DoublePostcard", QPageSize::DoublePostcard},

    { "", QPageSize::LastPageSize}

};

QStringList KReportPageSize::pageFormatNames()
{
    QStringList lst;
    for (int i = 0; pageSizeInfo[i].pageSize != QPageSize::LastPageSize ;i++) {
        lst << QPageSize::name(pageSizeInfo[ i ].pageSize);
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
