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

#include "KReportPreRenderer.h"
#include "KReportPreRenderer_p.h"
#include "KReportAsyncItemManager_p.h"
#include "KReportOneRecordDataSource_p.h"

#include "KReportRenderObjects.h"
#include "KReportDataSource.h"
#include "KReportItemBase.h"
#include "KReportDocument.h"
#include "KReportDetailSectionData.h"
#include "KReportLabelSizeInfo.h"
#include "KReportPageSize.h"
#include "KReportUtils_p.h"

#ifdef KREPORT_SCRIPTING
#include "scripting/KReportScriptHandler.h"
#include "scripting/KReportGroupTracker.h"
#endif

#include <QDomElement>
#include <QApplication>
#include "kreport_debug.h"

KReportPreRendererPrivate::KReportPreRendererPrivate(KReportPreRenderer *preRenderer)
 : preRenderer(preRenderer)
{
    valid = false;
    document = nullptr;
    reportDocument = nullptr;
    page = nullptr;
    yOffset = 0.0;
    topMargin = bottomMargin = 0.0;
    leftMargin = rightMargin = 0.0;
    pageCounter = 0;
    maxHeight = maxWidth = 0.0;
    oneRecord = new KReportPrivate::OneRecordDataSource();
    dataSource = nullptr;
#ifdef KREPORT_SCRIPTING
    scriptHandler = nullptr;
#endif
    asyncManager = new KReportPrivate::AsyncItemManager(this);

    connect(asyncManager, SIGNAL(finished()), this, SLOT(asyncItemsFinished()));
}

KReportPreRendererPrivate::~KReportPreRendererPrivate()
{
    delete reportDocument;
    delete document;
    delete oneRecord;
    postProcText.clear();
}

void KReportPreRendererPrivate::createNewPage()
{
    //kreportDebug();
    if (pageCounter > 0)
        finishCurPage(false);

    pageCounter++;

#ifdef KREPORT_SCRIPTING
    //Update the page count script value
    scriptHandler->setPageNumber(pageCounter);
    scriptHandler->newPage();
#endif

    page = new OROPage(nullptr);
    document->addPage(page);

    //! @todo calculate past page
    bool lastPage = false;

    yOffset = topMargin;

    if (pageCounter == 1 && reportDocument->section(KReportSectionData::Type::PageHeaderFirst))
        renderSection(*(reportDocument->section(KReportSectionData::Type::PageHeaderFirst)));
    else if (lastPage == true && reportDocument->section(KReportSectionData::Type::PageHeaderLast))
        renderSection(*(reportDocument->section(KReportSectionData::Type::PageHeaderLast)));
    else if ((pageCounter % 2) == 1 && reportDocument->section(KReportSectionData::Type::PageHeaderOdd))
        renderSection(*(reportDocument->section(KReportSectionData::Type::PageHeaderOdd)));
    else if ((pageCounter % 2) == 0 && reportDocument->section(KReportSectionData::Type::PageHeaderEven))
        renderSection(*(reportDocument->section(KReportSectionData::Type::PageHeaderEven)));
    else if (reportDocument->section(KReportSectionData::Type::PageHeaderAny))
        renderSection(*(reportDocument->section(KReportSectionData::Type::PageHeaderAny)));
}

qreal KReportPreRendererPrivate::finishCurPageSize(bool lastPage)
{
    qreal retval = 0.0;

    if (lastPage && reportDocument->section(KReportSectionData::Type::PageFooterLast))
        retval = renderSectionSize(* (reportDocument->section(KReportSectionData::Type::PageFooterLast)));
    else if (pageCounter == 1 && reportDocument->section(KReportSectionData::Type::PageFooterFirst))
        retval = renderSectionSize(* (reportDocument->section(KReportSectionData::Type::PageFooterFirst)));
    else if ((pageCounter % 2) == 1 && reportDocument->section(KReportSectionData::Type::PageFooterOdd))
        retval = renderSectionSize(* (reportDocument->section(KReportSectionData::Type::PageFooterOdd)));
    else if ((pageCounter % 2) == 0 && reportDocument->section(KReportSectionData::Type::PageFooterEven))
        retval = renderSectionSize(* (reportDocument->section(KReportSectionData::Type::PageFooterEven)));
    else if (reportDocument->section(KReportSectionData::Type::PageFooterAny))
        retval = renderSectionSize(* (reportDocument->section(KReportSectionData::Type::PageFooterAny)));

    //kreportDebug() << retval;
    return retval;
}

