/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
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

#include "KoReportDesignerItemBase.h"

#include <QString>
#include <QDomDocument>
#include <QCheckBox>

#include "common/KoReportItemBase.h"
#include "KReportUtils.h"

//
// ReportEntity
//
KoReportDesignerItemBase::~KoReportDesignerItemBase()
{
}

KoReportDesignerItemBase::KoReportDesignerItemBase(KoReportDesigner* r)
{
    m_reportDesigner = r;
}

void KoReportDesignerItemBase::buildXML(QGraphicsItem * item, QDomDocument *doc, QDomElement *parent)
{
    KoReportDesignerItemBase *re = 0;
    re = dynamic_cast<KoReportDesignerItemBase*>(item);

    if (re) {
        re->buildXML(doc, parent);
    }

}

void KoReportDesignerItemBase::buildXMLRect(QDomDocument *doc, QDomElement *entity, KRPos *pos, KRSize *size)
{
    Q_UNUSED(doc);
    KReportUtils::buildXMLRect(entity, pos, size);
}

void KoReportDesignerItemBase::buildXMLTextStyle(QDomDocument *doc, QDomElement *entity, const KRTextStyleData &ts)
{
    KReportUtils::buildXMLTextStyle(doc, entity, ts);
}

void KoReportDesignerItemBase::buildXMLLineStyle(QDomDocument *doc, QDomElement *entity, const KRLineStyleData &ls)
{
    KReportUtils::buildXMLLineStyle(doc, entity, ls);
}

QString KoReportDesignerItemBase::dataSourceAndObjectTypeName(const QString& dataSource, const QString& objectTypeName) const
{
    return QString::fromLatin1("%1: %2").arg(dataSource).arg(objectTypeName);
}

// static
void KoReportDesignerItemBase::addPropertyAsAttribute(QDomElement* e, KProperty* p)
{
    KReportUtils::addPropertyAsAttribute(e, p);
}
