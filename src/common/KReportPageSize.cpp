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
    const char *description;
    QPageSize::PageSizeId pageSize;
};

const KReportPageSizeInfo pageSizeInfo[] = {
    { "A3",	QT_TRANSLATE_NOOP("KReportPageFormat", "A3"), QPageSize::A3},
    { "A4",	QT_TRANSLATE_NOOP("KReportPageFormat", "A4"), QPageSize::A4},
    { "A5",	QT_TRANSLATE_NOOP("KReportPageFormat", "A5"), QPageSize::A5},
    { "Letter",	QT_TRANSLATE_NOOP("KReportPageFormat", "US Letter"), QPageSize::Letter},
    { "Legal",	QT_TRANSLATE_NOOP("KReportPageFormat", "US Legal"), QPageSize::Legal},
    { "Screen",	QT_TRANSLATE_NOOP("KReportPageFormat", "Screen"), QPageSize::A4},
    { "", "", QPageSize::LastPageSize}
};

QStringList KReportPageSize::pageFormatNames()
{
    QStringList lst;
    for (int i = 0; pageSizeInfo[i].pageSize != QPageSize::LastPageSize ;i++) {
        lst << QCoreApplication::translate("KReportPageFormat", pageSizeInfo[ i ].description);
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