qreal KReportPreRendererPrivate::finishCurPage(bool lastPage)
{

    qreal offset = maxHeight - bottomMargin;
    qreal retval = 0.0;
    //kreportDebug() << offset;

    if (lastPage && reportDocument->section(KReportSectionData::Type::PageFooterLast)) {
        //kreportDebug() << "Last Footer";
        yOffset = offset - renderSectionSize(* (reportDocument->section(KReportSectionData::Type::PageFooterLast)));
        retval = renderSection(* (reportDocument->section(KReportSectionData::Type::PageFooterLast)));
    } else if (pageCounter == 1 && reportDocument->section(KReportSectionData::Type::PageFooterFirst)) {
        //kreportDebug() << "First Footer";
        yOffset = offset - renderSectionSize(* (reportDocument->section(KReportSectionData::Type::PageFooterLast)));
        retval = renderSection(* (reportDocument->section(KReportSectionData::Type::PageFooterFirst)));
    } else if ((pageCounter % 2) == 1 && reportDocument->section(KReportSectionData::Type::PageFooterOdd)) {
        //kreportDebug() << "Odd Footer";
        yOffset = offset - renderSectionSize(* (reportDocument->section(KReportSectionData::Type::PageFooterOdd)));
        retval = renderSection(* (reportDocument->section(KReportSectionData::Type::PageFooterOdd)));
    } else if ((pageCounter % 2) == 0 && reportDocument->section(KReportSectionData::Type::PageFooterEven)) {
        //kreportDebug() << "Even Footer";
        yOffset = offset - renderSectionSize(* (reportDocument->section(KReportSectionData::Type::PageFooterEven)));
        retval = renderSection(* (reportDocument->section(KReportSectionData::Type::PageFooterEven)));
    } else if (reportDocument->section(KReportSectionData::Type::PageFooterAny)) {
        //kreportDebug() << "Any Footer";
        yOffset = offset - renderSectionSize(* (reportDocument->section(KReportSectionData::Type::PageFooterAny)));
        retval = renderSection(* (reportDocument->section(KReportSectionData::Type::PageFooterAny)));
    }

    return retval;
}

