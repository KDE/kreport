/* This file is part of the KDE project
   Copyright (C) 2010-2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KREPORTUTILS_H
#define KREPORTUTILS_H

#include "kreport_export.h"

#include <QPageSize>
#include <QRectF>
#include <QColor>
#include <QFont>

class QDomDocument;
class QDomElement;
class QFont;
class QPointF;
class KProperty;
class KReportTextStyleData;
class KReportLineStyle;

namespace KReportUtils
{
    KREPORT_EXPORT QString attr(const QDomElement &el, const QString &attrName,
                                const QString &defaultValue = QString());

    KREPORT_EXPORT QByteArray attr(const QDomElement &el, const QString &attrName,
                                   const QByteArray &defaultValue = QByteArray());

    KREPORT_EXPORT bool attr(const QDomElement &el, const QString &attrName, bool defaultValue = false);

    KREPORT_EXPORT int attr(const QDomElement &el, const QString &attrName, int defaultValue = 0);

    KREPORT_EXPORT qreal attr(const QDomElement &el, const QString &attrName, qreal defaultValue = 0.0);

    KREPORT_EXPORT QColor attr(const QDomElement &el, const QString &attrName, const QColor &defaultValue = QColor());

    //! @return percent value converted to qreal, e.g. 1.0 for "100%", 0.505 for "50.5%".
    //! @a defaultValue is returned if there is not "%" suffix or no proper number.
    KREPORT_EXPORT qreal attrPercent(const QDomElement& el, const QString &attrName, qreal defaultValue = 0.0);

    //! @return pen style from @a str or @a defaultValue
    //! Values from ODF 1.2 19.493 style:line-style are also recognized.
    KREPORT_EXPORT Qt::PenStyle penStyle(const QString &str, Qt::PenStyle defaultValue);

    //! @return vertical alignment flag from @a str or @a defaultValue
    KREPORT_EXPORT Qt::Alignment verticalAlignment(const QString &str, Qt::Alignment defaultValue);

    //! @return horizontal alignment flag from @a str or @a defaultValue
    KREPORT_EXPORT Qt::Alignment horizontalAlignment(const QString &str, Qt::Alignment defaultValue);

    //! @return vertical alignment flag name from @a alignment
    KREPORT_EXPORT QString verticalToString(Qt::Alignment alignment);

    //! @return horizontal alignment flag from @a alignment
    KREPORT_EXPORT QString horizontalToString(Qt::Alignment alignment);

    //! @return name value read from report:name attribute of @a el.
    //! If the attribute is missing, @a defaultValue is returned.
    KREPORT_EXPORT QString readNameAttribute(
        const QDomElement &el, const QString &defaultValue = QString());

    //! @return size value read from svg:width and svg:height attributes of @a el.
    //! If any of the attributes are missing, @a defaultValue is returned.
    //! @a defaultValue should be specified in Points.
    KREPORT_EXPORT QSizeF readSizeAttributes(
        const QDomElement &el, const QSizeF &defaultValue = QSizeF());

    //! @return rectangle value read from svg:x, svg:y, svg:width, svg:height attributes of @a el.
    //! If any of the attributes are missing, @a defaultValue is returned.
    //! @a defaultValue should be specified in Points.
    KREPORT_EXPORT QRectF readRectAttributes(
        const QDomElement &el, const QRectF &defaultValue = QRectF());

    //! @return Z index value read from report:z-index attribute of @a el.
    //! If the attribute is missing @a defaultValue is returned.
    //! @a defaultValue should be specified in Points.
    KREPORT_EXPORT qreal readZAttribute(const QDomElement &el, qreal defaultValue = 0.0);

    //! @return name of section type read from report:section-type attribute of @a el.
    //! If the attribute is missing, @a defaultValue is returned.
    KREPORT_EXPORT QString readSectionTypeNameAttribute(
        const QDomElement &el, const QString &defaultValue = QString());

    //! @return percent value for element @a name. If the element is missing, returns @a defaultPercentValue.
    //! If @a ok is not 0, *ok is set to the result.
    KREPORT_EXPORT int readPercent(const QDomElement &el, const QString &attrName,
        int defaultPercentValue, bool *ok);

    //! Reads all font attributes for element @a el into @a font.
    //! @todo add unit tests
    KREPORT_EXPORT void readFontAttributes(const QDomElement& el, QFont* font);

    //! Writes all attributes of font @a font into element @a el.
    //! @todo add unit tests
    KREPORT_EXPORT void writeFontAttributes(QDomElement *el, const QFont &font);

    //! Writes attributes for the rect position @a pos, @a siz
    KREPORT_EXPORT void buildXMLRect(QDomElement *entity, const QPointF &pos, const QSizeF &size);

    //! Writes attributes for text style @a ts
    KREPORT_EXPORT void buildXMLTextStyle(QDomDocument *doc, QDomElement *entity, const KReportTextStyleData &ts);

    //! Writes attributes for line style @a ls
    KREPORT_EXPORT void buildXMLLineStyle(QDomDocument *doc, QDomElement *entity, const KReportLineStyle &ls);

    //! Writes attributes for the property @a p
    KREPORT_EXPORT void addPropertyAsAttribute(QDomElement* e, KProperty* p);

    //! Writes @a attribute to element @a e, @a value is stored in points with unit 'pt'
    KREPORT_EXPORT void setAttribute(QDomElement *e, const QString &attribute, double value);

    //! Writes point @a value as attributes to element @a e
    KREPORT_EXPORT void setAttribute(QDomElement *e, const QPointF &value);

    //! Writes size @a value as attributes to element @a e
    KREPORT_EXPORT void setAttribute(QDomElement *e, const QSizeF &value);

    //! Writes @a attribute to element @a e, @a value is stored as boolean
    KREPORT_EXPORT void setAttribute(QDomElement *e, const QString &attribute, bool value);

    //! Reads attributes from @a elemSource into text style @a ts
    KREPORT_EXPORT bool parseReportTextStyleData(const QDomElement & elemSource, KReportTextStyleData *ts);

    //! Reads attributes from @a elemSource into line style @a ls
    KREPORT_EXPORT bool parseReportLineStyleData(const QDomElement & elemSource, KReportLineStyle *ls);

    //! @return page size ID for page key (the PPD standard mediaOption keyword, e.g. "A4")
    //! @note It's an efficient workaround because QPageSize misses this function.
    KREPORT_EXPORT QPageSize::PageSizeId pageSizeId(const QString &key);

    //! Like QPageSize::PageSizeId pageSizeId(const QString &key) but returns entire QPageSize object.
    KREPORT_EXPORT QPageSize pageSize(const QString &key);
} // KReportUtils

#endif
