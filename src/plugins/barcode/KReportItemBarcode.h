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

#ifndef KREPORTITEMBRCODE_H
#define KREPORTITEMBRCODE_H

#include "KReportItemBase.h"

class QDomNode;

namespace Scripting
{
class Barcode;
}

/**
*/
class KReportItemBarcode : public KReportItemBase
{
Q_OBJECT
public:
    KReportItemBarcode();
    explicit KReportItemBarcode(const QDomNode &element);
    ~KReportItemBarcode() override;

    QString typeName() const override;
    int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset, const QVariant &data, KReportScriptHandler *script) override;
    QString itemDataSource() const override;

protected:

    KProperty * controlSource;
    KProperty * horizontalAlignment;
    KProperty * format;
    KProperty * maxLength;
    KProperty* itemValue;

    // all these values are in inches and
    // are for internal use only
    qreal minWidthData;
    qreal minWidthTotal;
    qreal minHeight;

    int horizontalAlignmentValue() const;
    void setHorizontalAlignmentValue(int value);
    int maxLengthValue() const;
    void setMaxLengthValue(int i);
    QString formatValue() const;
    void setFormatValue(const QString&);

private:
    void createProperties() override;

    friend class Scripting::Barcode;
};

#endif