void KReportPreRendererPrivate::renderDetailSection(KReportDetailSectionData *detailData)
{
    if (detailData->detailSection) {
        if (dataSource/* && !curs->eof()*/) {
            QStringList keys;
            QStringList keyValues;
            QList<int> shownGroups;
            KReportDetailGroupSectionData * grp = nullptr;

            bool status = dataSource->moveFirst();
            int recordCount = dataSource->recordCount();

            //kreportDebug() << "Record Count:" << recordCount;

            for (int i = 0; i < (int) detailData->groupList.count(); ++i) {
                grp = detailData->groupList[i];
                //If the group has a header or footer, then emit a change of group value
                if(grp->groupFooter || grp->groupHeader) {
                    // we get here only if group is *shown*
                    shownGroups << i;
                    keys.append(grp->column);
                    if (!keys.last().isEmpty())
                        keyValues.append(dataSource->value(dataSource->fieldNumber(keys.last())).toString());
                    else
                        keyValues.append(QString());

                    //Tell interested parties we're about to render a header
                    emit(enteredGroup(keys.last(), keyValues.last()));
                }
                if (grp->groupHeader)
                    renderSection(*(grp->groupHeader));
            }

            while (status) {
                const qint64 pos = dataSource->at();
                //kreportDebug() << "At:" << l << "Y:" << yOffset << "Max Height:" << maxHeight;
                if ((renderSectionSize(*detailData->detailSection)
                        + finishCurPageSize((pos + 1 == recordCount))
                        + bottomMargin + yOffset) >= maxHeight)
                {
                    //kreportDebug() << "Next section is too big for this page";
                    if (pos > 0) {
                        dataSource->movePrevious();
                        createNewPage();
                        dataSource->moveNext();
                    }
                }

                renderSection(*(detailData->detailSection));
                status = dataSource->moveNext();

                if (status == true && keys.count() > 0) {
                    // check to see where it is we need to start
                    int pos = -1; // if it's still -1 by the time we are done then no keyValues changed
                    for (int i = 0; i < keys.count(); ++i) {
                        if (keyValues[i] != dataSource->value(dataSource->fieldNumber(keys[i])).toString()) {
                            pos = i;
                            break;
                        }
                    }
                    // don't bother if nothing has changed
                    if (pos != -1) {
                        // roll back the query and go ahead if all is good
                        status = dataSource->movePrevious();
                        if (status == true) {
                            // print the footers as needed
                            // any changes made in this for loop need to be duplicated
                            // below where the footers are finished.
                            bool do_break = false;
                            for (int i = shownGroups.count() - 1; i >= 0; i--) {
                                if (do_break)
                                    createNewPage();
                                do_break = false;
                                grp = detailData->groupList[shownGroups.at(i)];

                                if (grp->groupFooter) {
                                    if (renderSectionSize(*(grp->groupFooter)) + finishCurPageSize(false) + bottomMargin + yOffset >= maxHeight)
                                        createNewPage();
                                    renderSection(*(grp->groupFooter));
                                }

                                if (KReportDetailGroupSectionData::PageBreak::AfterGroupFooter == grp->pagebreak)
                                    do_break = true;
                            }
                            // step ahead to where we should be and print the needed headers
                            // if all is good
                            status = dataSource->moveNext();
                            if (do_break)
                                createNewPage();
                            if (status == true) {
                                for (int i = 0; i < shownGroups.count(); ++i) {
                                    grp = detailData->groupList[shownGroups.at(i)];

                                    if (grp->groupHeader) {
                                        if (renderSectionSize(*(grp->groupHeader)) + finishCurPageSize(false) + bottomMargin + yOffset >= maxHeight) {
                                            dataSource->movePrevious();
                                            createNewPage();
                                            dataSource->moveNext();
                                        }

                                        if (!keys[i].isEmpty())
                                            keyValues[i] = dataSource->value(dataSource->fieldNumber(keys[i])).toString();

                                        //Tell interested parties thak key values changed
                                        renderSection(*(grp->groupHeader));
                                    }


                                }
                            }
                        }
                    }
                }
            }

            if (keys.size() > 0 && dataSource->movePrevious()) {
                // finish footers
                // duplicated changes from above here
                for (int i = shownGroups.count() - 1; i >= 0; i--) {
                    grp = detailData->groupList[shownGroups.at(i)];

                    if (grp->groupFooter) {
                        if (renderSectionSize(*(grp->groupFooter)) + finishCurPageSize(false) + bottomMargin + yOffset >= maxHeight)
                            createNewPage();
                        renderSection(*(grp->groupFooter));
                        emit(exitedGroup(keys[i], keyValues[i]));
                    }
                }
            }
        }
        if (KReportDetailSectionData::PageBreak::AtEnd == detailData->pageBreak)
            createNewPage();
    }
}

qreal KReportPreRendererPrivate::renderSectionSize(const KReportSectionData & sectionData)
{
    qreal intHeight = POINT_TO_INCH(sectionData.heightValue()) * KReportPrivate::dpiX();

    if (sectionData.objects().count() == 0)
        return intHeight;

    QList<KReportItemBase*> objects = sectionData.objects();
    foreach(KReportItemBase *ob, objects) {
        QPointF offset(leftMargin, yOffset);
        //ASync objects cannot alter the section height
        KReportAsyncItemBase *async_ob = qobject_cast<KReportAsyncItemBase*>(ob);
        if (!async_ob) {
            QVariant itemData;
            if (dataSource) {
                itemData = dataSource->value(ob->itemDataSource());
            }
            const int itemHeight = ob->renderSimpleData(nullptr, nullptr, offset, itemData, scriptHandler);
            if (itemHeight > intHeight) {
                intHeight = itemHeight;
            }
        }
    }

    return intHeight;
}

