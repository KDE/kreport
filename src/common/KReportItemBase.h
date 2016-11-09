/* This file is part of the KDE project
 * Copyright (C) 2007-2010 by Adam Pigg (adam@piggz.co.uk)
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

#ifndef KREPORTITEMBASE_H
#define KREPORTITEMBASE_H

#include "config-kreport.h"
#include "kreport_export.h"
#include "KReportPosition.h"
#include "KReportSize.h"
#include "KReportDpi.h"

#include <QObject>
#include <QFont>
#include <QColor>

class OROPage;
class OROSection;
class KReportSize;
class KReportData;
class KReportLineStyle;

#ifdef KREPORT_SCRIPTING
class KReportScriptHandler;
#else
#define KReportScriptHandler void
#endif

class KProperty;
class KPropertySet;

class QDomElement;

class KRTextStyleData
{
public:
    QFont font;
    Qt::Alignment alignment;
    QColor backgroundColor;
    QColor foregroundColor;
    int backgroundOpacity;

};

/*
class KReportLineStyle
{
public:
    int weight;
    QColor lineColor;
    Qt::PenStyle style;
};
*/

/**
*/
class KREPORT_EXPORT KReportItemBase : public QObject
{
    Q_OBJECT
public:

    KReportItemBase();
    virtual ~KReportItemBase();

    /**
    @brief Return the item type as a string.  Required by all items
    @return Item type
    */
    virtual QString typeName() const = 0;

    /**
    @brief Render the item into a primitive which is used by the second stage renderer
    @return the height required by the object
    */
    virtual int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset, const QVariant &data, KReportScriptHandler *script);

    /**
    @brief Render a complex item that uses a sub query as a data source
    @return the height required by the object
    */
    virtual int renderReportData(OROPage *page, OROSection *section, const QPointF &offset, KReportData *data, KReportScriptHandler *script);

    /**
    @brief Override if the item supports a simple data source, such as a field
    @return The field name or expression for the data source
    */
    virtual QString itemDataSource() const;

    /**
    @brief Override if the item uses a sub query and linked fields, such as a chart or sub-report
    @return True if uses a sub query
    */
    virtual bool supportsSubQuery() const;
 
    KPropertySet* propertySet();
    const KPropertySet* propertySet() const;
    
    void setEntityName(const QString& n);
    QString entityName() const;

    virtual void setUnit(const KReportUnit& u);

    /**
     * @brief Return the size in points
     */
    QSizeF size() const;
    
    /**
     * @brief Return the position in points
     */
    QPointF position() const;

    void setPosition(const QPointF &pos);
    void setSize(const QSizeF &siz);
    
    qreal z() const;
    void setZ(qreal z);
    
    //Helper function to map between size/position units
    static QPointF scenePosition(const QPointF &pos);
    static QSizeF sceneSize(const QSizeF &size);
    static QPointF positionFromScene(const QPointF &pos);
    static QSizeF sizeFromScene(const QSizeF &size);

protected:
    virtual void createProperties() = 0;
    bool parseReportRect(const QDomElement &elem);
    static bool parseReportTextStyleData(const QDomElement &, KRTextStyleData*);
    static bool parseReportLineStyleData(const QDomElement &, KReportLineStyle*);
    
    KProperty *nameProperty();
    QString oldName() const;
    void setOldName(const QString &old);
    
    Q_SLOT virtual void propertyChanged(KPropertySet &s, KProperty &p);
    
private:
    class Private;
    Private * const d;
};

#endif
