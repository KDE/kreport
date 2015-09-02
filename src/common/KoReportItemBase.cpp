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

#include "KoReportItemBase.h"

#include "krpos.h"
#include "krsize.h"
#include "KReportUtils.h"
#include <KPropertySet>

KoReportItemBase::KoReportItemBase()
{
    Z = 0;
    m_name = new KProperty("name", QString(), tr("Name"), tr("Object Name"));
    m_name->setAutoSync(0);
}

KoReportItemBase::~KoReportItemBase() { }

void KoReportItemBase::addDefaultProperties()
{
    m_set->addProperty(m_name);
    m_set->addProperty(m_pos.property());
    m_set->addProperty(m_size.property());
}

bool KoReportItemBase::parseReportTextStyleData(const QDomElement & elemSource, KRTextStyleData *ts)
{
    return KReportUtils::parseReportTextStyleData(elemSource, ts);
}

bool KoReportItemBase::parseReportLineStyleData(const QDomElement & elemSource, KRLineStyleData *ls)
{
    return KReportUtils::parseReportLineStyleData(elemSource, ls);
}


bool KoReportItemBase::parseReportRect(const QDomElement & elemSource, KRPos *pos, KRSize *size)
{
    return KReportUtils::parseReportRect(elemSource, pos, size);
}

void KoReportItemBase::setUnit(const KReportUnit& u)
{
    m_pos.setUnit(u);
    m_size.setUnit(u);
}

int KoReportItemBase::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                       const QVariant &data, KRScriptHandler* script)
{
    Q_UNUSED(page)
    Q_UNUSED(section)
    Q_UNUSED(offset)
    Q_UNUSED(data)
    Q_UNUSED(script)
    return 0;
}

int KoReportItemBase::renderReportData(OROPage *page, OROSection *section, const QPointF &offset,
                                       KoReportData *data, KRScriptHandler* script)
{
    Q_UNUSED(page)
    Q_UNUSED(section)
    Q_UNUSED(offset)
    Q_UNUSED(data)
    Q_UNUSED(script)
    return 0;
}

QString KoReportItemBase::itemDataSource() const
{
    return QString();
}

KPropertySet* KoReportItemBase::propertySet() const
{
 return m_set;
}

bool KoReportItemBase::supportsSubQuery() const
{
    return false;
}

QString KoReportItemBase::entityName() const
{
    return m_name->value().toString();
}

void KoReportItemBase::setEntityName(const QString& n)
{
    m_name->setValue(n);
}

KRPos KoReportItemBase::position() const
{
    return m_pos;
}

KRSize KoReportItemBase::size() const
{
    return m_size;
}