qreal KReportPreRendererPrivate::renderSection(const KReportSectionData & sectionData)
{
    qreal sectionHeight = POINT_TO_INCH(sectionData.heightValue()) * KReportPrivate::dpiX();

    int itemHeight = 0;
    //kreportDebug() << "Name: " << sectionData.name() << " Height: " << sectionHeight
    //         << "Objects: " << sectionData.objects().count();
    emit(renderingSection(const_cast<KReportSectionData*>(&sectionData), page, QPointF(leftMargin, yOffset)));

    //Create a pre-rendered section for this section and add it to the document
    OROSection *sec = new OROSection(document);
    sec->setHeight(sectionData.heightValue());
    sec->setBackgroundColor(sectionData.backgroundColorValue());
    sec->setType(sectionData.type());
    document->addSection(sec);

    //Render section background
    ORORect* bg = new ORORect();
    bg->setPen(QPen(Qt::NoPen));
    bg->setBrush(sectionData.backgroundColorValue());
    qreal w = page->document()->pageLayout().fullRectPixels(KReportPrivate::dpiX()).width() - page->document()->pageLayout().marginsPixels(KReportPrivate::dpiX()).right() - leftMargin;

    bg->setRect(QRectF(leftMargin, yOffset, w, sectionHeight));
    page->insertPrimitive(bg, true);

    QList<KReportItemBase*> objects = sectionData.objects();
    foreach(KReportItemBase *ob, objects) {
        QPointF offset(leftMargin, yOffset);
        QVariant itemData = dataSource->value(ob->itemDataSource());

        if (ob->supportsSubQuery()) {
           itemHeight = ob->renderReportData(page, sec, offset, dataSource, scriptHandler);
        } else {
            KReportAsyncItemBase *async_ob = qobject_cast<KReportAsyncItemBase*>(ob);
            if (async_ob){
                //kreportDebug() << "async object";
                asyncManager->addItem(async_ob, page, sec, offset, async_ob->realItemData(itemData), scriptHandler);
            } else {
                //kreportDebug() << "sync object";
                itemHeight = ob->renderSimpleData(page, sec, offset, itemData, scriptHandler);
            }
        }

        if (itemHeight > sectionHeight) {
            sectionHeight = itemHeight;
        }
    }
    for (int i = 0; i < page->primitiveCount(); ++i) {
        OROPrimitive *prim = page->primitive(i);
        if (OROTextBox *text = dynamic_cast<OROTextBox*>(prim)) {
            if (text->requiresPostProcessing()) {
                postProcText.append(text);
            }
        }
    }
    yOffset += sectionHeight;

    return sectionHeight;
}

#ifdef KREPORT_SCRIPTING
void KReportPreRendererPrivate::initEngine()
{
    delete scriptHandler;
    scriptHandler = new KReportScriptHandler(dataSource, scriptSource, reportDocument);

    connect(this, SIGNAL(enteredGroup(QString,QVariant)), scriptHandler, SLOT(slotEnteredGroup(QString,QVariant)));

    connect(this, SIGNAL(exitedGroup(QString,QVariant)), scriptHandler, SLOT(slotExitedGroup(QString,QVariant)));

    connect(this, SIGNAL(renderingSection(KReportSectionData*,OROPage*,QPointF)), scriptHandler, SLOT(slotEnteredSection(KReportSectionData*,OROPage*,QPointF)));
}
#endif

void KReportPreRendererPrivate::asyncItemsFinished()
{
    //kreportDebug() << "Finished rendering async items";
    asyncManager->deleteLater();
    emit finishedAllASyncItems();
}

