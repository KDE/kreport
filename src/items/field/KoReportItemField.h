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

#ifndef KOREPORTITEMFIELD_H
#define KOREPORTITEMFIELD_H

#include "KoReportItemBase.h"
#include "krsize.h"

#include <KProperty>

class QDomNode;

namespace Scripting
{
class Field;
}

class KoReportItemField : public KoReportItemBase
{
public:
    KoReportItemField();
    explicit KoReportItemField(const QDomNode & element);
    virtual ~KoReportItemField();

    virtual QString typeName() const;
    virtual int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset, const QVariant &data, KRScriptHandler *script);

    virtual QString itemDataSource() const;

protected:

    KProperty * m_controlSource;
    KProperty * m_horizontalAlignment;
    KProperty * m_verticalAlignment;
    KProperty * m_font;
    //KProperty * m_trackTotal;
    //KProperty * m_trackBuiltinFormat;
    //KProperty * _useSubTotal;
    //KProperty * _trackTotalFormat;
    KProperty * m_foregroundColor;
    KProperty * m_backgroundColor;
    KProperty* m_backgroundOpacity;
    KProperty* m_lineColor;
    KProperty* m_lineWeight;
    KProperty* m_lineStyle;
    KProperty* m_wordWrap;
    KProperty* m_canGrow;
    KProperty* m_itemValue;

    //bool builtinFormat;
    //QString format;

    QStringList fieldNames(const QString &);

    KRLineStyleData lineStyle() const;
    KRTextStyleData textStyle() const;

    void setTrackTotal(bool);
    void setTrackTotalFormat(const QString &, bool = false);
    void setUseSubTotal(bool);
    int textFlags() const;

    QFont font() const {
        return m_font->value().value<QFont>();
    }

    void setItemDataSource(const QString&);

private:
    virtual void createProperties();

    friend class Scripting::Field;
};

#endif
