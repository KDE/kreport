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

#ifndef KREPORTITEMLINE_H
#define KREPORTITEMLINE_H

#include "KReportItemBase.h"
#include "kreport_export.h"

class QDomNode;

namespace Scripting
{
class Line;
}

/**
*/
class  KReportItemLine : public KReportItemBase
{
Q_OBJECT
public:
    KReportItemLine();
    explicit KReportItemLine(const QDomNode & element);
    ~KReportItemLine() override;

    QString typeName() const override;
    int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                         const QVariant &data, KReportScriptHandler *script) override;

    void setUnit(const KReportUnit &u) override;

    QPointF startPosition() const;
    void setStartPosition(const QPointF &ptPos);
    QPointF endPosition() const;
    void setEndPosition(const QPointF &ptPos);

protected:
    KProperty *m_lineColor;
    KProperty *m_lineWeight;
    KProperty *m_lineStyle;

    KReportLineStyle lineStyle() const;
    qreal weight() const;
    void setWeight(qreal w);

private:
    KProperty *m_start;
    KProperty *m_end;

    void createProperties() override;

    friend class Scripting::Line;
};

#endif
