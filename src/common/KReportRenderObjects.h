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
#ifndef KREPORTRENDEROBJECTS_H
#define KREPORTRENDEROBJECTS_H

#include <QString>
#include <QList>
#include <QPointF>
#include <QSizeF>
#include <QFont>
#include <QImage>
#include <QPen>
#include <QBrush>
#include <QPicture>
#include <QPageLayout>

#include "KReportDataSource.h"
#include "KReportItemBase.h"
#include "KReportSectionData.h"
#include "KReportLineStyle.h"

class ORODocument;
class OROPage;
class OROPrimitive;
class OROTextBox;
class OROLine;
class OROImage;
class OROSection;

//
// ORODocument
// This object is a single document containing one or more OROPage elements
//
class KREPORT_EXPORT ORODocument : public QObject
{
    Q_OBJECT

public:
    explicit ORODocument(const QString &title = QString());
    ~ORODocument();

    QString title() const;
    void setTitle(const QString &title);

    
    /**
     * @brief Return the total number of pages in the document
     * 
     */
    int pageCount() const;
    
    /**
     * @brief Return a pointer to a given page
     * 
     * @param index page number to find
     * @return OROPage*
     */
    OROPage* page(int index);
    const OROPage* page(int index) const;
    
    /**
     * @brief Adds the supplied page to this document
     * 
     * Ownership of the page is tranferred the document
     * 
     * @param page an OROPage* to be added
     */
    void addPage(OROPage* page);
    
    /**
     * @brief Returns the index of the supplied page in the document
     * 
     * @param page OROPage* to find
     * @return int page index
     */
    int pageIndex(const OROPage* page) const;
    
    /**
     * @brief Removes the given page from the document
     * 
     * The page is also deleted
     * 
     * @param page OROPage* to delete
     */
    void removePage(OROPage* page);

    /**
     * @brief Takes the page from the document but does not delete it
     * 
     * @param page OROPage* to take from the document
     */
    void takePage(OROPage *page);
    
    /**
     * @brief Return the total number of sections in the document
     * 
     */
    int sectionCount() const;
    
    /**
     * @brief Return a pointer to a given section
     * 
     * @param index section number to find
     * @return OROSection*
     */
    OROSection* section(int index);
    const OROSection* section(int index) const;
    
    /**
     * @brief Adds the supplied sectin to the document
     * 
     * Ownership of the section is transferred to the document
     * 
     * @param section OROSection* to add to the document
     */
    void addSection(OROSection* section);
    
    /**
     * @brief Removes the supplied section from the document
     * 
     * The section will also be deleted
     * 
     * @param section OROSection* to remove and delete
     */
    void removeSection(OROSection *section);
    
    /**
     * @brief Takes the section from the document but does not delete it
     * 
     * @param page OROSection* to take from the document
     */
    void takeSection(OROSection *section);
    
    void setPageLayout(const QPageLayout &layout);
    QPageLayout pageLayout() const;

    void notifyChange(int pageNo);
    
Q_SIGNALS:
    void updated(int pageNo);
    
private:
    class Private;
    Private * const d;
};

//
// OROPage
// This object is a single page in a document and may contain zero or more
// OROPrimitive objects all of which represent some form of mark to made on
// a page.
//
class KREPORT_EXPORT OROPage
{
public:
    explicit OROPage(ORODocument * doc = 0);
    ~OROPage();

    ORODocument* document();
    const ORODocument* document() const;
    void setDocument(ORODocument *doc);
    
    int pageNumber() const; // returns this pages current page number

    int primitiveCount() const;
    
    OROPrimitive* primitive(int index);
    const OROPrimitive* primitive(int index) const;
    
    void insertPrimitive(OROPrimitive* primitive, int index = -1);
    void removePrimitive(OROPrimitive *primitive);
    void takePrimitive(OROPrimitive *primitive);

private:
    class Private;
    Private * const d;
};
//
// OROSection
// This object is a single row in a document and may contain zero or more
// OROPrimitives
//
class KREPORT_EXPORT OROSection
{
public:
    explicit OROSection(ORODocument* doc = 0);
    ~OROSection();

    void setHeight(int);
    int height() const;

    void setBackgroundColor(const QColor& color);
    QColor backgroundColor() const;

    ORODocument* document();
    const ORODocument* document() const;
    void setDocument(ORODocument *doc);

    void setType(KReportSectionData::Section t);
    KReportSectionData::Section type() const;

    int primitiveCount() const;
    OROPrimitive* primitive(int index);
    const OROPrimitive* primitive(int index) const;
    void addPrimitive(OROPrimitive* primitive);
    void sortPrimitives(Qt::Orientation orientation);

private:
    class Private;
    Private * const d;
};


