/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC <info@openmfg.com>
 * Copyright (C) 2007-2010 by Adam Pigg <adam@piggz.co.uk>
 * Copyright (C) 2011-2017 Jarosław Staniek <staniek@kde.org>
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

#include "KReportDesigner.h"
#include "KReportDesign_p.h"
#include "KReportDesignerItemLine.h"
#include "KReportDesignerSection.h"
#include "KReportDesignerSectionDetail.h"
#include "KReportDesignerSectionDetailGroup.h"
#include "KReportDesignerSectionScene.h"
#include "KReportDesignerSectionView.h"
#include "KReportPageSize.h"
#include "KReportPluginInterface.h"
#include "KReportPluginManager.h"
#include "KReportPluginMetaData.h"
#include "KReportPropertiesButton.h"
#include "KReportRuler_p.h"
#include "KReportSection.h"
#include "KReportSectionEditor.h"
#include "KReportUtils.h"
#include "KReportUtils_p.h"
#include "KReportZoomHandler_p.h"
#include "kreport_debug.h"
#ifdef KREPORT_SCRIPTING
#include "KReportScriptSource.h"
#endif

#include <KPropertyListData>

#include <KStandardShortcut>
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
#include <QMessageBox>

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
        mouseAction = MouseAction::None;
    }
    virtual ~ReportWriterSectionData() {
    }

    enum class MouseAction {
        None = 0,
        Insert = 1,
        Grab = 2,
        MoveStartPoint,
        MoveEndPoint,
        ResizeNW = 8,
        ResizeN,
        ResizeNE,
        ResizeE,
        ResizeSE,
        ResizeS,
        ResizeSW,
        ResizeW
    };

    int selected_x_offset;
    int selected_y_offset;

    MouseAction mouseAction;
    QString itemToInsert;

    QList<KReportDesignerItemBase*> copy_list;
    QList<KReportDesignerItemBase*> cut_list;
};

//! @internal
class Q_DECL_HIDDEN KReportDesigner::Private
{
public:
    explicit Private(KReportDesigner *designer);

    ~Private()
    {
        delete dataSource;
    }

    void init(const QDomElement *xml);

    void updateCurrentUnit() {
        QString u = unit->value().toString();
        KReportUnit newUnit = KReportUnit(KReportUnit::symbolToType(u));
        if (newUnit.isValid()) {
            currentUnit = newUnit;
        } else {
            currentUnit = DEFAULT_UNIT;
        }

        if (u == QLatin1String("dm")) {
            gridDivisions->setOption("max", 100);
        } else {
            gridDivisions->setOption("max", 10);
            if (gridDivisions->value().toInt() > 10) {
                gridDivisions->setValue(10, KProperty::ValueOption::IgnoreOld);
            }
        }
    }

#ifdef KREPORT_SCRIPTING
    void updateScripts();
#endif

    KReportDesigner * const q;

    QGridLayout *grid;
    KReportRuler *hruler;
    KReportZoomHandler zoomHandler;
    QVBoxLayout *vboxlayout;
    KReportPropertiesButton *pageButton;

    QGraphicsScene *activeScene = nullptr;

    ReportWriterSectionData sectionData;

    KReportDesignerSection *reportHeader = nullptr;
    KReportDesignerSection *pageHeaderFirst = nullptr;
    KReportDesignerSection *pageHeaderOdd = nullptr;
    KReportDesignerSection *pageHeaderEven = nullptr;
    KReportDesignerSection *pageHeaderLast = nullptr;
    KReportDesignerSection *pageHeaderAny = nullptr;

    KReportDesignerSection *pageFooterFirst = nullptr;
    KReportDesignerSection *pageFooterOdd = nullptr;
    KReportDesignerSection *pageFooterEven = nullptr;
    KReportDesignerSection *pageFooterLast = nullptr;
    KReportDesignerSection *pageFooterAny = nullptr;
    KReportDesignerSection *reportFooter = nullptr;
    KReportDesignerSectionDetail *detail = nullptr;

    //Properties
    KPropertySet set;
    KPropertySet *itemSet;
    KProperty *title;
    KProperty *pageSize;
    KProperty *orientation;
    KProperty *unit;
    KProperty *customPageSize;
    KProperty *leftMargin;
    KProperty *rightMargin;
    KProperty *topMargin;
    KProperty *bottomMargin;
    KProperty *showGrid;
    KProperty *gridDivisions;
    KProperty *gridSnap;
    KProperty *labelType;
#ifdef KREPORT_SCRIPTING
    KProperty *script;
#endif

    KReportUnit currentUnit;

    //Actions
    QAction *editCutAction;
    QAction *editCopyAction;
    QAction *editPasteAction;
    QAction *editDeleteAction;
    QAction *sectionEdit;
    QAction *parameterEdit;
    QAction *itemRaiseAction;
    QAction *itemLowerAction;

    qreal pressX = -1;
    qreal pressY = -1;
    qreal releaseX = -1;
    qreal releaseY = -1;

    bool modified = false; // true if this document has been modified, false otherwise

    QString originalInterpreter; //Value of the script interpreter at load time
    QString originalScript; //Value of the script at load time

    KReportDataSource *dataSource = nullptr;
#ifdef KREPORT_SCRIPTING
    KReportScriptSource *scriptSource = nullptr;
#endif

private:
    void loadXml(const QDomElement &data);
};

KReportDesigner::Private::Private(KReportDesigner *designer)
    : q(designer), currentUnit(DEFAULT_UNIT_TYPE)
{
}

// (must be init() instead of ctor because we are indirectly depending on initialized KReportDesigner::d here)
void KReportDesigner::Private::init(const QDomElement *xml)
{
    KReportPluginManager::self(); // this loads icons early enough

    q->createProperties();
    q->createActions();

    grid = new QGridLayout(q);
    grid->setSpacing(0);
    grid->setMargin(0);
    grid->setColumnStretch(1, 1);
    grid->setRowStretch(1, 1);
    grid->setSizeConstraint(QLayout::SetFixedSize);

    vboxlayout = new QVBoxLayout();
    vboxlayout->setSpacing(0);
    vboxlayout->setMargin(0);
    vboxlayout->setSizeConstraint(QLayout::SetFixedSize);

    //Create nice rulers
    hruler = new KReportRuler(nullptr, Qt::Horizontal, zoomHandler);
    hruler->setUnit(DEFAULT_UNIT);

    pageButton = new KReportPropertiesButton;

    grid->addWidget(pageButton, 0, 0);
    grid->addWidget(hruler, 0, 1);
    grid->addLayout(vboxlayout, 1, 0, 1, 2);

    pageButton->setMaximumSize(QSize(19, 22));
    pageButton->setMinimumSize(QSize(19, 22));

    if (!xml) {
        detail = new KReportDesignerSectionDetail(q);
        vboxlayout->insertWidget(0, detail);
    }

    connect(pageButton, &KReportPropertiesButton::released,
            q, &KReportDesigner::slotPageButton_Pressed);
    emit q->pagePropertyChanged(set);

    connect(&set, &KPropertySet::propertyChanged, q, &KReportDesigner::slotPropertyChanged);

    if (xml) {
        loadXml(*xml);
    }
    set.clearModifiedFlags();
    q->changeSet(&set);
}

