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
#include "KReportRenderObjects.h"
#include "KReportUtils_p.h"

#include "kreport_debug.h"

// Helper functions
static bool xLessThan(OROPrimitive* s1, OROPrimitive* s2)
{
    return s1->position().x() < s2->position().x();
}

//
// ORODocument
//

class Q_DECL_HIDDEN ORODocument::Private
{
public:
    Private();
    ~Private();
    QString title;
    QList<OROPage*> pages;
    QList<OROSection*> sections;
    KReportPrivate::PageLayout pageLayout;
};

ORODocument::Private::Private()
{

}

ORODocument::Private::~Private()
{
    qDeleteAll(pages);
    qDeleteAll(sections);
}


ORODocument::ORODocument(const QString& title) : d(new Private())
{
    d->title = title;
}

ORODocument::~ORODocument()
{
    delete d;
}

void ORODocument::setTitle(const QString &title)
{
    d->title = title;
}

OROPage* ORODocument::page(int index)
{
    return d->pages.value(index);
}

const OROPage * ORODocument::page(int index) const
{
    return d->pages.value(index);
}

void ORODocument::addPage(OROPage* p)
{
    if (p == 0) {
        return;
    }

    if (p->document() != 0 && p->document() != this) {
        return;
    }

    p->setDocument(this);
    d->pages.append(p);
}

OROSection* ORODocument::section(int pnum)
{
    return d->sections.value(pnum);
}

const OROSection * ORODocument::section(int index) const
{
    return d->sections.value(index);
}


void ORODocument::addSection(OROSection* s)
{
    if (s == 0)
        return;

    if (s->document() != 0 && s->document() != this) {
        return;
    }
    s->setDocument(this);
    d->sections.append(s);
}

void ORODocument::setPageLayout(const QPageLayout & options)
{
    d->pageLayout = options;
}

void ORODocument::notifyChange(int pageNo)
{
    emit(updated(pageNo));
}

QPageLayout ORODocument::pageLayout() const
{
    return d->pageLayout;
}

int ORODocument::pageCount() const
{
    return d->pages.count();
}

int ORODocument::sectionCount() const
{
    return d->sections.count();
}

QString ORODocument::title() const
{
    return d->title;
}

void ORODocument::removePage(OROPage* page)
{
    d->pages.removeOne(page);
    delete page;
}

void ORODocument::takePage(OROPage* page)
{
    d->pages.removeOne(page);
}

void ORODocument::removeSection(OROSection* section)
{
    d->sections.removeOne(section);
    delete section;
}

void ORODocument::takeSection(OROSection* section)
{
    d->sections.removeOne(section);
}

int ORODocument::pageIndex(const OROPage* page) const
{
    return d->pages.indexOf(const_cast<OROPage*>(page));
}

//
// OROPage
//
class Q_DECL_HIDDEN OROPage::Private
{
public:
    Private();
    ~Private();
    ORODocument *document;
    QList<OROPrimitive*> primitives;
};

OROPage::Private::Private()
{
}

OROPage::Private::~Private()
{
    qDeleteAll(primitives);
}

OROPage::OROPage(ORODocument * pDocument)
        : d(new Private())
{
    d->document = pDocument;
}

OROPage::~OROPage()
{
    if (d->document) {
        d->document->takePage(this);
    }
    delete d;
}

int OROPage::pageNumber() const
{
    if (d->document) {
        return d->document->pageIndex(this);
    }
    return -1;
}

OROPrimitive* OROPage::primitive(int index)
{
    return d->primitives.value(index);
}

const OROPrimitive * OROPage::primitive(int index) const
{
    return d->primitives.value(index);
}


void OROPage::insertPrimitive(OROPrimitive* p, int index)
{
    //kreportDebug() << "Adding primitive" << p->type() << "to page" << page();
    if (p == 0)
        return;

    p->setPage(this);
    if (index == -1) {
        d->primitives.append(p);
    } else {
        d->primitives.insert(index, p);
    }

#if 0
//TODO
    if (notify) {
        if (d->document) {
            d->document->notifyChange(pageNumber());
        }
    }
#endif
}

ORODocument * OROPage::document()
{
    return d->document;
}

const ORODocument * OROPage::document() const
{
    return d->document;
}

int OROPage::primitiveCount() const
{
    return d->primitives.count();
}

