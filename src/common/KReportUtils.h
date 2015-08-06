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

class QDomDocument;
class QDomElement;
class QFont;
class QPointF;
class KProperty;
class KRPos;
class KRSize;
class KRTextStyleData;
class KRLineStyleData;

namespace KReportUtils
{
    KREPORT_EXPORT QString attr(const QDomElement &el, const char *attrName,
                                QString defaultValue = QString());

    KREPORT_EXPORT bool attr(const QDomElement &el, const char *attrName, bool defaultValue = false);

    KREPORT_EXPORT int attr(const QDomElement &el, const char *attrName, int defaultValue = 0);

    KREPORT_EXPORT qreal attr(const QDomElement &el, const char *attrName, qreal defaultValue = 0.0);

    KREPORT_EXPORT QColor attr(const QDomElement &el, const char *attrName, const QColor &defaultValue);

    //! @return percent value converted to qreal, e.g. 1.0 for "100%", 0.505 for "50.5%".
    //! @a defaultValue is returned if there is not "%" suffix or no proper number.
    KREPORT_EXPORT qreal attrPercent(const QDomElement& el, const char* attrName, qreal defaultValue);

    //! @return pen style from attribute @a attrname or @a defaultValue
    //! Values from ODF 1.2 19.493 style:line-style are also recognized.
    KREPORT_EXPORT Qt::PenStyle attr(const QDomElement& el, const char* attrName, Qt::PenStyle defaultValue);

    //! @return rectangle value read from svg:x, svg:y, svg:width, svg:height attributes of @a el.
    //! If any of the arguments are missing, @a defaultValue is returned.
    KREPORT_EXPORT QRectF readRectF(const QDomElement &el, const QRectF &defaultValue);

    //! @return percent value for element @a name. If the element is missing, returns @a defaultPercentValue.
    //! If @a ok is not 0, *ok is set to the result.
    KREPORT_EXPORT int readPercent(const QDomElement & el, const char* name, int defaultPercentValue, bool *ok);

    //! Reads all font attributes for element @a el into @a font.
    //! @todo add unit tests
    KREPORT_EXPORT bool readFontAttributes(const QDomElement& el, QFont* font);

    //! Writes all attributes of font @a font into element @a el.
    //! @todo add unit tests
    KREPORT_EXPORT void writeFontAttributes(QDomElement *el, const QFont &font);

    //! Writes attributes for the rect position @p pos, @p siz
    KREPORT_EXPORT void buildXMLRect(QDomElement *entity, KRPos *pos, KRSize *size);

    //! Writes attributes for text style @p ts
    KREPORT_EXPORT void buildXMLTextStyle(QDomDocument *doc, QDomElement *entity, const KRTextStyleData &ts);

    //! Writes attributes for line style @p ls
    KREPORT_EXPORT void buildXMLLineStyle(QDomDocument *doc, QDomElement *entity, const KRLineStyleData &ls);

    //! Writes attributes for the property @p p
    KREPORT_EXPORT void addPropertyAsAttribute(QDomElement* e, KProperty* p);

    //! Writes @p attribute to element @p e, @p value is stored in points with unit 'pt'
    KREPORT_EXPORT void setAttribute(QDomElement *e, const QString &attribute, double value);

    //! Writes point @p value as attributes to element @p e
    KREPORT_EXPORT void setAttribute(QDomElement *e, const QPointF &value);

    //! Writes size @p value as attributes to element @p e
    KREPORT_EXPORT void setAttribute(QDomElement *e, const QSizeF &value);

    //! Reads attributes from @p elemSource into text style @p ts
    KREPORT_EXPORT bool parseReportTextStyleData(const QDomElement & elemSource, KRTextStyleData *ts);

    //! Reads attributes from @p elemSource into line style @p ls
    KREPORT_EXPORT bool parseReportLineStyleData(const QDomElement & elemSource, KRLineStyleData *ls);

    //! Reads attributes from @p elemSource into rect @p pos, @p siz
    KREPORT_EXPORT bool parseReportRect(const QDomElement & elemSource, KRPos *pos, KRSize *size);

    //! @return page size ID for page key (the PPD standard mediaOption keyword, e.g. "A4")
    //! @note It's an efficient workaround because QPageSize misses this function.
    KREPORT_EXPORT QPageSize::PageSizeId pageSizeId(const QString &key);

    //! Like QPageSize::PageSizeId pageSizeId(const QString &key) but returns entire QPageSize object.
    KREPORT_EXPORT QPageSize pageSize(const QString &key);

} // KReportUtils

#endif