bool KReportPreRendererPrivate::generateDocument()
{
    if (!dataSource) {
        dataSource = oneRecord;
    }

    if (!valid || !reportDocument) {
        return false;
    }

    // Do this check now so we don't have to undo a lot of work later if it fails
    KReportLabelSizeInfo label;
    if (reportDocument->pageSize() == QLatin1String("Labels")) {
        label = KReportLabelSizeInfo::find(reportDocument->labelType());
        if (label.isNull()) {
            return false;
        }
    }
    //kreportDebug() << "Creating Document";
    document = new ORODocument(reportDocument->title());

    pageCounter  = 0;
    yOffset      = 0.0;

    //kreportDebug() << "Calculating Margins";
    if (!label.isNull()) {
        if (reportDocument->pageLayout().orientation() == QPageLayout::Portrait) {
            topMargin = (label.startY() / 100.0);
            bottomMargin = 0;
            rightMargin = 0;
            leftMargin = (label.startX() / 100.0);
        } else {
            topMargin = (label.startX() / 100.0);
            bottomMargin = 0;
            rightMargin = 0;
            leftMargin = (label.startY() / 100.0);
        }
    } else {

        topMargin    = reportDocument->pageLayout().marginsPoints().top();
        bottomMargin = reportDocument->pageLayout().marginsPoints().bottom();
        rightMargin  = reportDocument->pageLayout().marginsPoints().right();
        leftMargin   = reportDocument->pageLayout().marginsPoints().left();
        //kreportDebug() << "Margins:" << topMargin << bottomMargin << rightMargin << leftMargin;
     }

    //kreportDebug() << "Calculating Page Size";
    QPageLayout layout =  reportDocument->pageLayout();
    // This should reflect the information of the report page size
    if (reportDocument->pageSize() == QLatin1String("Custom")) {
        maxWidth = reportDocument->pageLayout().fullRectPoints().width();
        maxHeight = reportDocument->pageLayout().fullRectPoints().height();
    } else {
        if (!label.isNull()) {
            maxWidth = label.width();
            maxHeight = label.height();
            reportDocument->pageLayout().setPageSize(QPageSize(KReportPageSize::pageSize(label.paper())));
        } else {
            // lookup the correct size information for the specified size paper
            QSizeF pageSizePx = reportDocument->pageLayout().fullRectPixels(KReportPrivate::dpiX()).size();

            maxWidth = pageSizePx.width();
            maxHeight = pageSizePx.height();
        }
    }

    if (reportDocument->pageLayout().orientation() == QPageLayout::Landscape) {
        qreal tmp = maxWidth;
        maxWidth = maxHeight;
        maxHeight = tmp;
    }

    //kreportDebug() << "Page Size:" << maxWidth << maxHeight;

    document->setPageLayout(reportDocument->pageLayout());
    dataSource->setSorting(reportDocument->detail()->sortedFields);
    if (!dataSource->open()) {
        return false;
    }

    #ifdef KREPORT_SCRIPTING
    initEngine();
    connect(scriptHandler, SIGNAL(groupChanged(QMap<QString, QVariant>)),
            preRenderer, SIGNAL(groupChanged(QMap<QString, QVariant>)));

    //Loop through all abjects that have been registered, and register them with the script handler
    if (scriptHandler) {
        QMapIterator<QString, QObject*> i(scriptObjects);
        while (i.hasNext()) {
            i.next();
            scriptHandler->registerScriptObject(i.value(), i.key());
        }
        //execute the script, if it fails, abort and return the empty document
        if (!scriptHandler->trigger()) {
            scriptHandler->displayErrors();
            return document;
        }
    }
    #endif

    createNewPage();
    if (!label.isNull()) {
        // Label Print Run
        // remember the initial margin setting as we will be modifying
        // the value and restoring it as we move around
        qreal margin = leftMargin;

        yOffset = topMargin;

        qreal w = (label.width() / 100.0);
        qreal wg = (label.xGap() / 100.0);
        qreal h = (label.height() / 100.0);
        qreal hg = (label.yGap() / 100.0);
        int numCols = label.columns();
        int numRows = label.rows();
        qreal tmp;

        // flip the value around if we are printing landscape
        if (!(reportDocument->pageLayout().orientation() == QPageLayout::Portrait)) {
            w = (label.height() / 100.0);
            wg = (label.yGap() / 100.0);
            h = (label.width() / 100.0);
            hg = (label.xGap() / 100.0);
            numCols = label.rows();
            numRows = label.columns();
        }

        KReportDetailSectionData * detailData = reportDocument->detail();
        if (detailData->detailSection) {
            KReportDataSource *mydata = dataSource;

            if (mydata && mydata->recordCount() > 0) { /* && !((query = orqThis->getQuery())->eof()))*/
                if (!mydata->moveFirst()) {
                    return false;
                }
                int row = 0;
                int col = 0;
                do {
                    tmp = yOffset; // store the value as renderSection changes it
                    renderSection(*(detailData->detailSection));
                    yOffset = tmp; // restore the value that renderSection modified

                    col++;
                    leftMargin += w + wg;
                    if (col >= numCols) {
                        leftMargin = margin; // reset back to original value
                        col = 0;
                        row++;
                        yOffset += h + hg;
                        if (row >= numRows) {
                            yOffset = topMargin;
                            row = 0;
                            createNewPage();
                        }
                    }
                } while (mydata->moveNext());
            }
        }

    } else {
        // Normal Print Run
        if (reportDocument->section(KReportSectionData::Type::ReportHeader)) {
            renderSection(*(reportDocument->section(KReportSectionData::Type::ReportHeader)));
        }

        if (reportDocument->detail()) {
            renderDetailSection(reportDocument->detail());
        }

        if (reportDocument->section(KReportSectionData::Type::ReportFooter)) {
            if (renderSectionSize(*(reportDocument->section(KReportSectionData::Type::ReportFooter))) + finishCurPageSize(true) + bottomMargin + yOffset >= maxHeight) {
                createNewPage();
            }
            renderSection(*(reportDocument->section(KReportSectionData::Type::ReportFooter)));
        }
    }
    finishCurPage(true);

    #ifdef KREPORT_SCRIPTING
    // _postProcText contains those text boxes that need to be updated
    // with information that wasn't available at the time it was added to the document
    scriptHandler->setPageTotal(document->pageCount());

    for (int i = 0; i < postProcText.size(); i++) {
        OROTextBox * tb = postProcText.at(i);

        scriptHandler->setPageNumber(tb->page()->pageNumber() + 1);

        tb->setText(scriptHandler->evaluate(tb->text()).toString());
    }
    #endif

    asyncManager->startRendering();

    #ifdef KREPORT_SCRIPTING
    scriptHandler->displayErrors();
    #endif

    if (!dataSource->close()) {
        return false;
    }
    #ifdef KREPORT_SCRIPTING
    delete scriptHandler;
    scriptHandler = nullptr;
    #endif

    if (dataSource != oneRecord) {
        delete dataSource;
        dataSource = nullptr;
    }
    postProcText.clear();

    return true;
}