void OROPage::setDocument(ORODocument* doc)
{
    d->document = doc;
}

void OROPage::removePrimitive(OROPrimitive* primitive)
{
    d->primitives.removeOne(primitive);
    delete primitive;
}

void OROPage::takePrimitive(OROPrimitive* primitive)
{
    d->primitives.removeOne(primitive);
}

//
// OROSection
//

class Q_DECL_HIDDEN OROSection::Private
{
public:
    Private();
    ~Private();
    ORODocument * document;
    QList<OROPrimitive*> primitives;
    qint64 row = 0;
    int height = 0;
    KReportSectionData::Section type = KReportSectionData::None;
    QColor backgroundColor = Qt::white;
};

OROSection::Private::Private()
{

}

OROSection::Private::~Private()
{
    qDeleteAll(primitives);
    primitives.clear();
}

OROSection::OROSection(ORODocument* doc) : d(new Private())
{
    d->document = doc;
}

OROSection::~OROSection()
{
    if (d->document) {
        d->document->takeSection(this);
    }

    delete d;
}

OROPrimitive* OROSection::primitive(int index)
{
    return d->primitives.value(index);
}

const OROPrimitive * OROSection::primitive(int index) const
{
    return d->primitives.value(index);
}

void OROSection::addPrimitive(OROPrimitive* primitive)
{
    if (primitive == 0)
        return;

    d->primitives.append(primitive);
}

void OROSection::setHeight(int h)
{
    d->height = h;
}

int OROSection::height() const
{
    return d->height;
}

void OROSection::setBackgroundColor(const QColor& color)
{
    d->backgroundColor = color;
}

QColor OROSection::backgroundColor() const
{
    return d->backgroundColor;
}

void OROSection::sortPrimitives(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal) {
        qSort(d->primitives.begin(), d->primitives.end(), xLessThan);
    }
}

ORODocument * OROSection::document()
{
    return d->document;
}

const ORODocument * OROSection::document() const
{
    return d->document;
}


int OROSection::primitiveCount() const
{
    return d->primitives.count();
}

void OROSection::setType(KReportSectionData::Section t)
{
    d->type = t;
}

KReportSectionData::Section OROSection::type() const
{
    return d->type;
}

void OROSection::setDocument(ORODocument* doc)
{
    d->document = doc;
}

//
// OROPrimitive
//

class Q_DECL_HIDDEN OROPrimitive::Private
{
public:
    OROPage * page = nullptr;
    QPointF position;
    QSizeF size;
};

OROPrimitive::OROPrimitive()
        : d(new Private())
{
}

OROPrimitive::~OROPrimitive()
{
   if (d->page) {
        d->page->takePrimitive(this);
    }

    delete d;
}

void OROPrimitive::setPosition(const QPointF& pos)
{
    d->position = pos;
}

void OROPrimitive::setSize(const QSizeF & s)
{
    d->size = s;
}

OROPage * OROPrimitive::page()
{
    return d->page;
}

const OROPage * OROPrimitive::page() const
{
    return d->page;
}

QPointF OROPrimitive::position() const
{
    return d->position;
}

QSizeF OROPrimitive::size() const
{
    return d->size;
}

void OROPrimitive::setPage(OROPage* page)
{
    d->page = page;
}

//
// OROTextBox
//

class Q_DECL_HIDDEN OROTextBox::Private
{
public:
    Private();
    ~Private();
    QString text;
    KReportTextStyleData textStyle;
    KReportLineStyle lineStyle;
    Qt::Alignment alignment;
    int flags; // Qt::AlignmentFlag and Qt::TextFlag OR'd
    bool wordWrap;
    bool canGrow;
    bool requiresPostProcessing;

};

OROTextBox::Private::Private()
{
    flags = 0;
    wordWrap = false;
    canGrow = false;
    requiresPostProcessing = false;

    lineStyle.setColor(Qt::black);
    lineStyle.setWeight(0);
    lineStyle.setPenStyle(Qt::NoPen);
}

OROTextBox::Private::~Private()
{
}

OROTextBox::OROTextBox() : d(new Private())
{

}

OROTextBox::~OROTextBox()
{
    delete d;
}

void OROTextBox::setText(const QString & s)
{
    d->text = s;
}

void OROTextBox::setTextStyle(const KReportTextStyleData & ts)
{
    d->textStyle = ts;
}

