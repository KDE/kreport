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

#include "KReportDesignerItemBase.h"
#include "KReportItemBase.h"
#include "KReportUtils.h"

#include <QString>
#include <QDomDocument>
#include <QCheckBox>

class Q_DECL_HIDDEN KReportDesignerItemBase::Private
{
public:
    Private();
    ~Private();
    
    KReportDesigner *reportDesigner;
    KReportItemBase *item;
    QString renderText;   
};

KReportDesignerItemBase::Private::Private()
{
}

KReportDesignerItemBase::Private::~Private()
{
}

KReportDesignerItemBase::~KReportDesignerItemBase()
{
    delete d;
}

KReportDesignerItemBase::KReportDesignerItemBase(KReportDesigner *r, KReportItemBase *b) : d(new Private())
{
    d->reportDesigner = r;
    d->item = b;
}

void KReportDesignerItemBase::buildXML(QGraphicsItem * item, QDomDocument *doc, QDomElement *parent)
{
    KReportDesignerItemBase *re = 0;
    re = dynamic_cast<KReportDesignerItemBase*>(item);

    if (re) {
        re->buildXML(doc, parent);
    }

}

void KReportDesignerItemBase::buildXMLRect(QDomDocument *doc, QDomElement *entity, KReportItemBase *i)
{
    Q_UNUSED(doc);
    KReportUtils::buildXMLRect(entity, i->position(), i->size());
}

void KReportDesignerItemBase::buildXMLTextStyle(QDomDocument *doc, QDomElement *entity, const KRTextStyleData &ts)
{
    KReportUtils::buildXMLTextStyle(doc, entity, ts);
}

void KReportDesignerItemBase::buildXMLLineStyle(QDomDocument *doc, QDomElement *entity, const KReportLineStyle &ls)
{
    KReportUtils::buildXMLLineStyle(doc, entity, ls);
}

QString KReportDesignerItemBase::dataSourceAndObjectTypeName(const QString& dataSource, const QString& objectTypeName) const
{
    return QString::fromLatin1("%1: %2").arg(dataSource).arg(objectTypeName);
}

// static
void KReportDesignerItemBase::addPropertyAsAttribute(QDomElement* e, KProperty* p)
{
    KReportUtils::addPropertyAsAttribute(e, p);
}

KReportDesigner * KReportDesignerItemBase::designer() const
{
    return d->reportDesigner;
}

void KReportDesignerItemBase::setDesigner(KReportDesigner* rd)
{
    d->reportDesigner = rd;
}

KReportItemBase *KReportDesignerItemBase::item() const
{
    return d->item;
}

QString KReportDesignerItemBase::renderText() const
{
    return d->renderText;
}

void KReportDesignerItemBase::setRenderText(const QString& text)
{
    d->renderText = text;
}
