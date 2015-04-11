/*
 * Kexi Report Plugin
 * Copyright (C) 2009-2010 by Adam Pigg (adam@piggz.co.uk)
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

#ifndef KOREPORTITEMCHECK_H
#define KOREPORTITEMCHECK_H

#include "common/KoReportItemBase.h"
#include "common/krpos.h"
#include "common/krsize.h"

#include <QRect>
#include <QPainter>
#include <QDomDocument>

namespace Scripting
{
class Check;
}

class KoReportItemCheck : public KoReportItemBase
{
public:
    KoReportItemCheck();
    explicit KoReportItemCheck(QDomNode &element);
    virtual ~KoReportItemCheck();

    virtual QString typeName() const;
    virtual int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset, const QVariant &data, KRScriptHandler *script);

    virtual QString itemDataSource()const;

protected:

    KProperty * m_controlSource;
    KProperty* m_checkStyle;
    KProperty* m_foregroundColor;
    KProperty* m_lineColor;
    KProperty* m_lineWeight;
    KProperty* m_lineStyle;
    KProperty* m_staticValue;

    bool value();
    void setValue(bool);
    KRLineStyleData lineStyle();

private:
    virtual void createProperties();

    friend class Scripting::Check;
};

#endif // KOREPORTITEMCHECK_H