void KReportDesigner::Private::loadXml(const QDomElement &data)
{
    if (data.tagName() != QLatin1String("report:content")) {
        // arg we got an xml file but not one i know of
        kreportWarning() << "root element was not <report:content>";
    }
    //kreportDebug() << data.text();

    QDomNodeList nlist = data.childNodes();
    QDomNode it;

    for (int i = 0; i < nlist.count(); ++i) {
        it = nlist.item(i);
        // at this level all the children we get should be Elements
        if (it.isElement()) {
            QString n = it.nodeName().toLower();
            //kreportDebug() << n;
            if (n == QLatin1String("report:title")) {
                q->setReportTitle(it.firstChild().nodeValue());
#ifdef KREPORT_SCRIPTING
            } else if (n == QLatin1String("report:script")) {
                originalInterpreter = it.toElement().attribute(QLatin1String("report:script-interpreter"), QLatin1String("javascript"));
                if (originalInterpreter.isEmpty()) {
                    originalInterpreter = QLatin1String("javascript");
                }
                originalScript = it.firstChild().nodeValue();
                script->setValue(originalScript);

                if (originalInterpreter != QLatin1String("javascript") && originalInterpreter != QLatin1String("qtscript")) {
                    QString msg = tr("This report contains scripts of type \"%1\". "
                                     "Only scripts written in JavaScript language are "
                                     "supported. To prevent losing the scripts, their type "
                                     "and content will not be changed unless you change these scripts."
                                     ).arg(originalInterpreter);
                    QMessageBox::warning(q, tr("Unsupported Script Type"), msg);
                }
#endif
            } else if (n == QLatin1String("report:grid")) {
                showGrid->setValue(it.toElement().attribute(QLatin1String("report:grid-visible"), QString::number(1)).toInt() != 0);
                gridSnap->setValue(it.toElement().attribute(QLatin1String("report:grid-snap"), QString::number(1)).toInt() != 0);
                gridDivisions->setValue(it.toElement().attribute(QLatin1String("report:grid-divisions"), QString::number(4)).toInt());
                unit->setValue(it.toElement().attribute(QLatin1String("report:page-unit"), DEFAULT_UNIT_STRING));
                updateCurrentUnit();
            }

            //! @todo Load page options
            else if (n == QLatin1String("report:page-style")) {
                QString pagetype = it.firstChild().nodeValue();

                if (pagetype == QLatin1String("predefined")) {
                    pageSize->setValue(it.toElement().attribute(QLatin1String("report:page-size"), QLatin1String("A4")));
                } else if (pagetype == QLatin1String("custom")) {
                    pageSize->setValue(QLatin1String("Custom"));
                    customPageSize->setValue(QSizeF(KReportUnit::parseValue(it.toElement().attribute(QLatin1String("report:custom-page-width"), QLatin1String(""))),
                        KReportUnit::parseValue(it.toElement().attribute(QLatin1String("report:custom-page-height"), QLatin1String("")))));
                } else if (pagetype == QLatin1String("label")) {
                    //! @todo
                }

                rightMargin->setValue(currentUnit.convertFromPoint(
                    KReportUnit::parseValue(it.toElement().attribute(
                        QLatin1String("fo:margin-right"), DEFAULT_PAGE_MARGIN_STRING))));
                leftMargin->setValue(currentUnit.convertFromPoint(
                    KReportUnit::parseValue(it.toElement().attribute(
                        QLatin1String("fo:margin-left"), DEFAULT_PAGE_MARGIN_STRING))));
                topMargin->setValue(currentUnit.convertFromPoint(
                    KReportUnit::parseValue(it.toElement().attribute(
                        QLatin1String("fo:margin-top"), DEFAULT_PAGE_MARGIN_STRING))));
                bottomMargin->setValue(currentUnit.convertFromPoint(
                    KReportUnit::parseValue(it.toElement().attribute(
                        QLatin1String("fo:margin-bottom"), DEFAULT_PAGE_MARGIN_STRING))));
                orientation->setValue(
                    it.toElement().attribute(QLatin1String("report:print-orientation"),
                                             QLatin1String("portrait")));
            } else if (n == QLatin1String("report:body")) {
                QDomNodeList sectionlist = it.childNodes();
                QDomNode sec;

                for (int s = 0; s < sectionlist.count(); ++s) {
                    sec = sectionlist.item(s);
                    if (sec.isElement()) {
                        QString sn = sec.nodeName().toLower();
                        //kreportDebug() << sn;
                        if (sn == QLatin1String("report:section")) {
                            const QString sectiontype = KReportUtils::readSectionTypeNameAttribute(sec.toElement());
                            if (q->section(KReportSectionData::sectionTypeFromString(sectiontype)) == nullptr) {
                                q->insertSection(KReportSectionData::sectionTypeFromString(sectiontype));
                                q->section(KReportSectionData::sectionTypeFromString(sectiontype))->initFromXML(sec);
                            }
                        } else if (sn == QLatin1String("report:detail")) {
                            KReportDesignerSectionDetail * rsd = new KReportDesignerSectionDetail(q);
                            rsd->initFromXML(&sec);
                            q->setDetail(rsd);
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
    updateScripts();
    emit q->reportDataChanged();
    q->slotPropertyChanged(set, *unit); // set unit for all items
    q->setModified(false);
}

#ifdef KREPORT_SCRIPTING
void KReportDesigner::Private::updateScripts()
{
    if (scriptSource) {
        QStringList sl = scriptSource->scriptList();
        sl.prepend(QString()); // prepend "none"
        script->setListData(sl, sl);
    }
}
#endif

// ----

KReportDesigner::KReportDesigner(QWidget * parent)
        : QWidget(parent), d(new Private(this))
{
    d->init(nullptr);
}

KReportDesigner::KReportDesigner(QWidget *parent, const QDomElement &data)
    : QWidget(parent), d(new Private(this))
{
    d->init(&data);
}

KReportDesigner::~KReportDesigner()
{
    delete d;
}

///The saving code
QDomElement KReportDesigner::document() const
{
    QDomDocument doc;
    QString saveInterpreter;

    QDomElement content = doc.createElement(QLatin1String("report:content"));
    content.setAttribute(QLatin1String("xmlns:report"), QLatin1String(ns));
    content.setAttribute(QLatin1String("xmlns:fo"), QLatin1String("urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"));
    content.setAttribute(QLatin1String("xmlns:svg"), QLatin1String("urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0"));

    doc.appendChild(content);

    //title
    content.appendChild(propertyToElement(&doc, d->title));

#ifdef KREPORT_SCRIPTING
    if (d->originalInterpreter.isEmpty()) {
        d->originalInterpreter = QLatin1String("javascript");
    }
    saveInterpreter = d->originalInterpreter;

    if (!d->script->value().toString().isEmpty()) {
        if (d->script->value().toString() != d->originalScript || d->originalInterpreter == QLatin1String("qtscript") || d->originalInterpreter.isEmpty() ) {
            //The script has changed so force interpreter to 'javascript'.  Also set if was using qtscript
            saveInterpreter = QLatin1String("javascript");
        }
    }

    QDomElement scr = propertyToElement(&doc, d->script);
    scr.setAttribute(QLatin1String("report:script-interpreter"), saveInterpreter);
    content.appendChild(scr);
#endif

    QDomElement grd = doc.createElement(QLatin1String("report:grid"));
    KReportUtils::addPropertyAsAttribute(&grd, d->showGrid);
    KReportUtils::addPropertyAsAttribute(&grd, d->gridDivisions);
    KReportUtils::addPropertyAsAttribute(&grd, d->gridSnap);
    KReportUtils::addPropertyAsAttribute(&grd, d->unit);
    content.appendChild(grd);

    // pageOptions
    // -- size
    QDomElement pagestyle = doc.createElement(QLatin1String("report:page-style"));

    if (d->pageSize->value().toString() == QLatin1String("Custom")) {
        pagestyle.appendChild(doc.createTextNode(QLatin1String("custom")));

        KReportUtils::setAttribute(
            &pagestyle, QLatin1String("report:custom-page-width"),
            d->currentUnit.convertToPoint(d->customPageSize->value().toSizeF().width()));
        KReportUtils::setAttribute(
            &pagestyle, QLatin1String("report:custom-page-height"),
            d->currentUnit.convertToPoint(d->customPageSize->value().toSizeF().height()));
    } else if (d->pageSize->value().toString() == QLatin1String("Label")) {
        pagestyle.appendChild(doc.createTextNode(QLatin1String("label")));
        pagestyle.setAttribute(QLatin1String("report:page-label-type"), d->labelType->value().toString());
    } else {
        pagestyle.appendChild(doc.createTextNode(QLatin1String("predefined")));
        KReportUtils::addPropertyAsAttribute(&pagestyle, d->pageSize);
        //pagestyle.setAttribute("report:page-size", d->pageSize->value().toString());
    }

    // -- orientation
    KReportUtils::addPropertyAsAttribute(&pagestyle, d->orientation);

    // -- margins: save as points, and not localized
    KReportUtils::setAttribute(
        &pagestyle, QLatin1String("fo:margin-top"),
        d->currentUnit.convertToPoint(d->topMargin->value().toDouble()));
    KReportUtils::setAttribute(
        &pagestyle, QLatin1String("fo:margin-bottom"),
        d->currentUnit.convertToPoint(d->bottomMargin->value().toDouble()));
    KReportUtils::setAttribute(
        &pagestyle, QLatin1String("fo:margin-right"),
        d->currentUnit.convertToPoint(d->rightMargin->value().toDouble()));
    KReportUtils::setAttribute(
        &pagestyle, QLatin1String("fo:margin-left"),
        d->currentUnit.convertToPoint(d->leftMargin->value().toDouble()));

    content.appendChild(pagestyle);

    QDomElement body = doc.createElement(QLatin1String("report:body"));
    QDomElement domsection;

    for (int i = static_cast<int>(KReportSectionData::Type::PageHeaderFirst);
         i <= static_cast<int>(KReportSectionData::Type::PageFooterAny); ++i)
    {
        KReportDesignerSection *sec = section(static_cast<KReportSectionData::Type>(i));
        if (sec) {
            domsection = doc.createElement(QLatin1String("report:section"));
            domsection.setAttribute(
                QLatin1String("report:section-type"),
                KReportSectionData::sectionTypeString(static_cast<KReportSectionData::Type>(i)));
            sec->buildXML(&doc, &domsection);
            body.appendChild(domsection);
        }
    }

    QDomElement detail = doc.createElement(QLatin1String("report:detail"));
    d->detail->buildXML(&doc, &detail);
    body.appendChild(detail);

    content.appendChild(body);
    return content;
}

void KReportDesigner::slotSectionEditor()
{
    KReportSectionEditor se(this);
    (void)se.exec();
}

void KReportDesigner::setDataSource(KReportDataSource* source)
{
    if (d->dataSource == source) {
        return;
    }
    delete d->dataSource;

    d->dataSource = source;
    slotPageButton_Pressed();
    setModified(true);
    emit reportDataChanged();
}

#ifdef KREPORT_SCRIPTING
void KReportDesigner::setScriptSource(KReportScriptSource* source)
{
    d->scriptSource = source;
}
#endif

KReportDesignerSection * KReportDesigner::section(KReportSectionData::Type type) const
{
    KReportDesignerSection *sec;
    switch (type) {
    case KReportSectionData::Type::PageHeaderAny:
        sec = d->pageHeaderAny;
        break;
    case KReportSectionData::Type::PageHeaderEven:
        sec = d->pageHeaderEven;
        break;
    case KReportSectionData::Type::PageHeaderOdd:
        sec = d->pageHeaderOdd;
        break;
    case KReportSectionData::Type::PageHeaderFirst:
        sec = d->pageHeaderFirst;
        break;
    case KReportSectionData::Type::PageHeaderLast:
        sec = d->pageHeaderLast;
        break;
    case KReportSectionData::Type::PageFooterAny:
        sec = d->pageFooterAny;
        break;
    case KReportSectionData::Type::PageFooterEven:
        sec = d->pageFooterEven;
        break;
    case KReportSectionData::Type::PageFooterOdd:
        sec = d->pageFooterOdd;
        break;
    case KReportSectionData::Type::PageFooterFirst:
        sec = d->pageFooterFirst;
        break;
    case KReportSectionData::Type::PageFooterLast:
        sec = d->pageFooterLast;
        break;
    case KReportSectionData::Type::ReportHeader:
        sec = d->reportHeader;
        break;
    case KReportSectionData::Type::ReportFooter:
        sec = d->reportFooter;
        break;
    default:
        sec = nullptr;
    }
    return sec;
}

KReportDesignerSection* KReportDesigner::createSection()
{
    return new KReportDesignerSection(this, d->zoomHandler);
}

void KReportDesigner::removeSection(KReportSectionData::Type type)
{
    KReportDesignerSection* sec = section(type);
    if (sec) {
        delete sec;

        switch (type) {
        case KReportSectionData::Type::PageHeaderAny:
            d->pageHeaderAny = nullptr;
            break;
        case KReportSectionData::Type::PageHeaderEven:
            sec = d->pageHeaderEven = nullptr;
            break;
        case KReportSectionData::Type::PageHeaderOdd:
            d->pageHeaderOdd = nullptr;
            break;
        case KReportSectionData::Type::PageHeaderFirst:
            d->pageHeaderFirst = nullptr;
            break;
        case KReportSectionData::Type::PageHeaderLast:
            d->pageHeaderLast = nullptr;
            break;
        case KReportSectionData::Type::PageFooterAny:
            d->pageFooterAny = nullptr;
            break;
        case KReportSectionData::Type::PageFooterEven:
            d->pageFooterEven = nullptr;
            break;
        case KReportSectionData::Type::PageFooterOdd:
            d->pageFooterOdd = nullptr;
            break;
        case KReportSectionData::Type::PageFooterFirst:
            d->pageFooterFirst = nullptr;
            break;
        case KReportSectionData::Type::PageFooterLast:
            d->pageFooterLast = nullptr;
            break;
        case KReportSectionData::Type::ReportHeader:
            d->reportHeader = nullptr;
            break;
        case KReportSectionData::Type::ReportFooter:
            d->reportFooter = nullptr;
            break;
        default:
            sec = nullptr;
        }

        setModified(true);
        adjustSize();
    }
}

void KReportDesigner::insertSection(KReportSectionData::Type type)
{
    KReportDesignerSection* sec = section(type);
    if (!sec) {
        int idx = 0;
        for (int i = static_cast<int>(KReportSectionData::Type::PageHeaderFirst);
             i <= static_cast<int>(type); ++i)
        {
            if (section(static_cast<KReportSectionData::Type>(i)))
                idx++;
        }
        if (type > KReportSectionData::Type::ReportHeader)
            idx++;
        //kreportDebug() << idx;
        KReportDesignerSection *rs = createSection();
        d->vboxlayout->insertWidget(idx, rs);

        switch (type) {
        case KReportSectionData::Type::PageHeaderAny:
            rs->setTitle(tr("Page Header (Any)"));
            d->pageHeaderAny = rs;
            break;
        case KReportSectionData::Type::PageHeaderEven:
            rs->setTitle(tr("Page Header (Even)"));
            d->pageHeaderEven = rs;
            break;
        case KReportSectionData::Type::PageHeaderOdd:
            rs->setTitle(tr("Page Header (Odd)"));
            d->pageHeaderOdd = rs;
            break;
        case KReportSectionData::Type::PageHeaderFirst:
            rs->setTitle(tr("Page Header (First)"));
            d->pageHeaderFirst = rs;
            break;
        case KReportSectionData::Type::PageHeaderLast:
            rs->setTitle(tr("Page Header (Last)"));
            d->pageHeaderLast = rs;
            break;
        case KReportSectionData::Type::PageFooterAny:
            rs->setTitle(tr("Page Footer (Any)"));
            d->pageFooterAny = rs;
            break;
        case KReportSectionData::Type::PageFooterEven:
            rs->setTitle(tr("Page Footer (Even)"));
            d->pageFooterEven = rs;
            break;
        case KReportSectionData::Type::PageFooterOdd:
            rs->setTitle(tr("Page Footer (Odd)"));
            d->pageFooterOdd = rs;
            break;
        case KReportSectionData::Type::PageFooterFirst:
            rs->setTitle(tr("Page Footer (First)"));
            d->pageFooterFirst = rs;
            break;
        case KReportSectionData::Type::PageFooterLast:
            rs->setTitle(tr("Page Footer (Last)"));
            d->pageFooterLast = rs;
            break;
        case KReportSectionData::Type::ReportHeader:
            rs->setTitle(tr("Report Header"));
            d->reportHeader = rs;
            break;
        case KReportSectionData::Type::ReportFooter:
            rs->setTitle(tr("Report Footer"));
            d->reportFooter = rs;
            break;
            //These sections cannot be inserted this way
        case KReportSectionData::Type::None:
        case KReportSectionData::Type::GroupHeader:
        case KReportSectionData::Type::GroupFooter:
        case KReportSectionData::Type::Detail:
            break;
        }

        rs->show();
        setModified(true);
        adjustSize();
        emit pagePropertyChanged(d->set);
    }
}

void KReportDesigner::setReportTitle(const QString & str)
{
    if (reportTitle() != str) {
        d->title->setValue(str);
        setModified(true);
    }
}

KPropertySet* KReportDesigner::propertySet() const
{
    return &d->set;
}

KPropertySet* KReportDesigner::selectedItemPropertySet() const
{
    return d->itemSet;
}

KReportDataSource *KReportDesigner::reportDataSource() const
{
    return d->dataSource;
}

KReportDesignerSectionDetail * KReportDesigner::detailSection() const
{
    return d->detail;
}

QString KReportDesigner::reportTitle() const
{
    return d->title->value().toString();
}

bool KReportDesigner::isModified() const
{
    return d->modified;
}

void KReportDesigner::setModified(bool modified)
{
    d->modified = modified;

    if (d->modified) {
        emit dirty();
    }
}

QStringList KReportDesigner::fieldNames() const
{
    QStringList qs;
    qs << QString();
    if (d->dataSource)
        qs << d->dataSource->fieldNames();

    return qs;
}

QStringList KReportDesigner::fieldKeys() const
{
    QStringList qs;
    qs << QString();
    if (d->dataSource)
        qs << d->dataSource->fieldKeys();

    return qs;
}

void KReportDesigner::createProperties()
{
    KReportDesigner::addMetaProperties(&d->set,
        tr("Report", "Main report element"), QLatin1String("kreport-report-element"));

    connect(&d->set, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));

    d->title = new KProperty("title", QLatin1String("Report"), tr("Title"), tr("Report Title"));

    KPropertyListData *listData = new KPropertyListData(KReportPageSize::pageFormatKeys(),
                                                        KReportPageSize::pageFormatNames());
    QVariant defaultKey = KReportPageSize::pageSizeKey(KReportPageSize::defaultSize());
    d->pageSize = new KProperty("page-size", listData, defaultKey, tr("Page Size"));

    d->customPageSize = new KProperty("custom-page-size", DEFAULT_CUSTOM_PAGE_SIZE,
        tr("Custom Page Size"), tr("Custom Page Size"), KProperty::SizeF);
    d->customPageSize->setOption("suffix", d->currentUnit.symbol());

    listData = new KPropertyListData({ QLatin1String("portrait"), QLatin1String("landscape") },
                                     QVariantList{ tr("Portrait"), tr("Landscape") });
    d->orientation = new KProperty("print-orientation", listData, QLatin1String("portrait"),
                                   tr("Page Orientation"));

    QList<KReportUnit::Type> types(KReportUnit::allTypes());
    types.removeOne(KReportUnit::Type::Pixel);
    listData = new KPropertyListData(KReportUnit::symbols(types), KReportUnit::descriptions(types));
    d->unit = new KProperty("page-unit", listData, DEFAULT_UNIT_STRING, tr("Page Unit"));
    d->showGrid = new KProperty("grid-visible", true, tr("Show Grid"));
    d->gridSnap = new KProperty("grid-snap", true, tr("Snap to Grid"));
    d->gridDivisions = new KProperty("grid-divisions", 4, tr("Grid Divisions"));
    d->gridDivisions->setOption("min", 1);
    d->gridDivisions->setOption("max", 10);


    d->leftMargin = new KProperty("margin-left", pageUnit().convertFromPoint(KReportUnit::parseValue(DEFAULT_PAGE_MARGIN_STRING)),
        tr("Left Margin"), tr("Left Margin"), KProperty::Double);
    d->rightMargin = new KProperty("margin-right", pageUnit().convertFromPoint(KReportUnit::parseValue(DEFAULT_PAGE_MARGIN_STRING)),
        tr("Right Margin"), tr("Right Margin"), KProperty::Double);
    d->topMargin = new KProperty("margin-top", pageUnit().convertFromPoint(KReportUnit::parseValue(DEFAULT_PAGE_MARGIN_STRING)),
        tr("Top Margin"), tr("Top Margin"), KProperty::Double);
    d->bottomMargin = new KProperty("margin-bottom", pageUnit().convertFromPoint(KReportUnit::parseValue(DEFAULT_PAGE_MARGIN_STRING)),
        tr("Bottom Margin"), tr("Bottom Margin"), KProperty::Double);
    d->leftMargin->setOption("suffix", d->currentUnit.symbol());
    d->rightMargin->setOption("suffix", d->currentUnit.symbol());
    d->topMargin->setOption("suffix", d->currentUnit.symbol());
    d->bottomMargin->setOption("suffix", d->currentUnit.symbol());

    d->set.addProperty(d->title);
    d->set.addProperty(d->pageSize);
    d->set.addProperty(d->customPageSize);
    d->set.addProperty(d->orientation);
    d->set.addProperty(d->unit);
    d->set.addProperty(d->gridSnap);
    d->set.addProperty(d->showGrid);
    d->set.addProperty(d->gridDivisions);
    d->set.addProperty(d->leftMargin);
    d->set.addProperty(d->rightMargin);
    d->set.addProperty(d->topMargin);
    d->set.addProperty(d->bottomMargin);

    recalculateMaxMargins();

#ifdef KREPORT_SCRIPTING
    d->script = new KProperty("script", new KPropertyListData, QVariant(), tr("Object Script"));
    d->set.addProperty(d->script);
#endif
}

/**
@brief Handle property changes
*/
void KReportDesigner::slotPropertyChanged(KPropertySet &s, KProperty &p)
{
    const QSignalBlocker blocker(s);
    setModified(true);
    QByteArray propertyName = p.name();

    if (propertyName == "page-unit") {
        const KReportUnit oldUnit = d->currentUnit;
        d->updateCurrentUnit();
        d->hruler->setUnit(pageUnit());

        // convert values
        d->leftMargin->setValue(KReportUnit::convertFromUnitToUnit(
                                    d->leftMargin->value().toDouble(), oldUnit, d->currentUnit),
                                KProperty::ValueOption::IgnoreOld);

        d->rightMargin->setValue(KReportUnit::convertFromUnitToUnit(
                                     d->rightMargin->value().toDouble(), oldUnit, d->currentUnit),
                                 KProperty::ValueOption::IgnoreOld);

        d->topMargin->setValue(KReportUnit::convertFromUnitToUnit(d->topMargin->value().toDouble(),
                                                                  oldUnit, d->currentUnit),
                               KProperty::ValueOption::IgnoreOld);

        d->bottomMargin->setValue(KReportUnit::convertFromUnitToUnit(
                                      d->bottomMargin->value().toDouble(), oldUnit, d->currentUnit),
                                  KProperty::ValueOption::IgnoreOld);

        d->customPageSize->setValue(
            KReportUnit::convertFromUnitToUnit(d->customPageSize->value().toSizeF(), oldUnit,
                                               d->currentUnit),
            KProperty::ValueOption::IgnoreOld);

        d->leftMargin->setOption("suffix", d->currentUnit.symbol());
        d->rightMargin->setOption("suffix", d->currentUnit.symbol());
        d->topMargin->setOption("suffix", d->currentUnit.symbol());
        d->bottomMargin->setOption("suffix", d->currentUnit.symbol());
        d->customPageSize->setOption("suffix", d->currentUnit.symbol());
    } else if (propertyName.startsWith("margin-") || propertyName == "page-size" || propertyName == "custom-page-size") {
        recalculateMaxMargins();
    }
    emit pagePropertyChanged(s);

}

void KReportDesigner::slotPageButton_Pressed()
{
#ifdef KREPORT_SCRIPTING
    d->updateScripts();
    changeSet(&d->set);
#endif
}

QSize KReportDesigner::sizeHint() const
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

int KReportDesigner::pageWidthPx() const
{
    QSize pageSizePx;
    int pageWidth;

    if (d->set.property("page-size").value().toString() == QLatin1String("Custom")) {
        KReportUnit unit = pageUnit();

        QSizeF customSize = d->currentUnit.convertToPoint(d->set.property("custom-page-size").value().toSizeF());
        QPageLayout layout(QPageSize(customSize, QPageSize::Point, QString(), QPageSize::ExactMatch), d->set.property("print-orientation").value().toString()
                    == QLatin1String("portrait") ? QPageLayout::Portrait : QPageLayout::Landscape, QMarginsF(0,0,0,0));

        pageSizePx = layout.fullRectPixels(KReportPrivate::dpiX()).size();
    } else {
        QPageLayout layout = QPageLayout(
            QPageSize(KReportPageSize::pageSize(d->set.property("page-size").value().toString())),
            d->set.property("print-orientation").value().toString()
                    == QLatin1String("portrait") ? QPageLayout::Portrait : QPageLayout::Landscape, QMarginsF(0,0,0,0));
        pageSizePx = layout.fullRectPixels(KReportPrivate::dpiX()).size();
    }

    pageWidth = pageSizePx.width();

    pageWidth = pageWidth - KReportUnit::convertFromUnitToUnit(d->set.property("margin-left").value().toDouble(), pageUnit(), KReportUnit(KReportUnit::Type::Inch)) * KReportPrivate::dpiX();
    pageWidth = pageWidth - KReportUnit::convertFromUnitToUnit(d->set.property("margin-right").value().toDouble(), pageUnit(), KReportUnit(KReportUnit::Type::Inch)) * KReportPrivate::dpiX();

    return pageWidth;
}

QSize KReportDesigner::pageSizePt() const
{
    QSize pageSizePt;

    if (d->set.property("page-size").value().toString() == QLatin1String("Custom")) {
        KReportUnit unit = pageUnit();

        QSizeF customSize = d->currentUnit.convertToPoint(d->set.property("custom-page-size").value().toSizeF());
        QPageLayout layout(QPageSize(customSize, QPageSize::Point, QString(), QPageSize::ExactMatch), d->set.property("print-orientation").value().toString()
                    == QLatin1String("portrait") ? QPageLayout::Portrait : QPageLayout::Landscape, QMarginsF(0,0,0,0));

        pageSizePt = layout.fullRectPoints().size();
    } else {
        QPageLayout layout = QPageLayout(
            QPageSize(KReportPageSize::pageSize(d->set.property("page-size").value().toString())),
            d->set.property("print-orientation").value().toString()
                    == QLatin1String("portrait") ? QPageLayout::Portrait : QPageLayout::Landscape, QMarginsF(0,0,0,0));
        pageSizePt = layout.fullRectPoints().size();
    }

    return pageSizePt;
}

void KReportDesigner::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);
    d->hruler->setRulerLength(pageWidthPx());
}

void KReportDesigner::setDetail(KReportDesignerSectionDetail *rsd)
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

KReportUnit KReportDesigner::pageUnit() const
{
    return d->currentUnit;
}

void KReportDesigner::setGridOptions(bool vis, int div)
{
    d->showGrid->setValue(QVariant(vis));
    d->gridDivisions->setValue(div);
}

//
// methods for the sectionMouse*Event()
//
void KReportDesigner::sectionContextMenuEvent(KReportDesignerSectionScene * s, QGraphicsSceneContextMenuEvent * e)
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
    if (!d->sectionData.copy_list.isEmpty()) {
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

void KReportDesigner::sectionMousePressEvent(KReportDesignerSectionView * v, QMouseEvent * e)
{
    Q_UNUSED(v);
    d->pressX = e->pos().x();
    d->pressY = e->pos().y();
}

void KReportDesigner::sectionMouseReleaseEvent(KReportDesignerSectionView * v, QMouseEvent * e)
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

        if (d->sectionData.mouseAction == ReportWriterSectionData::MouseAction::Insert) {
            QGraphicsItem * item = nullptr;
            QString classString;
            QString iconName;
            if (d->sectionData.itemToInsert == QLatin1String("org.kde.kreport.line")) {
                item = new KReportDesignerItemLine(v->designer(), v->scene(), pos, end);
                classString = tr("Line", "Report line element");
                iconName = QLatin1String("kreport-line-element");
            }
            else {
                KReportPluginManager* pluginManager = KReportPluginManager::self();
                KReportPluginInterface *plug = pluginManager->plugin(d->sectionData.itemToInsert);
                if (plug) {
                    QObject *obj = plug->createDesignerInstance(v->designer(), v->scene(), pos);
                    if (obj) {
                        item = dynamic_cast<QGraphicsItem*>(obj);
                        classString = plug->metaData()->name();
                        iconName = plug->metaData()->iconName();
                    }
                }
                else {
                    kreportWarning() << "attempted to insert an unknown item";
                }
            }
            if (item) {
                item->setVisible(true);
                item->setSelected(true);
                KReportItemBase* baseReportItem = dynamic_cast<KReportItemBase*>(item);
                if (baseReportItem) {
                    KPropertySet *set = baseReportItem->propertySet();
                    KReportDesigner::addMetaProperties(set, classString, iconName);
                    set->clearModifiedFlags();
                    changeSet(set);
                    if (v && v->designer()) {
                        v->designer()->setModified(true);
                    }
                    emit itemInserted(d->sectionData.itemToInsert);
                }
            }

            d->sectionData.mouseAction = ReportWriterSectionData::MouseAction::None;
            d->sectionData.itemToInsert.clear();
            unsetSectionCursor();
        }
    }
}

unsigned int KReportDesigner::selectionCount() const
{
    if (activeScene())
        return activeScene()->selectedItems().count();
    else
        return 0;
}

void KReportDesigner::changeSet(KPropertySet *s)
{
    //Set the checked state of the report properties button
    if (set == &d->set)
        d->pageButton->setCheckState(Qt::Checked);
    else
        d->pageButton->setCheckState(Qt::Unchecked);

    if (d->itemSet != set) {
        d->itemSet = set;
        emit propertySetChanged();
    }
}

//
// Actions
//

void KReportDesigner::slotItem(const QString &entity)
{
    //kreportDebug() << entity;
    d->sectionData.mouseAction = ReportWriterSectionData::MouseAction::Insert;
    d->sectionData.itemToInsert = entity;
    setSectionCursor(QCursor(Qt::CrossCursor));
}

void KReportDesigner::slotEditDelete()
{
    QGraphicsItem * item = nullptr;
    bool modified = false;
    while (selectionCount() > 0) {
        item = activeScene()->selectedItems().value(0);
        if (item) {
            QGraphicsScene * scene = item->scene();
            delete item;
            scene->update();
            d->sectionData.mouseAction = ReportWriterSectionData::MouseAction::None;
            modified = true;
        }
    }
    activeScene()->selectedItems().clear();

    /*! @todo temporary: clears cut and copy lists to make sure we do not crash
     if weve deleted something in the list
     should really check if an item is in the list first
     and remove it. */
    d->sectionData.cut_list.clear();
    d->sectionData.copy_list.clear();
    if (modified) {
        setModified(true);
    }
}

void KReportDesigner::slotEditCut()
{
    if (selectionCount() > 0) {
        //First delete any items that are curerntly in the list
        //so as not to leak memory
        qDeleteAll(d->sectionData.cut_list);
        d->sectionData.cut_list.clear();

        QGraphicsItem * item = activeScene()->selectedItems().first();
        bool modified = false;
        if (item) {
            d->sectionData.copy_list.clear();
            foreach(QGraphicsItem *item, activeScene()->selectedItems()) {
                d->sectionData.cut_list.append(dynamic_cast<KReportDesignerItemBase*>(item));
                d->sectionData.copy_list.append(dynamic_cast<KReportDesignerItemBase*>(item));
            }
            foreach(QGraphicsItem *item, activeScene()->selectedItems()) {
                activeScene()->removeItem(item);
                activeScene()->update();
                modified = true;
            }
            d->sectionData.selected_x_offset = 10;
            d->sectionData.selected_y_offset = 10;
        }
        if (modified) {
            setModified(true);
        }
    }
}

void KReportDesigner::slotEditCopy()
{
    if (selectionCount() < 1)
        return;

    QGraphicsItem * item = activeScene()->selectedItems().first();
    if (item) {
        d->sectionData.copy_list.clear();
        foreach(QGraphicsItem *item, activeScene()->selectedItems()) {
            d->sectionData.copy_list.append(dynamic_cast<KReportDesignerItemBase*>(item));
        }
        d->sectionData.selected_x_offset = 10;
        d->sectionData.selected_y_offset = 10;
    }
}

void KReportDesigner::slotEditPaste()
{
    // call the editPaste function passing it a reportsection
    slotEditPaste(activeScene());
}

void KReportDesigner::slotEditPaste(QGraphicsScene * canvas)
{

    // paste a new item of the copy we have in the specified location
    if (!d->sectionData.copy_list.isEmpty()) {
        QList<QGraphicsItem*> activeItems = canvas->selectedItems();
        QGraphicsItem *activeItem = nullptr;
        if (activeItems.count() == 1) {
            activeItem = activeItems.first();
        }
        canvas->clearSelection();
        d->sectionData.mouseAction = ReportWriterSectionData::MouseAction::None;

        //! @todo this code sucks :)
        //! The setPos calls only work AFTER the name has been set ?!?!?

        foreach(KReportDesignerItemBase *item, d->sectionData.copy_list) {
            KReportItemBase *obj = dynamic_cast<KReportItemBase*>(item);
            const QString type = obj ? obj->typeName() : QLatin1String("object");
            //kreportDebug() << type;
            KReportDesignerItemBase *ent = item->clone();
            KReportItemBase *new_obj = dynamic_cast<KReportItemBase*>(ent);
            if (new_obj) {
                new_obj->setEntityName(suggestEntityName(type));
                if (activeItem) {
                    new_obj->setPosition(KReportItemBase::positionFromScene(QPointF(activeItem->x() + 10, activeItem->y() + 10)));
                } else {
                    new_obj->setPosition(KReportItemBase::positionFromScene(QPointF(0, 0)));
                }
                new_obj->propertySet()->clearModifiedFlags();
                changeSet(new_obj->propertySet());
            }
            QGraphicsItem *pasted_ent = dynamic_cast<QGraphicsItem*>(ent);
            if (pasted_ent) {
                pasted_ent->setSelected(true);
                canvas->addItem(pasted_ent);
                pasted_ent->show();
                d->sectionData.mouseAction = ReportWriterSectionData::MouseAction::Grab;
                setModified(true);
            }
        }
    }
}
void KReportDesigner::slotRaiseSelected()
{
    dynamic_cast<KReportDesignerSectionScene*>(activeScene())->raiseSelected();
}

void KReportDesigner::slotLowerSelected()
{
    dynamic_cast<KReportDesignerSectionScene*>(activeScene())->lowerSelected();
}

QGraphicsScene* KReportDesigner::activeScene() const
{
    return d->activeScene;
}

void KReportDesigner::setActiveScene(QGraphicsScene* a)
{
    if (d->activeScene && d->activeScene != a)
        d->activeScene->clearSelection();
    d->activeScene = a;

    //Trigger an update so that the last scene redraws its title;
    update();
}

QString KReportDesigner::suggestEntityName(const QString &name) const
{
    KReportDesignerSection *sec;
    int itemCount = 0;
    // Count items in the main sections
    for (int i = static_cast<int>(KReportSectionData::Type::PageHeaderFirst);
         i <= static_cast<int>(KReportSectionData::Type::PageFooterAny); i++)
    {
        sec = section(static_cast<KReportSectionData::Type>(i));
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

    while (!isEntityNameUnique(name + QString::number(itemCount))) {
        itemCount++;
    }
    return name + QString::number(itemCount);
}

bool KReportDesigner::isEntityNameUnique(const QString &name, KReportItemBase* ignore) const
{
    KReportDesignerSection *sec;
    bool unique = true;

    // Check items in the main sections
    for (int i = static_cast<int>(KReportSectionData::Type::PageHeaderFirst);
         i <= static_cast<int>(KReportSectionData::Type::PageFooterAny); i++)
    {
        sec = section(static_cast<KReportSectionData::Type>(i));
        if (sec) {
            const QGraphicsItemList l = sec->items();
            for (QGraphicsItemList::const_iterator it = l.constBegin(); it != l.constEnd(); ++it) {
                KReportItemBase* itm = dynamic_cast<KReportItemBase*>(*it);
                if (itm && itm->entityName() == name  && itm != ignore) {
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
                    KReportItemBase* itm = dynamic_cast<KReportItemBase*>(*it);
                    if (itm && itm->entityName() == name  && itm != ignore) {
                        unique = false;
                        break;
                    }
                }

            }
            sec = d->detail->groupSection(i)->groupFooter();
            if (unique && sec) {
                const QGraphicsItemList l = sec->items();
                for (QGraphicsItemList::const_iterator it = l.constBegin(); it != l.constEnd(); ++it) {
                    KReportItemBase* itm = dynamic_cast<KReportItemBase*>(*it);
                    if (itm && itm->entityName() == name  && itm != ignore) {
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
                KReportItemBase* itm = dynamic_cast<KReportItemBase*>(*it);
                if (itm && itm->entityName() == name  && itm != ignore) {
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

QList<QAction*> KReportDesigner::itemActions(QActionGroup* group)
{
    KReportPluginManager* manager = KReportPluginManager::self();
    QList<QAction*> actList = manager->createActions(group);

    //! @todo make line a real plugin so this isn't needed:
    QAction *act = new QAction(QIcon::fromTheme(QLatin1String("kreport-line-element")), tr("Line"), group);
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

QList< QAction* > KReportDesigner::designerActions()
{
    QList<QAction*> al;
    QAction *sep = new QAction(QString(), this);
    sep->setSeparator(true);

    al << d->editCutAction << d->editCopyAction << d->editPasteAction << d->editDeleteAction << sep << d->sectionEdit << sep << d->itemLowerAction << d->itemRaiseAction;

    return al;
}

void KReportDesigner::createActions()
{
    d->editCutAction = new QAction(QIcon::fromTheme(QLatin1String("edit-cut")), tr("Cu&t"), this);
    d->editCutAction->setObjectName(QLatin1String("edit_cut"));
    d->editCutAction->setToolTip(tr("Cut selection to clipboard"));
    d->editCutAction->setShortcuts(KStandardShortcut::cut());
    d->editCutAction->setProperty("iconOnly", true);

    d->editCopyAction = new QAction(QIcon::fromTheme(QLatin1String("edit-copy")), tr("&Copy"), this);
    d->editCopyAction->setObjectName(QLatin1String("edit_copy"));
    d->editCopyAction->setToolTip(tr("Copy selection to clipboard"));
    d->editCopyAction->setShortcuts(KStandardShortcut::copy());
    d->editCopyAction->setProperty("iconOnly", true);

    d->editPasteAction = new QAction(QIcon::fromTheme(QLatin1String("edit-paste")), tr("&Paste"), this);
    d->editPasteAction->setObjectName(QLatin1String("edit_paste"));
    d->editPasteAction->setToolTip(tr("Paste clipboard content"));
    d->editPasteAction->setShortcuts(KStandardShortcut::paste());
    d->editPasteAction->setProperty("iconOnly", true);

    const KGuiItem del = KStandardGuiItem::del();
    d->editDeleteAction = new QAction(del.icon(), del.text(), this);
    d->editDeleteAction->setObjectName(QLatin1String("edit_delete"));
    d->editDeleteAction->setToolTip(del.toolTip());
    d->editDeleteAction->setWhatsThis(del.whatsThis());
    d->editDeleteAction->setProperty("iconOnly", true);

    d->sectionEdit = new QAction(tr("Edit Sections"), this);
    d->sectionEdit->setObjectName(QLatin1String("section_edit"));

    d->itemRaiseAction = new QAction(QIcon::fromTheme(QLatin1String("arrow-up")), tr("Raise"), this);
    d->itemRaiseAction->setObjectName(QLatin1String("item_raise"));
    d->itemLowerAction = new QAction(QIcon::fromTheme(QLatin1String("arrow-down")), tr("Lower"), this);
    d->itemLowerAction->setObjectName(QLatin1String("item_lower"));

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

void KReportDesigner::setSectionCursor(const QCursor& c)
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

void KReportDesigner::unsetSectionCursor()
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

qreal KReportDesigner::countSelectionHeight() const
{
    if (d->releaseY == -1 || d->pressY == -1) {
        return -1;
    }
    return qAbs(d->releaseY - d->pressY);
}

qreal KReportDesigner::countSelectionWidth() const
{
    if (d->releaseX == -1 || d->pressX == -1) {
        return -1;
    }
    return qAbs(d->releaseX - d->pressX);
}

qreal KReportDesigner::getSelectionPressX() const
{
    return d->pressX;
}

qreal KReportDesigner::getSelectionPressY() const
{
    return d->pressY;
}

QPointF KReportDesigner::getPressPoint() const
{
    return QPointF(d->pressX, d->pressY);
}

QPointF KReportDesigner::getReleasePoint() const
{
    return QPointF(d->releaseX, d->releaseY);
}

void KReportDesigner::plugItemActions(const QList<QAction*> &actList)
{
    foreach(QAction *a, actList) {
        connect(a, SIGNAL(triggered(bool)), this, SLOT(slotItemTriggered(bool)));
    }
}

void KReportDesigner::slotItemTriggered(bool checked)
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

void KReportDesigner::addMetaProperties(KPropertySet* set, const QString &classString,
                                        const QString &iconName)
{
    Q_ASSERT(set);
    KProperty *prop;
    set->addProperty(prop = new KProperty("this:classString", classString));
    prop->setVisible(false);
    set->addProperty(prop = new KProperty("this:iconName", iconName));
    prop->setVisible(false);
}

void KReportDesigner::recalculateMaxMargins()
{
    QSize pageSize = pageSizePt();
    d->leftMargin->setOption("max", d->currentUnit.convertFromPoint(pageSize.width() - d->currentUnit.convertToPoint(d->rightMargin->value().toReal()) - SMALLEST_PAGE_SIZE_PT));
    d->rightMargin->setOption("max", d->currentUnit.convertFromPoint(pageSize.width()  - d->currentUnit.convertToPoint(d->leftMargin->value().toReal())- SMALLEST_PAGE_SIZE_PT));
    d->topMargin->setOption("max", d->currentUnit.convertFromPoint(pageSize.height()  - d->currentUnit.convertToPoint(d->bottomMargin->value().toReal())- SMALLEST_PAGE_SIZE_PT));
    d->bottomMargin->setOption("max", d->currentUnit.convertFromPoint(pageSize.height()  - d->currentUnit.convertToPoint(d->topMargin->value().toReal())- SMALLEST_PAGE_SIZE_PT));
}
