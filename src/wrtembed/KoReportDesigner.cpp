/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC <info@openmfg.com>
 * Copyright (C) 2007-2010 by Adam Pigg <adam@piggz.co.uk>
 * Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
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

#include "KoReportDesigner.h"
#include "reportsection.h"
#include "reportscene.h"
#include "reportsceneview.h"
#include "reportsectiondetailgroup.h"
#include "reportpropertiesbutton.h"
#include "sectioneditor.h"
#include "reportsectiondetail.h"
#include "KoReportDesignerItemLine.h"
#include "KReportRuler_p.h"
#include "KReportZoomHandler.h"
#include "common/KReportPageFormat.h"
#include "KReportDpi.h"
#include "common/krutils.h"
#include "common/KoReportPluginInterface.h"
#include "common/KoReportPluginManager.h"


#include <kross/core/manager.h>

#include <kstandardaction.h>
#include <KStandardGuiItem>
#include <QLayout>
#include <QDomDocument>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPointer>
#include <QIcon>
#include <QAction>
#include <QMouseEvent>
#include "kreport_debug.h"

//! Also add public method for runtime?
const char ns[] = "http://kexi-project.org/report/2.0";

static QDomElement propertyToElement(QDomDocument* d, KProperty* p)
{
    QDomElement e = d->createElement(QLatin1String("report:" + p->name().toLower()));
    e.appendChild(d->createTextNode(p->value().toString()));
    return e;
}

//
// define and implement the ReportWriterSectionData class
// a simple class to hold/hide data in the ReportHandler class
//
class ReportWriterSectionData
{
public:
    ReportWriterSectionData() {
        selected_x_offset = 0;
        selected_y_offset = 0;
        mouseAction = ReportWriterSectionData::MA_None;
    }
    virtual ~ReportWriterSectionData() {
    }

    enum MouseAction {
        MA_None = 0,
        MA_Insert = 1,
        MA_Grab = 2,
        MA_MoveStartPoint,
        MA_MoveEndPoint,
        MA_ResizeNW = 8,
        MA_ResizeN,
        MA_ResizeNE,
        MA_ResizeE,
        MA_ResizeSE,
        MA_ResizeS,
        MA_ResizeSW,
        MA_ResizeW
    };

    int selected_x_offset;
    int selected_y_offset;

    MouseAction mouseAction;
    QString insertItem;

    QList<KoReportDesignerItemBase*> copy_list;
    QList<KoReportDesignerItemBase*> cut_list;
};

//! @internal
class KoReportDesigner::Private
{
public:
    Private()
        : activeScene(0)
        , reportHeader(0)
        , pageHeaderFirst(0)
        , pageHeaderOdd(0)
        , pageHeaderEven(0)
        , pageHeaderLast(0)
        , pageHeaderAny(0)
        , pageFooterFirst(0)
        , pageFooterOdd(0)
        , pageFooterEven(0)
        , pageFooterLast(0)
        , pageFooterAny(0)
        , reportFooter(0)
        , detail(0)
        , pressX(-1)
        , pressY(-1)
        , releaseX(-1)
        , releaseY(-1)
        , modified(false)
        , kordata(0)
    {}

    ~Private()
    {
        delete zoom;
        delete sectionData;
        delete set;
    }

    QGridLayout *grid;
    KReportRuler *hruler;
    KReportZoomHandler *zoom;
    QVBoxLayout *vboxlayout;
    ReportPropertiesButton *pageButton;

    QGraphicsScene *activeScene;

    ReportWriterSectionData *sectionData;

    ReportSection *reportHeader;
    ReportSection *pageHeaderFirst;
    ReportSection *pageHeaderOdd;
    ReportSection *pageHeaderEven;
    ReportSection *pageHeaderLast;
    ReportSection *pageHeaderAny;

    ReportSection *pageFooterFirst;
    ReportSection *pageFooterOdd;
    ReportSection *pageFooterEven;
    ReportSection *pageFooterLast;
    ReportSection *pageFooterAny;
    ReportSection *reportFooter;
    ReportSectionDetail *detail;

    //Properties
    KPropertySet *set;
    KPropertySet *itmset;
    KProperty *title;
    KProperty *pageSize;
    KProperty *orientation;
    KProperty *unit;
    KProperty *customHeight;
    KProperty *customWidth;
    KProperty *leftMargin;
    KProperty *rightMargin;
    KProperty *topMargin;
    KProperty *bottomMargin;
    KProperty *showGrid;
    KProperty *gridDivisions;
    KProperty *gridSnap;
    KProperty *labelType;
    KProperty *interpreter;
    KProperty *script;

    //Actions
    QAction *editCutAction;
    QAction *editCopyAction;
    QAction *editPasteAction;
    QAction *editDeleteAction;
    QAction *sectionEdit;
    QAction *parameterEdit;
    QAction *itemRaiseAction;
    QAction *itemLowerAction;

    qreal pressX;
    qreal pressY;
    qreal releaseX;
    qreal releaseY;

    bool modified; // true if this document has been modified, false otherwise

    KoReportData *kordata;
};

KoReportDesigner::KoReportDesigner(QWidget * parent)
        : QWidget(parent), d(new Private())
{
    init();
}

void KoReportDesigner::init()
{
    d->sectionData = new ReportWriterSectionData();
    createProperties();
    createActions();

    d->grid = new QGridLayout(this);
    d->grid->setSpacing(0);
    d->grid->setMargin(0);
    d->grid->setColumnStretch(1, 1);
    d->grid->setRowStretch(1, 1);
    d->grid->setSizeConstraint(QLayout::SetFixedSize);

    d->vboxlayout = new QVBoxLayout();
    d->vboxlayout->setSpacing(0);
    d->vboxlayout->setMargin(0);
    d->vboxlayout->setSizeConstraint(QLayout::SetFixedSize);

    //Create nice rulers
    d->zoom = new KReportZoomHandler();
    d->hruler = new KReportRuler(this, Qt::Horizontal, d->zoom);

    d->pageButton = new ReportPropertiesButton(this);

    d->hruler->setUnit(KReportUnit(KReportUnit::Centimeter));

    d->grid->addWidget(d->pageButton, 0, 0);
    d->grid->addWidget(d->hruler, 0, 1);
    d->grid->addLayout(d->vboxlayout, 1, 0, 1, 2);

    d->pageButton->setMaximumSize(QSize(19, 22));
    d->pageButton->setMinimumSize(QSize(19, 22));

    d->detail = new ReportSectionDetail(this);
    d->vboxlayout->insertWidget(0, d->detail);

    setLayout(d->grid);

    connect(d->pageButton, SIGNAL(released()), this, SLOT(slotPageButton_Pressed()));
    emit pagePropertyChanged(*d->set);

    connect(d->set, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));

    changeSet(d->set);
}