//
// OROPrimitive
// This object represents the basic primitive with a position and type.
// Other primitives are subclasses with a defined type and any additional
// information they require to define that primitive.
//
class KREPORT_EXPORT OROPrimitive
{
public:
    virtual ~OROPrimitive();

    OROPage* page();
    const OROPage* page() const;
    void setPage(OROPage *page);

    QPointF position() const;
    void setPosition(const QPointF &pos);
    
    QSizeF size() const;
    void setSize(const QSizeF &s);

    virtual OROPrimitive* clone() const = 0;
    
protected:
    OROPrimitive();
    
private:
    class Private;
    Private * const d;
};

//
// OROTextBox
// This is a text box primitive it defines a box region and text that will
// be rendered inside that region. It also contains information for font
// and positioning of the text.
//
class KREPORT_EXPORT OROTextBox : public OROPrimitive
{
public:
    OROTextBox();
    virtual ~OROTextBox();

    QString text() const;
    void setText(const QString &text);

    KReportTextStyleData textStyle() const;
    void setTextStyle(const KReportTextStyleData&);

    KReportLineStyle lineStyle() const;
    void setLineStyle(const KReportLineStyle&);

    void setFont(const QFont &font);

    int flags() const;
    void setFlags(int flags);

    virtual OROPrimitive* clone() const;

    bool requiresPostProcessing() const;
    void setRequiresPostProcessing(bool pp);

    bool wordWrap() const;
    void setWordWrap(bool ww);

    bool canGrow() const;
    void setCanGrow(bool grow);
    
private:
    class Private;
    Private * const d;
};

//
// OROLine
// This primitive defines a line with a width/weight.
//
class KREPORT_EXPORT OROLine : public OROPrimitive
{
public:
    OROLine();
    virtual ~OROLine();

    QPointF startPoint() const {
        return position();
    };
    void setStartPoint(const QPointF &start);

    QPointF endPoint() const;
    void setEndPoint(const QPointF &end);

    KReportLineStyle lineStyle() const;
    void setLineStyle(const KReportLineStyle& style);

    virtual OROPrimitive* clone() const;
    
private:
    class Private;
    Private * const d;
};

//
// OROImage
// This primitive defines an image
//
class KREPORT_EXPORT OROImage: public OROPrimitive
{
public:
    OROImage();
    virtual ~OROImage();

    QImage image() const;
    void setImage(const QImage &img);

    bool isScaled() const;
    void setScaled(bool scaled);

    Qt::TransformationMode transformationMode() const;
    void setTransformationMode(Qt::TransformationMode transformation);

    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode aspect);

    virtual OROPrimitive* clone() const;
    
private:
    class Private;
    Private * const d;
};

//
// OROPicture
// This primitive defines a picture
//
class KREPORT_EXPORT OROPicture: public OROPrimitive
{
public:
    OROPicture();
    virtual ~OROPicture();

    void setPicture(const QPicture& pic);
    QPicture* picture();

    virtual OROPrimitive* clone() const;
    
private:
    class Private;
    Private * const d;

};
//
// ORORect
// This primitive defines a drawn rectangle
//
class KREPORT_EXPORT ORORect: public OROPrimitive
{
public:
    ORORect();
    virtual ~ORORect();

    QRectF rect() const;
    void setRect(const QRectF &rectangle);

    QPen pen() const;
    void setPen(const QPen &pen);

    QBrush brush() const;
    void setBrush(const QBrush &brush);

    virtual OROPrimitive* clone() const;
    
private:
    class Private;
    Private * const d;
};

//
// ORORect
// This primitive defines a drawn rectangle
//
class KREPORT_EXPORT OROEllipse: public OROPrimitive
{
public:
    OROEllipse();
    virtual ~OROEllipse();

    QRectF rect() const;
    void setRect(const QRectF &rectangle);

    QPen pen() const;
    void setPen(const QPen &pen);

    QBrush brush() const;
    void setBrush(const QBrush &brush);

    virtual OROPrimitive* clone() const;
    
private:
    class Private;
    Private * const d;
};

class KREPORT_EXPORT OROCheckBox : public OROPrimitive
{
public:
    enum Type {
        Cross = 1,
        Tick,
        Dot
    };
    
    OROCheckBox();
    virtual ~OROCheckBox();
    virtual OROPrimitive* clone() const;

    void setCheckType(Type type);
    Type checkType() const;

    void setValue(bool val);
    bool value() const;

    void setLineStyle(const KReportLineStyle& ls);
    KReportLineStyle lineStyle() const;
    
    void setForegroundColor(const QColor& fg);
    QColor foregroundColor() const;

private:
    class Private;
    Private * const d;

};

#endif // __RENDEROBJECTS_H__
