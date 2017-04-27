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

    KProperty * m_controlSource;
    KProperty * m_horizontalAlignment;
    KProperty * m_format;
    KProperty * m_maxLength;
    KProperty* m_itemValue;


    int alignment();
    void setAlignment(int);
    int maxLength();
    void setMaxLength(int i);
    QString format();
    void setFormat(const QString&);

    // all these values are in inches and
    // are for internal use only
    qreal m_minWidthData;
    qreal m_minWidthTotal;
    qreal m_minHeight;

private:
    void createProperties() override;

    friend class Scripting::Barcode;
};

#endif