KoReportDesigner::~KoReportDesigner()
{
    delete d;
}

///The loading Code
KoReportDesigner::KoReportDesigner(QWidget *parent, const QDomElement &data) : QWidget(parent), d(new Private())
{
    init();

    if (data.tagName() != QLatin1String("report:content")) {
        // arg we got an xml file but not one i know of
        kreportWarning() << "root element was not <report:content>";
    }
    //kreportDebug() << data.text();
    deleteDetail();

    QDomNodeList nlist = data.childNodes();
    QDomNode it;

    for (int i = 0; i < nlist.count(); ++i) {
        it = nlist.item(i);
        // at this level all the children we get should be Elements
        if (it.isElement()) {
            QString n = it.nodeName().toLower();
            //kreportDebug() << n;
            if (n == QLatin1String("report:title")) {
                setReportTitle(it.firstChild().nodeValue());
            } else if (n == QLatin1String("report:script")) {
                d->interpreter->setValue(it.toElement().attribute(QLatin1String("report:script-interpreter")));
                d->script->setValue(it.firstChild().nodeValue());
            } else if (n == QLatin1String("report:grid")) {
                d->showGrid->setValue(it.toElement().attribute(QLatin1String("report:grid-visible"), QString::number(1)).toInt() != 0);
                d->gridSnap->setValue(it.toElement().attribute(QLatin1String("report:grid-snap"), QString::number(1)).toInt() != 0);
                d->gridDivisions->setValue(it.toElement().attribute(QLatin1String("report:grid-divisions"), QString::number(4)).toInt());
                d->unit->setValue(it.toElement().attribute(QLatin1String("report:page-unit"), QLatin1String("cm")));
            }

            //! @todo Load page options
            else if (n == QLatin1String("report:page-style")) {
                QString pagetype = it.firstChild().nodeValue();

                if (pagetype == QLatin1String("predefined")) {
                    d->pageSize->setValue(it.toElement().attribute(QLatin1String("report:page-size"), QLatin1String("A4")));
                } else if (pagetype == QLatin1String("custom")) {
                    d->pageSize->setValue(QLatin1String("custom"));
                    d->customHeight->setValue(KReportUnit::parseValue(it.toElement().attribute(QLatin1String("report:custom-page-height"), QLatin1String(""))));
                    d->customWidth->setValue(KReportUnit::parseValue(it.toElement().attribute(QLatin1String("report:custom-page-widtht"), QLatin1String(""))));
                } else if (pagetype == QLatin1String("label")) {
                    //! @todo
                }

                d->rightMargin->setValue(KReportUnit::parseValue(it.toElement().attribute(QLatin1String("fo:margin-right"), QLatin1String("1.0cm"))));
                d->leftMargin->setValue(KReportUnit::parseValue(it.toElement().attribute(QLatin1String("fo:margin-left"), QLatin1String("1.0cm"))));
                d->topMargin->setValue(KReportUnit::parseValue(it.toElement().attribute(QLatin1String("fo:margin-top"), QLatin1String("1.0cm"))));
                d->bottomMargin->setValue(KReportUnit::parseValue(it.toElement().attribute(QLatin1String("fo:margin-bottom"), QLatin1String("1.0cm"))));

                d->orientation->setValue(it.toElement().attribute(QLatin1String("report:print-orientation"), QLatin1String("portrait")));

            } else if (n == QLatin1String("report:body")) {
                QDomNodeList sectionlist = it.childNodes();
                QDomNode sec;

                for (int s = 0; s < sectionlist.count(); ++s) {
                    sec = sectionlist.item(s);
                    if (sec.isElement()) {
                        QString sn = sec.nodeName().toLower();
                        //kreportDebug() << sn;
                        if (sn == QLatin1String("report:section")) {
                            QString sectiontype = sec.toElement().attribute(QLatin1String("report:section-type"));
                            if (section(KRSectionData::sectionTypeFromString(sectiontype)) == 0) {
                                insertSection(KRSectionData::sectionTypeFromString(sectiontype));
                                section(KRSectionData::sectionTypeFromString(sectiontype))->initFromXML(sec);
                            }
                        } else if (sn == QLatin1String("report:detail")) {
                            ReportSectionDetail * rsd = new ReportSectionDetail(this);
                            rsd->initFromXML(sec);
                            setDetail(rsd);
                        }
                    } else {
                        kreportWarning() << "Encountered an unknown Element: "  << n;
                    }
                }
            }
        } else {
            kreportWarning() << "Encountered a child node of root that is not an Element";
        }
    }
    this->slotPageButton_Pressed();
    emit reportDataChanged();
    slotPropertyChanged(*d->set, *d->unit); // set unit for all items
    setModified(false);
}

