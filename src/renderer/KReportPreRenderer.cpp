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
#include "KReportScriptHandler.h"
#include "KReportGroupTracker.h"
#endif

#include <QDomElement>
#include <QApplication>
#include "kreport_debug.h"

KReportPreRendererPrivate::KReportPreRendererPrivate(KReportPreRenderer *preRenderer)
 : m_preRenderer(preRenderer)
{
    m_valid = false;
    m_document = nullptr;
    m_reportDocument = nullptr;
    m_page = nullptr;
    m_yOffset = 0.0;
    m_topMargin = m_bottomMargin = 0.0;
    m_leftMargin = m_rightMargin = 0.0;
    m_pageCounter = 0;
    m_maxHeight = m_maxWidth = 0.0;
    m_oneRecord = new KReportPrivate::OneRecordDataSource();
    m_dataSource = nullptr;
#ifdef KREPORT_SCRIPTING
    m_scriptHandler = nullptr;
#endif
    asyncManager = new KReportPrivate::AsyncItemManager(this);

    connect(asyncManager, SIGNAL(finished()), this, SLOT(asyncItemsFinished()));
}

KReportPreRendererPrivate::~KReportPreRendererPrivate()
{
    delete m_reportDocument;
    delete m_document;
    delete m_oneRecord;
    m_postProcText.clear();
}

void KReportPreRendererPrivate::createNewPage()
{
    //kreportDebug();
    if (m_pageCounter > 0)
        finishCurPage(false);

    m_pageCounter++;

#ifdef KREPORT_SCRIPTING
    //Update the page count script value
    m_scriptHandler->setPageNumber(m_pageCounter);
    m_scriptHandler->newPage();
#endif

    m_page = new OROPage(nullptr);
    m_document->addPage(m_page);

    //! @todo calculate past page
    bool lastPage = false;

    m_yOffset = m_topMargin;

    if (m_pageCounter == 1 && m_reportDocument->section(KReportSectionData::Type::PageHeaderFirst))
        renderSection(*(m_reportDocument->section(KReportSectionData::Type::PageHeaderFirst)));
    else if (lastPage == true && m_reportDocument->section(KReportSectionData::Type::PageHeaderLast))
        renderSection(*(m_reportDocument->section(KReportSectionData::Type::PageHeaderLast)));
    else if ((m_pageCounter % 2) == 1 && m_reportDocument->section(KReportSectionData::Type::PageHeaderOdd))
        renderSection(*(m_reportDocument->section(KReportSectionData::Type::PageHeaderOdd)));
    else if ((m_pageCounter % 2) == 0 && m_reportDocument->section(KReportSectionData::Type::PageHeaderEven))
        renderSection(*(m_reportDocument->section(KReportSectionData::Type::PageHeaderEven)));
    else if (m_reportDocument->section(KReportSectionData::Type::PageHeaderAny))
        renderSection(*(m_reportDocument->section(KReportSectionData::Type::PageHeaderAny)));
}

qreal KReportPreRendererPrivate::finishCurPageSize(bool lastPage)
{
    qreal retval = 0.0;

    if (lastPage && m_reportDocument->section(KReportSectionData::Type::PageFooterLast))
        retval = renderSectionSize(* (m_reportDocument->section(KReportSectionData::Type::PageFooterLast)));
    else if (m_pageCounter == 1 && m_reportDocument->section(KReportSectionData::Type::PageFooterFirst))
        retval = renderSectionSize(* (m_reportDocument->section(KReportSectionData::Type::PageFooterFirst)));
    else if ((m_pageCounter % 2) == 1 && m_reportDocument->section(KReportSectionData::Type::PageFooterOdd))
        retval = renderSectionSize(* (m_reportDocument->section(KReportSectionData::Type::PageFooterOdd)));
    else if ((m_pageCounter % 2) == 0 && m_reportDocument->section(KReportSectionData::Type::PageFooterEven))
        retval = renderSectionSize(* (m_reportDocument->section(KReportSectionData::Type::PageFooterEven)));
    else if (m_reportDocument->section(KReportSectionData::Type::PageFooterAny))
        retval = renderSectionSize(* (m_reportDocument->section(KReportSectionData::Type::PageFooterAny)));

    //kreportDebug() << retval;
    return retval;
}

