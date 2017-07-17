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

/*
 *     This file contains all the Report Entity classes. Each one is a
 * derivative of ReportEntity, which in turn is derived from QCanvasItem.
 */

#ifndef KREPORTDESIGNERITEMBASE_H
#define KREPORTDESIGNERITEMBASE_H

#include <QGraphicsItem>

#include "KReportItemBase.h"

class QDomDocument;
class QDomElement;

class KReportDesigner;

//
// ReportEntity
//
class KREPORT_EXPORT KReportDesignerItemBase
{
public:
    virtual ~KReportDesignerItemBase();

    static void buildXML(QGraphicsItem * item, QDomDocument *doc, QDomElement *parent);
    virtual void buildXML(QDomDocument *doc, QDomElement *parent) = 0;

    static void buildXMLRect(QDomDocument *doc, QDomElement *entity, KReportItemBase *i);
    static void buildXMLTextStyle(QDomDocument *doc, QDomElement *entity, const KReportTextStyleData &ts);
    static void buildXMLLineStyle(QDomDocument *doc, QDomElement *entity, const KReportLineStyle &ls);

    virtual KReportDesignerItemBase* clone() = 0;
    virtual void move(const QPointF&) = 0;

    KReportDesigner* designer() const;
    void setDesigner(KReportDesigner* rd);

    static void addPropertyAsAttribute(QDomElement* e, KProperty* p);

protected:
    explicit KReportDesignerItemBase(KReportDesigner *r, KReportItemBase *);
    QString dataSourceAndObjectTypeName(const QString &dataSource,
                                        const QString &objectTypeName) const;

    /**
     * @brief Updates the text that is shown for the item in the report designer
     * If itemDataSource is set then it is preferred over itemStaticValue
     * itemType is appended to the end of the text
     *
     * @param itemDataSource source field property
     * @param itemStaticValue value property
     * @param itemType type of item
     * @return void
     */
    void updateRenderText(const QString &itemDataSource, const QString &itemStaticValue, const QString &itemType);    
    KReportItemBase *item() const;
    
    void setRenderText(const QString &text);
    QString renderText() const;

private:
    Q_DISABLE_COPY(KReportDesignerItemBase)
    class Private;
    Private * const d;
};

#endif

