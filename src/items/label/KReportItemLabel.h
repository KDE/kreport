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

#ifndef KReportItemLabel_H
#define KReportItemLabel_H

#include "KReportItemBase.h"

#include <KProperty>

class QDomNode;

namespace Scripting
{
class Label;
}

/**
*/
class KReportItemLabel : public KReportItemBase
{
Q_OBJECT
public:
    KReportItemLabel();
    explicit KReportItemLabel(const QDomNode & element);
    ~KReportItemLabel() override;

    QString typeName() const override;
    int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset, const QVariant &data, KReportScriptHandler *script) override;

protected:
    KProperty *text;
    KProperty *horizontalAlignment;
    KProperty *verticalAlignment;
    KProperty *font;
    KProperty *foregroundColor;
    KProperty *backgroundColor;
    KProperty *backgroundOpacity;
    KProperty *lineColor;
    KProperty *lineWeight;
    KProperty *lineStyle;

    QString textValue() const;
    QFont fontValue() const {
        return font->value().value<QFont>();
    }
    Qt::Alignment textFlags() const;
    void setText(const QString&);
    KReportTextStyleData textStyleValue() const;
    KReportLineStyle lineStyleValue() const;

private:
    void createProperties() override;

    friend class Scripting::Label;
};
#endif