qreal KReportPreRendererPrivate::finishCurPage(bool lastPage)
{

    qreal offset = m_maxHeight - m_bottomMargin;
    qreal retval = 0.0;
    //kreportDebug() << offset;

    if (lastPage && m_reportDocument->section(KReportSectionData::Type::PageFooterLast)) {
        //kreportDebug() << "Last Footer";
        m_yOffset = offset - renderSectionSize(* (m_reportDocument->section(KReportSectionData::Type::PageFooterLast)));
        retval = renderSection(* (m_reportDocument->section(KReportSectionData::Type::PageFooterLast)));
    } else if (m_pageCounter == 1 && m_reportDocument->section(KReportSectionData::Type::PageFooterFirst)) {
        //kreportDebug() << "First Footer";
        m_yOffset = offset - renderSectionSize(* (m_reportDocument->section(KReportSectionData::Type::PageFooterFirst)));
        retval = renderSection(* (m_reportDocument->section(KReportSectionData::Type::PageFooterFirst)));
    } else if ((m_pageCounter % 2) == 1 && m_reportDocument->section(KReportSectionData::Type::PageFooterOdd)) {
        //kreportDebug() << "Odd Footer";
        m_yOffset = offset - renderSectionSize(* (m_reportDocument->section(KReportSectionData::Type::PageFooterOdd)));
        retval = renderSection(* (m_reportDocument->section(KReportSectionData::Type::PageFooterOdd)));
    } else if ((m_pageCounter % 2) == 0 && m_reportDocument->section(KReportSectionData::Type::PageFooterEven)) {
        //kreportDebug() << "Even Footer";
        m_yOffset = offset - renderSectionSize(* (m_reportDocument->section(KReportSectionData::Type::PageFooterEven)));
        retval = renderSection(* (m_reportDocument->section(KReportSectionData::Type::PageFooterEven)));
    } else if (m_reportDocument->section(KReportSectionData::Type::PageFooterAny)) {
        //kreportDebug() << "Any Footer";
        m_yOffset = offset - renderSectionSize(* (m_reportDocument->section(KReportSectionData::Type::PageFooterAny)));
        retval = renderSection(* (m_reportDocument->section(KReportSectionData::Type::PageFooterAny)));
    }

    return retval;
}