///The saving code
QDomElement KoReportDesigner::document() const
{
    QDomDocument doc;

    QDomElement content = doc.createElement(QLatin1String("report:content"));
    content.setAttribute(QLatin1String("xmlns:report"), QLatin1String(ns));
    content.setAttribute(QLatin1String("xmlns:fo"), QLatin1String("urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"));
    content.setAttribute(QLatin1String("xmlns:svg"), QLatin1String("urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0"));

    doc.appendChild(content);

    //title
    content.appendChild(propertyToElement(&doc, d->title));

    QDomElement scr = propertyToElement(&doc, d->script);
    KRUtils::addPropertyAsAttribute(&scr, d->interpreter);
    content.appendChild(scr);

    QDomElement grd = doc.createElement(QLatin1String("report:grid"));
    KRUtils::addPropertyAsAttribute(&grd, d->showGrid);
    KRUtils::addPropertyAsAttribute(&grd, d->gridDivisions);
    KRUtils::addPropertyAsAttribute(&grd, d->gridSnap);
    KRUtils::addPropertyAsAttribute(&grd, d->unit);
    content.appendChild(grd);

    // pageOptions
    // -- size
    QDomElement pagestyle = doc.createElement(QLatin1String("report:page-style"));

    if (d->pageSize->value().toString() == QLatin1String("Custom")) {
        pagestyle.appendChild(doc.createTextNode(QLatin1String("custom")));
        KRUtils::setAttribute(pagestyle, QLatin1String("report:custom-page-width"), d->customWidth->value().toDouble());
        KRUtils::setAttribute(pagestyle, QLatin1String("report:custom-page-height"), d->customHeight->value().toDouble());

    } else if (d->pageSize->value().toString() == QLatin1String("Label")) {
        pagestyle.appendChild(doc.createTextNode(QLatin1String("label")));
        pagestyle.setAttribute(QLatin1String("report:page-label-type"), d->labelType->value().toString());
    } else {
        pagestyle.appendChild(doc.createTextNode(QLatin1String("predefined")));
        KRUtils::addPropertyAsAttribute(&pagestyle, d->pageSize);
        //pagestyle.setAttribute("report:page-size", d->pageSize->value().toString());
    }

    // -- orientation
    KRUtils::addPropertyAsAttribute(&pagestyle, d->orientation);

    // -- margins: save as points, and not localized
    KRUtils::setAttribute(pagestyle, QLatin1String("fo:margin-top"), d->topMargin->value().toDouble());
    KRUtils::setAttribute(pagestyle, QLatin1String("fo:margin-bottom"), d->bottomMargin->value().toDouble());
    KRUtils::setAttribute(pagestyle, QLatin1String("fo:margin-right"), d->rightMargin->value().toDouble());
    KRUtils::setAttribute(pagestyle, QLatin1String("fo:margin-left"), d->leftMargin->value().toDouble());

    content.appendChild(pagestyle);

    QDomElement body = doc.createElement(QLatin1String("report:body"));
    QDomElement domsection;

    for (int i = KRSectionData::PageHeaderFirst; i <= KRSectionData::PageFooterAny; ++i) {
        ReportSection *sec = section((KRSectionData::Section)i);
        if (sec) {
            domsection = doc.createElement(QLatin1String("report:section"));
            domsection.setAttribute(QLatin1String("report:section-type"), KRSectionData::sectionTypeString(KRSectionData::Section(i)));
            sec->buildXML(doc, domsection);
            body.appendChild(domsection);
        }
    }

    QDomElement detail = doc.createElement(QLatin1String("report:detail"));
    d->detail->buildXML(doc, detail);
    body.appendChild(detail);

    content.appendChild(body);
    return content;
}

void KoReportDesigner::slotSectionEditor()
{
    QPointer<SectionEditor> se = new SectionEditor(this);
    se->init(this);
    se->exec();
    delete se;
}

void KoReportDesigner::setReportData(KoReportData* kodata)
{
    //kreportDebug();
    if (kodata) {
        d->kordata = kodata;
        slotPageButton_Pressed();
        setModified(true);
        emit reportDataChanged();
    }
}

ReportSection * KoReportDesigner::section(KRSectionData::Section s) const
{
    ReportSection *sec;
    switch (s) {
    case KRSectionData::PageHeaderAny:
        sec = d->pageHeaderAny;
        break;
    case KRSectionData::PageHeaderEven:
        sec = d->pageHeaderEven;
        break;
    case KRSectionData::PageHeaderOdd:
        sec = d->pageHeaderOdd;
        break;
    case KRSectionData::PageHeaderFirst:
        sec = d->pageHeaderFirst;
        break;
    case KRSectionData::PageHeaderLast:
        sec = d->pageHeaderLast;
        break;
    case KRSectionData::PageFooterAny:
        sec = d->pageFooterAny;
        break;
    case KRSectionData::PageFooterEven:
        sec = d->pageFooterEven;
        break;
    case KRSectionData::PageFooterOdd:
        sec = d->pageFooterOdd;
        break;
    case KRSectionData::PageFooterFirst:
        sec = d->pageFooterFirst;
        break;
    case KRSectionData::PageFooterLast:
        sec = d->pageFooterLast;
        break;
    case KRSectionData::ReportHeader:
        sec = d->reportHeader;
        break;
    case KRSectionData::ReportFooter:
        sec = d->reportFooter;
        break;
    default:
        sec = 0;
    }
    return sec;
}
void KoReportDesigner::removeSection(KRSectionData::Section s)
{
    ReportSection* sec = section(s);
    if (sec) {
        delete sec;

        switch (s) {
        case KRSectionData::PageHeaderAny:
            d->pageHeaderAny = 0;
            break;
        case KRSectionData::PageHeaderEven:
            sec = d->pageHeaderEven = 0;
            break;
        case KRSectionData::PageHeaderOdd:
            d->pageHeaderOdd = 0;
            break;
        case KRSectionData::PageHeaderFirst:
            d->pageHeaderFirst = 0;
            break;
        case KRSectionData::PageHeaderLast:
            d->pageHeaderLast = 0;
            break;
        case KRSectionData::PageFooterAny:
            d->pageFooterAny = 0;
            break;
        case KRSectionData::PageFooterEven:
            d->pageFooterEven = 0;
            break;
        case KRSectionData::PageFooterOdd:
            d->pageFooterOdd = 0;
            break;
        case KRSectionData::PageFooterFirst:
            d->pageFooterFirst = 0;
            break;
        case KRSectionData::PageFooterLast:
            d->pageFooterLast = 0;
            break;
        case KRSectionData::ReportHeader:
            d->reportHeader = 0;
            break;
        case KRSectionData::ReportFooter:
            d->reportFooter = 0;
            break;
        default:
            sec = 0;
        }

        setModified(true);
        adjustSize();
    }
}

