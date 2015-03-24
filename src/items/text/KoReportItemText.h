/*
 * Kexi Report Plugin
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
#ifndef KOREPORTITEMTEXT_H
#define KOREPORTITEMTEXT_H

#include <KoReportItemBase.h>
#include <QRect>
#include <QPainter>
#include <QDomDocument>
#include <krpos.h>
#include <krsize.h>

namespace Scripting
{
class Text;

}
/**
 @author
*/
class KoReportItemText : public KoReportItemBase
{
public:
    KoReportItemText();
    explicit KoReportItemText(QDomNode & element);
    virtual ~KoReportItemText();

    virtual QString typeName() const;
    virtual int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset, const QVariant &data, KRScriptHandler *script);

    virtual QString itemDataSource() const;

protected:

    KoProperty::Property* m_controlSource;
    KoProperty::Property* m_horizontalAlignment;
    KoProperty::Property* m_verticalAlignment;
    KoProperty::Property* m_font;
    KoProperty::Property* m_foregroundColor;
    KoProperty::Property* m_backgroundColor;
    KoProperty::Property* m_backgroundOpacity;
    KoProperty::Property* m_lineColor;
    KoProperty::Property* m_lineWeight;
    KoProperty::Property* m_lineStyle;
    KoProperty::Property* m_itemValue;


    qreal m_bottomPadding;

    Qt::Alignment textFlags() const;
    QFont font() const {
        return m_font->value().value<QFont>();
    }

    void setBottomPadding(qreal bp);
    qreal bottomPadding() const;

    
    KRTextStyleData textStyle();
    KRLineStyleData lineStyle();
    
private:
    virtual void createProperties();

    friend class Scripting::Text;
};

#endif
