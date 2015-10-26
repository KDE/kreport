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

#include "KReportItemBase.h"

#include "KReportPosition.h"
#include "KReportSize.h"
#include "KReportUtils.h"
#include <KPropertySet>

KReportItemBase::KReportItemBase()
{
    Z = 0;
    m_name = new KProperty("name", QString(), tr("Name"), tr("Object Name"));
    m_name->setAutoSync(0);
}

KReportItemBase::~KReportItemBase() { }

void KReportItemBase::addDefaultProperties()
{
    m_set->addProperty(m_name);
    m_set->addProperty(m_pos.property());
    m_set->addProperty(m_size.property());
}

bool KReportItemBase::parseReportTextStyleData(const QDomElement & elemSource, KRTextStyleData *ts)
{
    return KReportUtils::parseReportTextStyleData(elemSource, ts);
}

bool KReportItemBase::parseReportLineStyleData(const QDomElement & elemSource, KReportLineStyle *ls)
{
    return KReportUtils::parseReportLineStyleData(elemSource, ls);
}


bool KReportItemBase::parseReportRect(const QDomElement & elemSource, KReportPosition *pos, KReportSize *size)
{
    return KReportUtils::parseReportRect(elemSource, pos, size);
}

void KReportItemBase::setUnit(const KReportUnit& u)
{
    m_pos.setUnit(u);
    m_size.setUnit(u);
}

int KReportItemBase::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                       const QVariant &data, KReportScriptHandler* script)
{
    Q_UNUSED(page)
    Q_UNUSED(section)
    Q_UNUSED(offset)
    Q_UNUSED(data)
    Q_UNUSED(script)
    return 0;
}

int KReportItemBase::renderReportData(OROPage *page, OROSection *section, const QPointF &offset,
                                       KReportData *data, KReportScriptHandler* script)
{
    Q_UNUSED(page)
    Q_UNUSED(section)
    Q_UNUSED(offset)
    Q_UNUSED(data)
    Q_UNUSED(script)
    return 0;
}

QString KReportItemBase::itemDataSource() const
{
    return QString();
}

KPropertySet* KReportItemBase::propertySet() const
{
 return m_set;
}

bool KReportItemBase::supportsSubQuery() const
{
    return false;
}

QString KReportItemBase::entityName() const
{
    return m_name->value().toString();
}

void KReportItemBase::setEntityName(const QString& n)
{
    m_name->setValue(n);
}

KReportPosition KReportItemBase::position() const
{
    return m_pos;
}

KReportSize KReportItemBase::size() const
{
    return m_size;
}