void KoReportDesigner::insertSection(KRSectionData::Section s)
{
    ReportSection* sec = section(s);
    if (!sec) {
        int idx = 0;
        for (int i = 1; i <= s; ++i) {
            if (section((KRSectionData::Section)i))
                idx++;
        }
        if (s > KRSectionData::ReportHeader)
            idx++;
        //kreportDebug() << idx;
        ReportSection *rs = new ReportSection(this);
        d->vboxlayout->insertWidget(idx, rs);

        switch (s) {
        case KRSectionData::PageHeaderAny:
            rs->setTitle(tr("Page Header (Any)"));
            d->pageHeaderAny = rs;
            break;
        case KRSectionData::PageHeaderEven:
            rs->setTitle(tr("Page Header (Even)"));
            d->pageHeaderEven = rs;
            break;
        case KRSectionData::PageHeaderOdd:
            rs->setTitle(tr("Page Header (Odd)"));
            d->pageHeaderOdd = rs;
            break;
        case KRSectionData::PageHeaderFirst:
            rs->setTitle(tr("Page Header (First)"));
            d->pageHeaderFirst = rs;
            break;
        case KRSectionData::PageHeaderLast:
            rs->setTitle(tr("Page Header (Last)"));
            d->pageHeaderLast = rs;
            break;
        case KRSectionData::PageFooterAny:
            rs->setTitle(tr("Page Footer (Any)"));
            d->pageFooterAny = rs;
            break;
        case KRSectionData::PageFooterEven:
            rs->setTitle(tr("Page Footer (Even)"));
            d->pageFooterEven = rs;
            break;
        case KRSectionData::PageFooterOdd:
            rs->setTitle(tr("Page Footer (Odd)"));
            d->pageFooterOdd = rs;
            break;
        case KRSectionData::PageFooterFirst:
            rs->setTitle(tr("Page Footer (First)"));
            d->pageFooterFirst = rs;
            break;
        case KRSectionData::PageFooterLast:
            rs->setTitle(tr("Page Footer (Last)"));
            d->pageFooterLast = rs;
            break;
        case KRSectionData::ReportHeader:
            rs->setTitle(tr("Report Header"));
            d->reportHeader = rs;
            break;
        case KRSectionData::ReportFooter:
            rs->setTitle(tr("Report Footer"));
            d->reportFooter = rs;
            break;
            //These sections cannot be inserted this way
        case KRSectionData::None:
        case KRSectionData::GroupHeader:
        case KRSectionData::GroupFooter:
        case KRSectionData::Detail:
            break;
        }

        rs->show();
        setModified(true);
        adjustSize();
        emit pagePropertyChanged(*d->set);
    }
}

void KoReportDesigner::setReportTitle(const QString & str)
{
    if (reportTitle() != str) {
        d->title->setValue(str);
        setModified(true);
    }
}

KPropertySet * KoReportDesigner::propertySet() const
{
    return d->set;
}

KPropertySet* KoReportDesigner::itemPropertySet() const
{
    return d->itmset;
}

KoReportData *KoReportDesigner::reportData() const
{
    return d->kordata;
}

ReportSectionDetail * KoReportDesigner::detailSection() const
{
    return d->detail;
}

QString KoReportDesigner::reportTitle() const
{
    return d->title->value().toString();
}

bool KoReportDesigner::isModified() const
{
    return d->modified;
}

void KoReportDesigner::setModified(bool mod)
{
    d->modified = mod;

    if (d->modified) {
        emit dirty();
    }
}

QStringList KoReportDesigner::fieldNames() const
{
    QStringList qs;
    qs << QString();
    if (d->kordata)
        qs << d->kordata->fieldNames();

    return qs;
}

QStringList KoReportDesigner::fieldKeys() const
{
    QStringList qs;
    qs << QString();
    if (d->kordata)
        qs << d->kordata->fieldKeys();

    return qs;
}

void KoReportDesigner::createProperties()
{
    QStringList keys, strings;
    d->set = new KPropertySet(0, QLatin1String("Report"));

    connect(d->set, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));

    d->title = new KProperty("title", QLatin1String("Report"), tr("Title"), tr("Report Title"));

    keys.clear();
    keys =  KReportPageFormat::pageFormatNames();
    strings = KReportPageFormat::localizedPageFormatNames();
    QString defaultKey = KReportPageFormat::formatString(KReportPageFormat::defaultFormat());
    d->pageSize = new KProperty("page-size", keys, strings, defaultKey, tr("Page Size"));

    keys.clear(); strings.clear();
    keys << QLatin1String("portrait") << QLatin1String("landscape");
    strings << tr("Portrait") << tr("Landscape");
    d->orientation = new KProperty("print-orientation", keys, strings, QLatin1String("portrait"), tr("Page Orientation"));

    keys.clear(); strings.clear();

    strings = KReportUnit::listOfUnitNameForUi(KReportUnit::HidePixel);
    QString unit;
    foreach(const QString &un, strings) {
        unit = un.mid(un.indexOf(QLatin1String("(")) + 1, 2);
        keys << unit;
    }

    d->unit = new KProperty("page-unit", keys, strings, QLatin1String("cm"), tr("Page Unit"));

    d->showGrid = new KProperty("grid-visible", true, tr("Show Grid"));
    d->gridSnap = new KProperty("grid-snap", true, tr("Snap to Grid"));
    d->gridDivisions = new KProperty("grid-divisions", 4, tr("Grid Divisions"));

    d->leftMargin = new KProperty("margin-left", KReportUnit(KReportUnit::Centimeter).fromUserValue(1.0),
        tr("Left Margin"), tr("Left Margin"), KProperty::Double);
    d->rightMargin = new KProperty("margin-right", KReportUnit(KReportUnit::Centimeter).fromUserValue(1.0),
        tr("Right Margin"), tr("Right Margin"), KProperty::Double);
    d->topMargin = new KProperty("margin-top", KReportUnit(KReportUnit::Centimeter).fromUserValue(1.0),
        tr("Top Margin"), tr("Top Margin"), KProperty::Double);
    d->bottomMargin = new KProperty("margin-bottom", KReportUnit(KReportUnit::Centimeter).fromUserValue(1.0),
        tr("Bottom Margin"), tr("Bottom Margin"), KProperty::Double);
    d->leftMargin->setOption("unit", QLatin1String("cm"));
    d->rightMargin->setOption("unit", QLatin1String("cm"));
    d->topMargin->setOption("unit", QLatin1String("cm"));
    d->bottomMargin->setOption("unit", QLatin1String("cm"));

    keys = Kross::Manager::self().interpreters();
    d->interpreter = new KProperty("script-interpreter", keys, keys, keys.value(0), tr("Script Interpreter"));

    d->script = new KProperty("script", keys, keys, QString(), tr("Object Script"));

    d->set->addProperty(d->title);
    d->set->addProperty(d->pageSize);
    d->set->addProperty(d->orientation);
    d->set->addProperty(d->unit);
    d->set->addProperty(d->gridSnap);
    d->set->addProperty(d->showGrid);
    d->set->addProperty(d->gridDivisions);
    d->set->addProperty(d->leftMargin);
    d->set->addProperty(d->rightMargin);
    d->set->addProperty(d->topMargin);
    d->set->addProperty(d->bottomMargin);
    d->set->addProperty(d->interpreter);
    d->set->addProperty(d->script);