//===========================KReportPreRenderer===============================

KReportPreRenderer::KReportPreRenderer(const QDomElement &document) : d(new KReportPreRendererPrivate(this))
{
    setDocument(document);
    connect(d, &KReportPreRendererPrivate::finishedAllASyncItems, this, &KReportPreRenderer::finishedAllASyncItems);
}

KReportPreRenderer::~KReportPreRenderer()
{
    delete d;
}

void KReportPreRenderer::setName(const QString &n)
{
    d->reportDocument->setName(n);
}

bool KReportPreRenderer::isValid() const
{
    if (d && d->valid)
        return true;
    return false;
}

ORODocument* KReportPreRenderer::document()
{
    return d->document;
}

bool KReportPreRenderer::generateDocument()
{
//    delete d->document;
    if (!d->generateDocument()) {
        delete d->document;
        d->document = nullptr;
    }
    return d->document;
}

void KReportPreRenderer::setDataSource(KReportDataSource *dataSource)
{
    if (d && dataSource != d->dataSource) {
        delete d->dataSource;
        d->dataSource = dataSource;
    }
}

void KReportPreRenderer::setScriptSource(KReportScriptSource *source)
{
    if (d) {
        d->scriptSource = source;
    }
}


bool KReportPreRenderer::setDocument(const QDomElement &document)
{
    delete d->document;
    d->valid = false;

    if (document.tagName() != QLatin1String("report:content")) {
        kreportWarning() << "report schema is invalid";
        return false;
    }

    d->reportDocument = new KReportDocument(document);
    d->valid = d->reportDocument->isValid();
    return isValid();
}

#ifdef KREPORT_SCRIPTING
void KReportPreRenderer::registerScriptObject(QObject* obj, const QString& name)
{
    //kreportDebug() << name;
    d->scriptObjects[name] = obj;
}

KReportScriptHandler *KReportPreRenderer::scriptHandler()
{
    return d->scriptHandler;
}
#endif

const KReportDocument* KReportPreRenderer::reportData() const
{
    return d->reportDocument;
}
