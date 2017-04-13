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

#ifndef KREPORTITEMTEXT_H
#define KREPORTITEMTEXT_H

#include "KReportItemBase.h"

#include <KProperty>

class QDomNode;

namespace Scripting
{
class Text;

}
/**
*/
class KReportItemText : public KReportItemBase
{
Q_OBJECT
public:
    KReportItemText();
    explicit KReportItemText(const QDomNode & element);
    virtual ~KReportItemText();

    virtual QString typeName() const;
    virtual int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset, const QVariant &data, KReportScriptHandler *script);

    virtual QString itemDataSource() const;

protected:

    KProperty* m_controlSource;
    KProperty* m_horizontalAlignment;
    KProperty* m_verticalAlignment;
    KProperty* m_font;
    KProperty* m_foregroundColor;
    KProperty* m_backgroundColor;
    KProperty* m_backgroundOpacity;
    KProperty* m_lineColor;
    KProperty* m_lineWeight;
    KProperty* m_lineStyle;
    KProperty* m_itemValue;


    qreal m_bottomPadding;

    Qt::Alignment textFlags() const;
    QFont font() const {
        return m_font->value().value<QFont>();
    }

    void setBottomPadding(qreal bp);
    qreal bottomPadding() const;


    KReportTextStyleData textStyle() const;
    KReportLineStyle lineStyle() const;

private:
    virtual void createProperties();

    friend class Scripting::Text;
};

#endif