void OROTextBox::setLineStyle(const KReportLineStyle & ls)
{
    d->lineStyle = ls;
}

void OROTextBox::setFont(const QFont & f)
{
    d->textStyle.font = f;
}

void OROTextBox::setFlags(int f)
{
    d->flags = f;
}

bool OROTextBox::canGrow() const
{
    return d->canGrow;
}

int OROTextBox::flags() const
{
    return d->flags;
}

KReportLineStyle OROTextBox::lineStyle() const
{
    return d->lineStyle;
}

bool OROTextBox::requiresPostProcessing() const
{
    return d->requiresPostProcessing;
}

void OROTextBox::setCanGrow(bool grow)
{
    d->canGrow = grow;
}

void OROTextBox::setRequiresPostProcessing(bool pp)
{
    d->requiresPostProcessing = pp;
}

void OROTextBox::setWordWrap(bool ww)
{
    d->wordWrap = ww;
}

QString OROTextBox::text() const
{
    return d->text;
}

KReportTextStyleData OROTextBox::textStyle() const
{
    return d->textStyle;
}

bool OROTextBox::wordWrap() const
{
    return d->wordWrap;
}

OROPrimitive* OROTextBox::clone() const
{
    OROTextBox *theClone = new OROTextBox();
    theClone->setSize(size());
    theClone->setPosition(position());
    theClone->setText(text());
    theClone->setTextStyle(textStyle());
    theClone->setLineStyle(lineStyle());
    theClone->setFlags(flags());
    theClone->setCanGrow(canGrow());
    theClone->setWordWrap(wordWrap());
    theClone->setRequiresPostProcessing(requiresPostProcessing());
    return theClone;
}


//
// OROLine
//

class Q_DECL_HIDDEN OROLine::Private
{
public:
    QPointF endPoint;
    KReportLineStyle lineStyle;
};

OROLine::OROLine() : d(new Private())
{

}

OROLine::~OROLine()
{
    delete d;
}

void OROLine::setStartPoint(const QPointF & p)
{
    setPosition(p);
}

void OROLine::setEndPoint(const QPointF & p)
{
    d->endPoint = p;
}

void OROLine::setLineStyle(const KReportLineStyle& style)
{
    d->lineStyle = style;
}

QPointF OROLine::endPoint() const
{
    return d->endPoint;
}

KReportLineStyle OROLine::lineStyle() const
{
    return d->lineStyle;
}

OROPrimitive* OROLine::clone() const
{
    OROLine *theClone = new OROLine();
    theClone->setStartPoint(position());
    theClone->setEndPoint(endPoint());
    theClone->setLineStyle(lineStyle());
    return theClone;
}

//
// OROImage
//

class Q_DECL_HIDDEN OROImage::Private
{
public:
    QImage image;
    bool scaled;
    Qt::TransformationMode transformFlags;
    Qt::AspectRatioMode aspectFlags;
};

OROImage::OROImage() : d(new Private())
{
    d->scaled = false;
    d->transformFlags = Qt::FastTransformation;
    d->aspectFlags = Qt::IgnoreAspectRatio;
}

OROImage::~OROImage()
{
    delete d;
}

void OROImage::setImage(const QImage & img)
{
    d->image = img;
}

void OROImage::setScaled(bool b)
{
    d->scaled = b;
}

void OROImage::setTransformationMode(Qt::TransformationMode transformation)
{
    d->transformFlags = transformation;
}

void OROImage::setAspectRatioMode(Qt::AspectRatioMode aspectRatioMode)
{
    d->aspectFlags = aspectRatioMode;
}

Qt::AspectRatioMode OROImage::aspectRatioMode() const
{
    return d->aspectFlags;
}

QImage OROImage::image() const
{
    return d->image;
}

bool OROImage::isScaled() const
{
    return d->scaled;
}

Qt::TransformationMode OROImage::transformationMode() const
{
    return d->transformFlags;
}

OROPrimitive* OROImage::clone() const
{
    OROImage *theClone = new OROImage();
    theClone->setSize(size());
    theClone->setPosition(position());
    theClone->setImage(image());
    theClone->setScaled(isScaled());
    theClone->setTransformationMode(transformationMode());
    theClone->setAspectRatioMode(aspectRatioMode());
    return theClone;
}