//    KProperty* _customHeight;
//    KProperty* _customWidth;

}

/**
@brief Handle property changes
*/
void KoReportDesigner::slotPropertyChanged(KPropertySet &s, KProperty &p)
{
    setModified(true);
    emit pagePropertyChanged(s);

    if (p.name() == "page-unit") {
        d->hruler->setUnit(pageUnit());
        QString newstr = d->set->property("page-unit").value().toString();

        d->set->property("margin-left").setOption("unit", newstr);
        d->set->property("margin-right").setOption("unit", newstr);
        d->set->property("margin-top").setOption("unit", newstr);
        d->set->property("margin-bottom").setOption("unit", newstr);
    }
}

void KoReportDesigner::slotPageButton_Pressed()
{
    if (d->kordata) {
        QStringList sl = d->kordata->scriptList(d->interpreter->value().toString());

        d->script->setListData(sl, sl);

    }
    changeSet(d->set);
}

QSize KoReportDesigner::sizeHint() const
{
    int w = 0;
    int h = 0;

    if (d->pageFooterAny)
        h += d->pageFooterAny->sizeHint().height();
    if (d->pageFooterEven)
        h += d->pageFooterEven->sizeHint().height();
    if (d->pageFooterFirst)
        h += d->pageFooterFirst->sizeHint().height();
    if (d->pageFooterLast)
        h += d->pageFooterLast->sizeHint().height();
    if (d->pageFooterOdd)
        h += d->pageFooterOdd->sizeHint().height();
    if (d->pageHeaderAny)
        h += d->pageHeaderAny->sizeHint().height();
    if (d->pageHeaderEven)
        h += d->pageHeaderEven->sizeHint().height();
    if (d->pageHeaderFirst)
        h += d->pageHeaderFirst->sizeHint().height();
    if (d->pageHeaderLast)
        h += d->pageHeaderLast->sizeHint().height();
    if (d->pageHeaderOdd)
        h += d->pageHeaderOdd->sizeHint().height();
    if (d->reportHeader)
        h += d->reportHeader->sizeHint().height();
    if (d->reportFooter) {
        h += d->reportFooter->sizeHint().height();

    }
    if (d->detail) {
        h += d->detail->sizeHint().height();
        w += d->detail->sizeHint().width();
    }

    h += d->hruler->height();

    return QSize(w, h);
}

int KoReportDesigner::pageWidthPx() const
{
    int cw = 0;
    int ch = 0;
    int width = 0;

    KReportPageFormat::Format pf = KReportPageFormat::formatFromString(d->set->property("page-size").value().toString());

    cw = POINT_TO_INCH(MM_TO_POINT(KReportPageFormat::width(pf, KReportPageFormat::Portrait))) * KReportDpi::dpiX();

    ch = POINT_TO_INCH(MM_TO_POINT(KReportPageFormat::height(pf, KReportPageFormat::Portrait))) * KReportDpi::dpiY();

    width = (d->set->property("print-orientation").value().toString() == QLatin1String("portrait") ? cw : ch);

    width = width - POINT_TO_INCH(d->set->property("margin-left").value().toDouble()) * KReportDpi::dpiX();
    width = width - POINT_TO_INCH(d->set->property("margin-right").value().toDouble()) * KReportDpi::dpiX();

    return width;
}

void KoReportDesigner::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);
    //hruler->setRulerLength ( vboxlayout->geometry().width() );
    d->hruler->setRulerLength(pageWidthPx());
}

void KoReportDesigner::setDetail(ReportSectionDetail *rsd)
{
    if (!d->detail) {
        int idx = 0;
        if (d->pageHeaderFirst) idx++;
        if (d->pageHeaderOdd) idx++;
        if (d->pageHeaderEven) idx++;
        if (d->pageHeaderLast) idx++;
        if (d->pageHeaderAny) idx++;
        if (d->reportHeader) idx++;
        d->detail = rsd;
        d->vboxlayout->insertWidget(idx, d->detail);
    }
}
void KoReportDesigner::deleteDetail()
{
    delete d->detail;
    d->detail = 0;
}

KReportUnit KoReportDesigner::pageUnit() const
{
    QString u;
    bool found;

    u = d->unit->value().toString();

    KReportUnit unit = KReportUnit::fromSymbol(u, &found);
    if (!found) {
        unit = KReportUnit(KReportUnit::Centimeter);
    }

    return unit;
}

void KoReportDesigner::setGridOptions(bool vis, int div)
{
    d->showGrid->setValue(QVariant(vis));
    d->gridDivisions->setValue(div);
}

//
// methods for the sectionMouse*Event()
//
void KoReportDesigner::sectionContextMenuEvent(ReportScene * s, QGraphicsSceneContextMenuEvent * e)
{
    Q_UNUSED(s);

    QMenu pop;

    bool itemsSelected = selectionCount() > 0;
    if (itemsSelected) {
        //! @todo KF5 use KStandardAction
        QAction *a = new QAction(QIcon::fromTheme(QLatin1String("edit-cut")), tr("Cut"), this);
        connect(a, SIGNAL(triggered()), this, SLOT(slotEditCut()));
        pop.addAction(a);
        //! @todo KF5 use KStandardAction
        a = new QAction(QIcon::fromTheme(QLatin1String("edit-copy")), tr("Copy"), this);
        connect(a, SIGNAL(triggered()), this, SLOT(slotEditCopy()));
        pop.addAction(a);
    }
    if (!d->sectionData->copy_list.isEmpty()) {
        QAction *a = new QAction(QIcon::fromTheme(QLatin1String("edit-paste")), tr("Paste"), this);
        connect(a, SIGNAL(triggered()), this, SLOT(slotEditPaste()));
        pop.addAction(a);
    }

    if (itemsSelected) {
        pop.addSeparator();
        //! @todo KF5 use KStandard*
        //const KGuiItem del = KStandardGuiItem::del();
        //a->setToolTip(del.toolTip());
        //a->setShortcut(QKeySequence(QKeySequence::Delete));
        QAction *a = new QAction(QIcon::fromTheme(QLatin1String("edit-delete")), tr("Delete"), this);
        connect(a, SIGNAL(triggered()), SLOT(slotEditDelete()));
        pop.addAction(a);
    }
    if (!pop.actions().isEmpty()) {
        pop.exec(e->screenPos());
    }
}