void KReportPreRendererPrivate::renderDetailSection(KReportDetailSectionData *detailData)
{
    if (detailData->detailSection) {
        if (m_dataSource/* && !curs->eof()*/) {
            QStringList keys;
            QStringList keyValues;
            QList<int> shownGroups;
            KReportDetailGroupSectionData * grp = nullptr;

            bool status = m_dataSource->moveFirst();
            int recordCount = m_dataSource->recordCount();

            //kreportDebug() << "Record Count:" << recordCount;

            for (int i = 0; i < (int) detailData->groupList.count(); ++i) {
                grp = detailData->groupList[i];
                //If the group has a header or footer, then emit a change of group value
                if(grp->groupFooter || grp->groupHeader) {
                    // we get here only if group is *shown*
                    shownGroups << i;
                    keys.append(grp->column);
                    if (!keys.last().isEmpty())
                        keyValues.append(m_dataSource->value(m_dataSource->fieldNumber(keys.last())).toString());
                    else
                        keyValues.append(QString());

                    //Tell interested parties we're about to render a header
                    emit(enteredGroup(keys.last(), keyValues.last()));
                }
                if (grp->groupHeader)
                    renderSection(*(grp->groupHeader));
            }

            while (status) {
                const qint64 pos = m_dataSource->at();
                //kreportDebug() << "At:" << l << "Y:" << m_yOffset << "Max Height:" << m_maxHeight;
                if ((renderSectionSize(*detailData->detailSection)
                        + finishCurPageSize((pos + 1 == recordCount))
                        + m_bottomMargin + m_yOffset) >= m_maxHeight)
                {
                    //kreportDebug() << "Next section is too big for this page";
                    if (pos > 0) {
                        m_dataSource->movePrevious();
                        createNewPage();
                        m_dataSource->moveNext();
                    }
                }

                renderSection(*(detailData->detailSection));
                status = m_dataSource->moveNext();

                if (status == true && keys.count() > 0) {
                    // check to see where it is we need to start
                    int pos = -1; // if it's still -1 by the time we are done then no keyValues changed
                    for (int i = 0; i < keys.count(); ++i) {
                        if (keyValues[i] != m_dataSource->value(m_dataSource->fieldNumber(keys[i])).toString()) {
                            pos = i;
                            break;
                        }
                    }
                    // don't bother if nothing has changed
                    if (pos != -1) {
                        // roll back the query and go ahead if all is good
                        status = m_dataSource->movePrevious();
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
                                    if (renderSectionSize(*(grp->groupFooter)) + finishCurPageSize(false) + m_bottomMargin + m_yOffset >= m_maxHeight)
                                        createNewPage();
                                    renderSection(*(grp->groupFooter));
                                }

                                if (KReportDetailGroupSectionData::PageBreak::AfterGroupFooter == grp->pagebreak)
                                    do_break = true;
                            }
                            // step ahead to where we should be and print the needed headers
                            // if all is good
                            status = m_dataSource->moveNext();
                            if (do_break)
                                createNewPage();
                            if (status == true) {
                                for (int i = 0; i < shownGroups.count(); ++i) {
                                    grp = detailData->groupList[shownGroups.at(i)];

                                    if (grp->groupHeader) {
                                        if (renderSectionSize(*(grp->groupHeader)) + finishCurPageSize(false) + m_bottomMargin + m_yOffset >= m_maxHeight) {
                                            m_dataSource->movePrevious();
                                            createNewPage();
                                            m_dataSource->moveNext();
                                        }

                                        if (!keys[i].isEmpty())
                                            keyValues[i] = m_dataSource->value(m_dataSource->fieldNumber(keys[i])).toString();

                                        //Tell interested parties thak key values changed
                                        renderSection(*(grp->groupHeader));
                                    }


                                }
                            }
                        }
                    }
                }
            }

            if (keys.size() > 0 && m_dataSource->movePrevious()) {
                // finish footers
                // duplicated changes from above here
                for (int i = shownGroups.count() - 1; i >= 0; i--) {
                    grp = detailData->groupList[shownGroups.at(i)];

                    if (grp->groupFooter) {
                        if (renderSectionSize(*(grp->groupFooter)) + finishCurPageSize(false) + m_bottomMargin + m_yOffset >= m_maxHeight)
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
    qreal intHeight = POINT_TO_INCH(sectionData.height()) * KReportPrivate::dpiX();

    if (sectionData.objects().count() == 0)
        return intHeight;

    QList<KReportItemBase*> objects = sectionData.objects();
    foreach(KReportItemBase *ob, objects) {
        QPointF offset(m_leftMargin, m_yOffset);
        //ASync objects cannot alter the section height
        KReportAsyncItemBase *async_ob = qobject_cast<KReportAsyncItemBase*>(ob);
        if (!async_ob) {
            QVariant itemData;
            if (m_dataSource) {
                itemData = m_dataSource->value(ob->itemDataSource());
            }
            const int itemHeight = ob->renderSimpleData(nullptr, nullptr, offset, itemData, m_scriptHandler);
            if (itemHeight > intHeight) {
                intHeight = itemHeight;
            }
        }
    }

    return intHeight;
}

qreal KReportPreRendererPrivate::renderSection(const KReportSectionData & sectionData)
{
    qreal sectionHeight = POINT_TO_INCH(sectionData.height()) * KReportPrivate::dpiX();

    int itemHeight = 0;
    //kreportDebug() << "Name: " << sectionData.name() << " Height: " << sectionHeight
    //         << "Objects: " << sectionData.objects().count();
    emit(renderingSection(const_cast<KReportSectionData*>(&sectionData), m_page, QPointF(m_leftMargin, m_yOffset)));

    //Create a pre-rendered section for this section and add it to the document
    OROSection *sec = new OROSection(m_document);
    sec->setHeight(sectionData.height());
    sec->setBackgroundColor(sectionData.backgroundColor());
    sec->setType(sectionData.type());
    m_document->addSection(sec);

    //Render section background
    ORORect* bg = new ORORect();
    bg->setPen(QPen(Qt::NoPen));
    bg->setBrush(sectionData.backgroundColor());
    qreal w = m_page->document()->pageLayout().fullRectPixels(KReportPrivate::dpiX()).width() - m_page->document()->pageLayout().marginsPixels(KReportPrivate::dpiX()).right() - m_leftMargin;

    bg->setRect(QRectF(m_leftMargin, m_yOffset, w, sectionHeight));
    m_page->insertPrimitive(bg, true);

    QList<KReportItemBase*> objects = sectionData.objects();
    foreach(KReportItemBase *ob, objects) {
        QPointF offset(m_leftMargin, m_yOffset);
        QVariant itemData = m_dataSource->value(ob->itemDataSource());

        if (ob->supportsSubQuery()) {
           itemHeight = ob->renderReportData(m_page, sec, offset, m_dataSource, m_scriptHandler);
        } else {
            KReportAsyncItemBase *async_ob = qobject_cast<KReportAsyncItemBase*>(ob);
            if (async_ob){
                //kreportDebug() << "async object";
                asyncManager->addItem(async_ob, m_page, sec, offset, async_ob->realItemData(itemData), m_scriptHandler);
            } else {
                //kreportDebug() << "sync object";
                itemHeight = ob->renderSimpleData(m_page, sec, offset, itemData, m_scriptHandler);
            }
        }

        if (itemHeight > sectionHeight) {
            sectionHeight = itemHeight;
        }
    }
    for (int i = 0; i < m_page->primitiveCount(); ++i) {
        OROPrimitive *prim = m_page->primitive(i);
        if (OROTextBox *text = dynamic_cast<OROTextBox*>(prim)) {
            if (text->requiresPostProcessing()) {
                m_postProcText.append(text);
            }
        }
    }
    m_yOffset += sectionHeight;

    return sectionHeight;
}

#ifdef KREPORT_SCRIPTING
void KReportPreRendererPrivate::initEngine()
{
    delete m_scriptHandler;
    m_scriptHandler = new KReportScriptHandler(m_dataSource, scriptSource, m_reportDocument);

    connect(this, SIGNAL(enteredGroup(QString,QVariant)), m_scriptHandler, SLOT(slotEnteredGroup(QString,QVariant)));

    connect(this, SIGNAL(exitedGroup(QString,QVariant)), m_scriptHandler, SLOT(slotExitedGroup(QString,QVariant)));

    connect(this, SIGNAL(renderingSection(KReportSectionData*,OROPage*,QPointF)), m_scriptHandler, SLOT(slotEnteredSection(KReportSectionData*,OROPage*,QPointF)));
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
    if (!m_dataSource) {
        m_dataSource = m_oneRecord;
    }

    if (!m_valid || !m_reportDocument) {
        return false;
    }

    // Do this check now so we don't have to undo a lot of work later if it fails
    KReportLabelSizeInfo label;
    if (m_reportDocument->pageSize() == QLatin1String("Labels")) {
        label = KReportLabelSizeInfo::find(m_reportDocument->labelType());
        if (label.isNull()) {
            return false;
        }
    }
    //kreportDebug() << "Creating Document";
    m_document = new ORODocument(m_reportDocument->title());

    m_pageCounter  = 0;
    m_yOffset      = 0.0;

    //kreportDebug() << "Calculating Margins";
    if (!label.isNull()) {
        if (m_reportDocument->pageLayout().orientation() == QPageLayout::Portrait) {
            m_topMargin = (label.startY() / 100.0);
            m_bottomMargin = 0;
            m_rightMargin = 0;
            m_leftMargin = (label.startX() / 100.0);
        } else {
            m_topMargin = (label.startX() / 100.0);
            m_bottomMargin = 0;
            m_rightMargin = 0;
            m_leftMargin = (label.startY() / 100.0);
        }
    } else {

        m_topMargin    = m_reportDocument->pageLayout().marginsPoints().top();
        m_bottomMargin = m_reportDocument->pageLayout().marginsPoints().bottom();
        m_rightMargin  = m_reportDocument->pageLayout().marginsPoints().right();
        m_leftMargin   = m_reportDocument->pageLayout().marginsPoints().left();
        //kreportDebug() << "Margins:" << m_topMargin << m_bottomMargin << m_rightMargin << m_leftMargin;
     }

    //kreportDebug() << "Calculating Page Size";
    QPageLayout layout =  m_reportDocument->pageLayout();
    // This should reflect the information of the report page size
    if (m_reportDocument->pageSize() == QLatin1String("Custom")) {
        m_maxWidth = m_reportDocument->pageLayout().fullRectPoints().width();
        m_maxHeight = m_reportDocument->pageLayout().fullRectPoints().height();
    } else {
        if (!label.isNull()) {
            m_maxWidth = label.width();
            m_maxHeight = label.height();
            m_reportDocument->pageLayout().setPageSize(QPageSize(KReportPageSize::pageSize(label.paper())));
        } else {
            // lookup the correct size information for the specified size paper
            QSizeF pageSizePx = m_reportDocument->pageLayout().fullRectPixels(KReportPrivate::dpiX()).size();

            m_maxWidth = pageSizePx.width();
            m_maxHeight = pageSizePx.height();
        }
    }

    if (m_reportDocument->pageLayout().orientation() == QPageLayout::Landscape) {
        qreal tmp = m_maxWidth;
        m_maxWidth = m_maxHeight;
        m_maxHeight = tmp;
    }

    //kreportDebug() << "Page Size:" << m_maxWidth << m_maxHeight;

    m_document->setPageLayout(m_reportDocument->pageLayout());
    m_dataSource->setSorting(m_reportDocument->detail()->sortedFields);
    if (!m_dataSource->open()) {
        return false;
    }

    #ifdef KREPORT_SCRIPTING
    initEngine();
    connect(m_scriptHandler, SIGNAL(groupChanged(QMap<QString, QVariant>)),
            m_preRenderer, SIGNAL(groupChanged(QMap<QString, QVariant>)));

    //Loop through all abjects that have been registered, and register them with the script handler
    if (m_scriptHandler) {
        QMapIterator<QString, QObject*> i(m_scriptObjects);
        while (i.hasNext()) {
            i.next();
            m_scriptHandler->registerScriptObject(i.value(), i.key());
        }
        //execute the script, if it fails, abort and return the empty document
        if (!m_scriptHandler->trigger()) {
            m_scriptHandler->displayErrors();
            return m_document;
        }
    }
    #endif

    createNewPage();
    if (!label.isNull()) {
        // Label Print Run
        // remember the initial margin setting as we will be modifying
        // the value and restoring it as we move around
        qreal margin = m_leftMargin;

        m_yOffset = m_topMargin;

        qreal w = (label.width() / 100.0);
        qreal wg = (label.xGap() / 100.0);
        qreal h = (label.height() / 100.0);
        qreal hg = (label.yGap() / 100.0);
        int numCols = label.columns();
        int numRows = label.rows();
        qreal tmp;

        // flip the value around if we are printing landscape
        if (!(m_reportDocument->pageLayout().orientation() == QPageLayout::Portrait)) {
            w = (label.height() / 100.0);
            wg = (label.yGap() / 100.0);
            h = (label.width() / 100.0);
            hg = (label.xGap() / 100.0);
            numCols = label.rows();
            numRows = label.columns();
        }

        KReportDetailSectionData * detailData = m_reportDocument->detail();
        if (detailData->detailSection) {
            KReportDataSource *mydata = m_dataSource;

            if (mydata && mydata->recordCount() > 0) { /* && !((query = orqThis->getQuery())->eof()))*/
                if (!mydata->moveFirst()) {
                    return false;
                }
                int row = 0;
                int col = 0;
                do {
                    tmp = m_yOffset; // store the value as renderSection changes it
                    renderSection(*(detailData->detailSection));
                    m_yOffset = tmp; // restore the value that renderSection modified

                    col++;
                    m_leftMargin += w + wg;
                    if (col >= numCols) {
                        m_leftMargin = margin; // reset back to original value
                        col = 0;
                        row++;
                        m_yOffset += h + hg;
                        if (row >= numRows) {
                            m_yOffset = m_topMargin;
                            row = 0;
                            createNewPage();
                        }
                    }
                } while (mydata->moveNext());
            }
        }

    } else {
        // Normal Print Run
        if (m_reportDocument->section(KReportSectionData::Type::ReportHeader)) {
            renderSection(*(m_reportDocument->section(KReportSectionData::Type::ReportHeader)));
        }

        if (m_reportDocument->detail()) {
            renderDetailSection(m_reportDocument->detail());
        }

        if (m_reportDocument->section(KReportSectionData::Type::ReportFooter)) {
            if (renderSectionSize(*(m_reportDocument->section(KReportSectionData::Type::ReportFooter))) + finishCurPageSize(true) + m_bottomMargin + m_yOffset >= m_maxHeight) {
                createNewPage();
            }
            renderSection(*(m_reportDocument->section(KReportSectionData::Type::ReportFooter)));
        }
    }
    finishCurPage(true);

    #ifdef KREPORT_SCRIPTING
    // _postProcText contains those text boxes that need to be updated
    // with information that wasn't available at the time it was added to the document
    m_scriptHandler->setPageTotal(m_document->pageCount());

    for (int i = 0; i < m_postProcText.size(); i++) {
        OROTextBox * tb = m_postProcText.at(i);

        m_scriptHandler->setPageNumber(tb->page()->pageNumber() + 1);

        tb->setText(m_scriptHandler->evaluate(tb->text()).toString());
    }
    #endif

    asyncManager->startRendering();

    #ifdef KREPORT_SCRIPTING
    m_scriptHandler->displayErrors();
    #endif

    if (!m_dataSource->close()) {
        return false;
    }
    #ifdef KREPORT_SCRIPTING
    delete m_scriptHandler;
    m_scriptHandler = nullptr;
    #endif

    if (m_dataSource != m_oneRecord) {
        delete m_dataSource;
        m_dataSource = nullptr;
    }
    m_postProcText.clear();

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
    d->m_reportDocument->setName(n);
}

bool KReportPreRenderer::isValid() const
{
    if (d && d->m_valid)
        return true;
    return false;
}

ORODocument* KReportPreRenderer::document()
{
    return d->m_document;
}

bool KReportPreRenderer::generateDocument()
{
//    delete d->m_document;
    if (!d->generateDocument()) {
        delete d->m_document;
        d->m_document = nullptr;
    }
    return d->m_document;
}

void KReportPreRenderer::setDataSource(KReportDataSource *dataSource)
{
    if (d && dataSource != d->m_dataSource) {
        delete d->m_dataSource;
        d->m_dataSource = dataSource;
    }
}

bool KReportPreRenderer::setDocument(const QDomElement &document)
{
    delete d->m_document;
    d->m_valid = false;

    if (document.tagName() != QLatin1String("report:content")) {
        kreportWarning() << "report schema is invalid";
        return false;
    }

    d->m_reportDocument = new KReportDocument(document);
    d->m_valid = d->m_reportDocument->isValid();
    return isValid();
}

#ifdef KREPORT_SCRIPTING
void KReportPreRenderer::setScriptSource(KReportScriptSource *source)
{
    if (d) {
        d->scriptSource = source;
    }
}

void KReportPreRenderer::registerScriptObject(QObject* obj, const QString& name)
{
    //kreportDebug() << name;
    d->m_scriptObjects[name] = obj;
}

KReportScriptHandler *KReportPreRenderer::scriptHandler()
{
    return d->m_scriptHandler;
}
#endif

const KReportDocument* KReportPreRenderer::reportData() const
{
    return d->m_reportDocument;
}
