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
    ~KReportItemText() override;

    QString typeName() const override;
    int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset, const QVariant &data, KReportScriptHandler *script) override;

    QString itemDataSource() const override;

protected:
    KProperty* controlSource;
    KProperty* horizontalAlignment;
    KProperty* verticalAlignment;
    KProperty* font;
    KProperty* foregroundColor;
    KProperty* backgroundColor;
    KProperty* backgroundOpacity;
    KProperty* lineColor;
    KProperty* lineWeight;
    KProperty* lineStyle;
    KProperty* itemValue;


    qreal bottomPadding;

    Qt::Alignment textFlags() const;
    QFont fontValue() const {
        return font->value().value<QFont>();
    }

    void setBottomPaddingValue(qreal bp);
    qreal bottomPaddingValue() const;


    KReportTextStyleData textStyleValue() const;
    KReportLineStyle lineStyleValue() const;

private:
    void createProperties() override;

    friend class Scripting::Text;
};

#endif