void KoReportDesigner::sectionMousePressEvent(ReportSceneView * v, QMouseEvent * e)
{
    Q_UNUSED(v);
    d->pressX = e->pos().x();
    d->pressY = e->pos().y();
}

void KoReportDesigner::sectionMouseReleaseEvent(ReportSceneView * v, QMouseEvent * e)
{
    e->accept();

    d->releaseX = e->pos().x();
    d->releaseY = e->pos().y();

    if (e->button() == Qt::LeftButton) {
        QPointF pos(d->pressX, d->pressY);
        QPointF end(d->releaseX, d->releaseY);
        if (d->releaseY >= v->scene()->height()) {
            d->releaseY = v->scene()->height();
            end.setY(v->scene()->height());
        }

        if (d->releaseX >= v->scene()->width()) {
            d->releaseX = v->scene()->width();
            end.setX(v->scene()->width());
        }

        if (d->sectionData->mouseAction == ReportWriterSectionData::MA_Insert) {
            QGraphicsItem * item = 0;
            if (d->sectionData->insertItem == QLatin1String("org.kde.kreport.line")) {
                item = new KoReportDesignerItemLine(v->designer(), v->scene(), pos, end);

            }
            else {
                KoReportPluginManager* pluginManager = KoReportPluginManager::self();
                KoReportPluginInterface *plug = pluginManager->plugin(d->sectionData->insertItem);
                if (plug) {
                    QObject *obj = plug->createDesignerInstance(v->designer(), v->scene(), pos);
                    if (obj) {
                        item = dynamic_cast<QGraphicsItem*>(obj);
                    }
                }
                else {
                    kreportWarning() << "attempted to insert an unknown item";
                }
            }
            if (item) {
                item->setVisible(true);
                item->setSelected(true);
                KoReportItemBase* baseReportItem = dynamic_cast<KoReportItemBase*>(item);
                if (baseReportItem) {
                    changeSet(baseReportItem->propertySet());
                    if (v && v->designer()) {
                        v->designer()->setModified(true);
                    }
                    emit itemInserted(d->sectionData->insertItem);
                }
            }

            d->sectionData->mouseAction = ReportWriterSectionData::MA_None;
            d->sectionData->insertItem.clear();
            unsetSectionCursor();
        }
    }
}

unsigned int KoReportDesigner::selectionCount() const
{
    if (activeScene())
        return activeScene()->selectedItems().count();
    else
        return 0;
}

void KoReportDesigner::changeSet(KPropertySet *s)
{
    //Set the checked state of the report properties button
    if (s == d->set)
        d->pageButton->setCheckState(Qt::Checked);
    else
        d->pageButton->setCheckState(Qt::Unchecked);

    d->itmset = s;
    emit propertySetChanged();
}

//
// Actions
//

void KoReportDesigner::slotItem(const QString &entity)
{
    kreportDebug() << entity;
    d->sectionData->mouseAction = ReportWriterSectionData::MA_Insert;
    d->sectionData->insertItem = entity;
    setSectionCursor(QCursor(Qt::CrossCursor));
}

void KoReportDesigner::slotEditDelete()
{
    QGraphicsItem * item = 0;
    bool modified = false;
    while (selectionCount() > 0) {
        item = activeScene()->selectedItems().value(0);
        if (item) {
            QGraphicsScene * scene = item->scene();
            delete item;
            scene->update();
            d->sectionData->mouseAction = ReportWriterSectionData::MA_None;
            modified = true;
        }
    }
    activeScene()->selectedItems().clear();

    /*! @todo temporary: clears cut and copy lists to make sure we do not crash
     if weve deleted something in the list
     should really check if an item is in the list first
     and remove it. */
    d->sectionData->cut_list.clear();
    d->sectionData->copy_list.clear();
    if (modified) {
        setModified(true);
    }
}

void KoReportDesigner::slotEditCut()
{
    if (selectionCount() > 0) {
        //First delete any items that are curerntly in the list
        //so as not to leak memory
        qDeleteAll(d->sectionData->cut_list);
        d->sectionData->cut_list.clear();

        QGraphicsItem * item = activeScene()->selectedItems().first();
        bool modified = false;
        if (item) {
            d->sectionData->copy_list.clear();
            foreach(QGraphicsItem *item, activeScene()->selectedItems()) {
                d->sectionData->cut_list.append(dynamic_cast<KoReportDesignerItemBase*>(item));
                d->sectionData->copy_list.append(dynamic_cast<KoReportDesignerItemBase*>(item));
            }
            foreach(QGraphicsItem *item, activeScene()->selectedItems()) {
                activeScene()->removeItem(item);
                activeScene()->update();
                modified = true;
            }
            d->sectionData->selected_x_offset = 10;
            d->sectionData->selected_y_offset = 10;
        }
        if (modified) {
            setModified(true);
        }
    }
}

void KoReportDesigner::slotEditCopy()
{
    if (selectionCount() < 1)
        return;

    QGraphicsItem * item = activeScene()->selectedItems().first();
    if (item) {
        d->sectionData->copy_list.clear();
        foreach(QGraphicsItem *item, activeScene()->selectedItems()) {
            d->sectionData->copy_list.append(dynamic_cast<KoReportDesignerItemBase*>(item));
        }
        d->sectionData->selected_x_offset = 10;
        d->sectionData->selected_y_offset = 10;
    }
}

void KoReportDesigner::slotEditPaste()
{
    // call the editPaste function passing it a reportsection
    slotEditPaste(activeScene());
}