//
// OROPicture
//

class Q_DECL_HIDDEN OROPicture::Private
{
public:
    QPicture picture;
};

OROPicture::OROPicture() : d(new Private())
{

}

OROPicture::~OROPicture()
{
    delete d;
}

OROPrimitive* OROPicture::clone() const
{
    OROPicture *theClone = new OROPicture();
    theClone->setSize(size());
    theClone->setPosition(position());
//     theClone->setPicture(*(picture()ddddddds));
    return theClone;
}

QPicture* OROPicture::picture()
{
    return &d->picture;
}

void OROPicture::setPicture(const QPicture& p)
{
    d->picture = p;
}

//
// ORORect
//

class Q_DECL_HIDDEN ORORect::Private
{
public:
    QPen pen;
    QBrush brush;
};

ORORect::ORORect() : d(new Private())
{
}

ORORect::~ORORect()
{
    delete d;
}

void ORORect::setRect(const QRectF & r)
{
    setPosition(r.topLeft());
    setSize(r.size());
}

void ORORect::setPen(const QPen & p)
{
    d->pen = p;
}

void ORORect::setBrush(const QBrush & b)
{
    d->brush = b;
}

QBrush ORORect::brush() const
{
    return d->brush;
}

QPen ORORect::pen() const
{
    return d->pen;
}

QRectF ORORect::rect() const
{
    return QRectF(position(), size());
}

OROPrimitive* ORORect::clone() const
{
    ORORect *theClone = new ORORect();
    theClone->setSize(size());
    theClone->setPosition(position());
    theClone->setPen(pen());
    theClone->setBrush(brush());
    return theClone;
}
//
// OROEllipse
//

class Q_DECL_HIDDEN OROEllipse::Private
{
public:
    QPen pen;
    QBrush brush;
};

OROEllipse::OROEllipse() : d(new Private())
{
}

OROEllipse::~OROEllipse()
{
    delete d;
}

void OROEllipse::setRect(const QRectF & r)
{
    setPosition(r.topLeft());
    setSize(r.size());
}

void OROEllipse::setPen(const QPen & p)
{
    d->pen = p;
}

void OROEllipse::setBrush(const QBrush & b)
{
    d->brush = b;
}

QBrush OROEllipse::brush() const
{
    return d->brush;
}

QPen OROEllipse::pen() const
{
    return d->pen;
}

QRectF OROEllipse::rect() const
{
    return QRectF(position(), size());
}

OROPrimitive* OROEllipse::clone() const
{
    OROEllipse *theClone = new OROEllipse();
    theClone->setSize(size());
    theClone->setPosition(position());
    theClone->setPen(pen());
    theClone->setBrush(brush());
    return theClone;
}

//
// OROCheck
//

class Q_DECL_HIDDEN OROCheckBox::Private
{
public:
    OROCheckBox::Type checkType;
    bool value;
    KReportLineStyle lineStyle;
    QColor foregroundColor;
};

OROCheckBox::OROCheckBox() : d(new Private())
{
    d->value = false;
}

OROCheckBox::~OROCheckBox()
{
    delete d;
}

OROCheckBox::Type OROCheckBox::checkType() const
{
    return d->checkType;
}

QColor OROCheckBox::foregroundColor() const
{
    return d->foregroundColor;
}

KReportLineStyle OROCheckBox::lineStyle() const
{
    return d->lineStyle;
}

void OROCheckBox::setForegroundColor(const QColor& fg)
{
    d->foregroundColor = fg;
}

void OROCheckBox::setLineStyle(const KReportLineStyle& ls)
{
    d->lineStyle = ls;
}

void OROCheckBox::setValue(bool v)
{
    d->value = v;
}

bool OROCheckBox::value() const
{
    return d->value;
}

void OROCheckBox::setCheckType(Type type)
{
    if (type == Cross || type == Tick || type == Dot) {
        d->checkType = type;
    } else {
        d->checkType = Cross;
    }
}

OROPrimitive* OROCheckBox::clone() const
{
    OROCheckBox *theClone = new OROCheckBox();
    theClone->setSize(size());
    theClone->setPosition(position());
    theClone->setLineStyle(lineStyle());
    theClone->setForegroundColor(foregroundColor());
    theClone->setValue(value());
    theClone->setCheckType(checkType());
    return theClone;
}