void KoReportDesigner::slotEditPaste(QGraphicsScene * canvas)
{

    // paste a new item of the copy we have in the specified location
    if (!d->sectionData->copy_list.isEmpty()) {
        QList<QGraphicsItem*> activeItems = canvas->selectedItems();
        QGraphicsItem *activeItem = 0;
        if (activeItems.count() == 1) {
            activeItem = activeItems.first();
        }
        canvas->clearSelection();
        d->sectionData->mouseAction = ReportWriterSectionData::MA_None;

        //! @todo this code sucks :)
        //! The setPos calls only work AFTER the name has been set ?!?!?

        foreach(KoReportDesignerItemBase *item, d->sectionData->copy_list) {
            KoReportItemBase *obj = dynamic_cast<KoReportItemBase*>(item);
            const QString type = obj ? obj->typeName() : QLatin1String("object");
            //kreportDebug() << type;
            KoReportDesignerItemBase *ent = item->clone();
            KoReportItemBase *new_obj = dynamic_cast<KoReportItemBase*>(ent);
            new_obj->setEntityName(suggestEntityName(type));
            if (activeItem) {
                new_obj->position().setScenePos(QPointF(activeItem->x() + 10, activeItem->y() + 10));
            } else {
                new_obj->position().setScenePos(QPointF(0, 0));
            }
            changeSet(new_obj->propertySet());
            QGraphicsItem *pasted_ent = dynamic_cast<QGraphicsItem*>(ent);
            if (pasted_ent) {
                pasted_ent->setSelected(true);
                canvas->addItem(pasted_ent);
                pasted_ent->show();
                d->sectionData->mouseAction = ReportWriterSectionData::MA_Grab;
                setModified(true);
            }
        }
    }
}
void KoReportDesigner::slotRaiseSelected()
{
    dynamic_cast<ReportScene*>(activeScene())->raiseSelected();
}

void KoReportDesigner::slotLowerSelected()
{
    dynamic_cast<ReportScene*>(activeScene())->lowerSelected();
}

QGraphicsScene* KoReportDesigner::activeScene() const
{
    return d->activeScene;
}

void KoReportDesigner::setActiveScene(QGraphicsScene* a)
{
    if (d->activeScene && d->activeScene != a)
        d->activeScene->clearSelection();
    d->activeScene = a;

    //Trigger an update so that the last scene redraws its title;
    update();
}

KReportZoomHandler* KoReportDesigner::zoomHandler() const
{
    return d->zoom;
}

QString KoReportDesigner::suggestEntityName(const QString &n) const
{
    ReportSection *sec;
    int itemCount = 0;
    //Count items in the main sections
    for (int i = 1; i <= KRSectionData::PageFooterAny; i++) {
        sec = section((KRSectionData::Section) i);
        if (sec) {
            const QGraphicsItemList l = sec->items();
            itemCount += l.count();
        }
    }

    if (d->detail) {
        //Count items in the group headers/footers
        for (int i = 0; i < d->detail->groupSectionCount(); i++) {
            sec = d->detail->groupSection(i)->groupHeader();
            if (sec) {
                const QGraphicsItemList l = sec->items();
                itemCount += l.count();
            }
            sec = d->detail->groupSection(i)->groupFooter();
            if (sec) {
                const QGraphicsItemList l = sec->items();
                itemCount += l.count();
            }
        }

        sec = d->detail->detailSection();
        if (sec) {
            const QGraphicsItemList l = sec->items();
            itemCount += l.count();
        }
    }

    while (!isEntityNameUnique(n + QString::number(itemCount))) {
        itemCount++;
    }
    return n + QString::number(itemCount);
}

bool KoReportDesigner::isEntityNameUnique(const QString &n, KoReportItemBase* ignore) const
{
    ReportSection *sec;
    bool unique = true;

    //Check items in the main sections
    for (int i = 1; i <= KRSectionData::PageFooterAny; i++) {
        sec = section((KRSectionData::Section)i);
        if (sec) {
            const QGraphicsItemList l = sec->items();
            for (QGraphicsItemList::const_iterator it = l.constBegin(); it != l.constEnd(); ++it) {
                KoReportItemBase* itm = dynamic_cast<KoReportItemBase*>(*it);
                if (itm && itm->entityName() == n  && itm != ignore) {
                    unique = false;
                    break;
                }
            }
            if (!unique) break;
        }
    }

    //Count items in the group headers/footers
    if (unique && d->detail) {
        for (int i = 0; i < d->detail->groupSectionCount(); ++i) {
            sec = d->detail->groupSection(i)->groupHeader();
            if (sec) {
                const QGraphicsItemList l = sec->items();
                for (QGraphicsItemList::const_iterator it = l.constBegin(); it != l.constEnd(); ++it) {
                    KoReportItemBase* itm = dynamic_cast<KoReportItemBase*>(*it);
                    if (itm && itm->entityName() == n  && itm != ignore) {
                        unique = false;
                        break;
                    }
                }

            }
            sec = d->detail->groupSection(i)->groupFooter();
            if (unique && sec) {
                const QGraphicsItemList l = sec->items();
                for (QGraphicsItemList::const_iterator it = l.constBegin(); it != l.constEnd(); ++it) {
                    KoReportItemBase* itm = dynamic_cast<KoReportItemBase*>(*it);
                    if (itm && itm->entityName() == n  && itm != ignore) {
                        unique = false;
                        break;
                    }
                }
            }
        }
    }
    if (unique && d->detail) {
        sec = d->detail->detailSection();
        if (sec) {
            const QGraphicsItemList l = sec->items();
            for (QGraphicsItemList::const_iterator it = l.constBegin(); it != l.constEnd(); ++it) {
                KoReportItemBase* itm = dynamic_cast<KoReportItemBase*>(*it);
                if (itm && itm && itm->entityName() == n  && itm != ignore) {
                    unique = false;
                    break;
                }
            }
        }
    }

    return unique;
}

static bool actionPriortyLessThan(QAction* act1, QAction* act2)
{
    if (act1->data().toInt() > 0 && act2->data().toInt() > 0) {
        return act1->data().toInt() < act2->data().toInt();
    }
    return false;
}

QList<QAction*> KoReportDesigner::itemActions(QActionGroup* group)
{
    KoReportPluginManager* manager = KoReportPluginManager::self();
    QList<QAction*> actList = manager->actions();

    QAction *act = new QAction(QIcon::fromTheme(QLatin1String("draw-line")), tr("Line"), group);
    act->setObjectName(QLatin1String("org.kde.kreport.line"));
    act->setData(9);
    act->setCheckable(true);
    actList << act;

    qSort(actList.begin(), actList.end(), actionPriortyLessThan);
    int i = 0;

    /*! @todo maybe this is a bit hackish
     It finds the first plugin based on the priority in userdata
     The lowest oriority a plugin can have is 10
     And inserts a separator before it. */
    bool sepInserted = false;
    foreach(QAction *a, actList) {
        ++i;
        if (!sepInserted && a->data().toInt() >= 10) {
            QAction *sep = new QAction(QLatin1String("separator"), group);
            sep->setSeparator(true);
            actList.insert(i-1, sep);
            sepInserted = true;
        }
        if (group) {
            group->addAction(a);
        }
    }

    return actList;
}

QList< QAction* > KoReportDesigner::designerActions()
{
    QList<QAction*> al;
    QAction *sep = new QAction(QString(), this);
    sep->setSeparator(true);

    al << d->editCutAction << d->editCopyAction << d->editPasteAction << d->editDeleteAction << sep << d->sectionEdit << sep << d->itemLowerAction << d->itemRaiseAction;

    return al;
}

void KoReportDesigner::createActions()
{
    d->editCutAction = KStandardAction::cut(this);
    d->editCutAction->setProperty("iconOnly", true);
    d->editCopyAction = KStandardAction::copy(this);
    d->editCopyAction->setProperty("iconOnly", true);
    d->editPasteAction = KStandardAction::paste(this);
    d->editPasteAction->setProperty("iconOnly", true);
    const KGuiItem del = KStandardGuiItem::del();
    d->editDeleteAction = new QAction(del.icon(), del.text(), this);
    d->editDeleteAction->setObjectName(QLatin1String("editdelete"));
    d->editDeleteAction->setToolTip(del.toolTip());
    d->editDeleteAction->setWhatsThis(del.whatsThis());
    d->editDeleteAction->setProperty("iconOnly", true);

    d->sectionEdit = new QAction(tr("Edit Sections"), this);
    d->sectionEdit->setObjectName(QLatin1String("sectionedit"));

    d->itemRaiseAction = new QAction(QIcon::fromTheme(QLatin1String("arrow-up")), tr("Raise"), this);
    d->itemRaiseAction->setObjectName(QLatin1String("itemraise"));
    d->itemLowerAction = new QAction(QIcon::fromTheme(QLatin1String("arrow-down")), tr("Lower"), this);
    d->itemLowerAction->setObjectName(QLatin1String("itemlower"));

    //Edit Actions
    connect(d->editCutAction, SIGNAL(triggered(bool)), this, SLOT(slotEditCut()));
    connect(d->editCopyAction, SIGNAL(triggered(bool)), this, SLOT(slotEditCopy()));
    connect(d->editPasteAction, SIGNAL(triggered(bool)), this, SLOT(slotEditPaste()));
    connect(d->editDeleteAction, SIGNAL(triggered(bool)), this, SLOT(slotEditDelete()));

    connect(d->sectionEdit, SIGNAL(triggered(bool)), this, SLOT(slotSectionEditor()));

    //Raise/Lower
    connect(d->itemRaiseAction, SIGNAL(triggered(bool)), this, SLOT(slotRaiseSelected()));
    connect(d->itemLowerAction, SIGNAL(triggered(bool)), this, SLOT(slotLowerSelected()));
}

void KoReportDesigner::setSectionCursor(const QCursor& c)
{
    if (d->pageFooterAny)
        d->pageFooterAny->setSectionCursor(c);
    if (d->pageFooterEven)
        d->pageFooterEven->setSectionCursor(c);
    if (d->pageFooterFirst)
        d->pageFooterFirst->setSectionCursor(c);
    if (d->pageFooterLast)
        d->pageFooterLast->setSectionCursor(c);
    if (d->pageFooterOdd)
        d->pageFooterOdd->setSectionCursor(c);

    if (d->pageHeaderAny)
        d->pageHeaderAny->setSectionCursor(c);
    if (d->pageHeaderEven)
        d->pageHeaderEven->setSectionCursor(c);
    if (d->pageHeaderFirst)
        d->pageHeaderFirst->setSectionCursor(c);
    if (d->pageHeaderLast)
        d->pageHeaderLast->setSectionCursor(c);
    if (d->pageHeaderOdd)
        d->pageHeaderOdd->setSectionCursor(c);

    if (d->detail)
        d->detail->setSectionCursor(c);
}

void KoReportDesigner::unsetSectionCursor()
{
    if (d->pageFooterAny)
        d->pageFooterAny->unsetSectionCursor();
    if (d->pageFooterEven)
        d->pageFooterEven->unsetSectionCursor();
    if (d->pageFooterFirst)
        d->pageFooterFirst->unsetSectionCursor();
    if (d->pageFooterLast)
        d->pageFooterLast->unsetSectionCursor();
    if (d->pageFooterOdd)
        d->pageFooterOdd->unsetSectionCursor();

    if (d->pageHeaderAny)
        d->pageHeaderAny->unsetSectionCursor();
    if (d->pageHeaderEven)
        d->pageHeaderEven->unsetSectionCursor();
    if (d->pageHeaderFirst)
        d->pageHeaderFirst->unsetSectionCursor();
    if (d->pageHeaderLast)
        d->pageHeaderLast->unsetSectionCursor();
    if (d->pageHeaderOdd)
        d->pageHeaderOdd->unsetSectionCursor();

    if (d->detail)
        d->detail->unsetSectionCursor();
}

qreal KoReportDesigner::countSelectionHeight() const
{
    if (d->releaseY == -1 || d->pressY == -1) {
        return -1;
    }
    return qAbs(d->releaseY - d->pressY);
}

qreal KoReportDesigner::countSelectionWidth() const
{
    if (d->releaseX == -1 || d->pressX == -1) {
        return -1;
    }
    return qAbs(d->releaseX - d->pressX);
}

qreal KoReportDesigner::getSelectionPressX() const
{
    return d->pressX;
}

qreal KoReportDesigner::getSelectionPressY() const
{
    return d->pressY;
}

QPointF KoReportDesigner::getPressPoint() const
{
    return QPointF(d->pressX, d->pressY);
}

QPointF KoReportDesigner::getReleasePoint() const
{
    return QPointF(d->releaseX, d->releaseY);
}

void KoReportDesigner::plugItemActions(const QList<QAction*> &actList)
{
    foreach(QAction *a, actList) {
        connect(a, SIGNAL(triggered(bool)), this, SLOT(slotItemTriggered(bool)));
    }
}

void KoReportDesigner::slotItemTriggered(bool checked)
{
    if (!checked) {
        return;
    }
    QObject *theSender = sender();
    if (!theSender) {
        return;
    }
    slotItem(theSender->objectName());
}
