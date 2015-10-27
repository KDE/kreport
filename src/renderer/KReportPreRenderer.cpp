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
#include "KReportOneRecordData_p.h"

#include "KReportRenderObjects.h"
#include "KReportData.h"
#include "KReportItemBase.h"
#include "KReportDocument.h"
#include "KReportDetailSectionData.h"
#include "KReportLabelSizeInfo.h"
#include "KReportPageFormat.h"

#ifdef KREPORT_SCRIPTING
#include "scripting/KReportScriptHandler.h"
#include "scripting/KReportGroupTracker.h"
#endif

#include <QDomElement>
#include <QScreen>
#include <QApplication>
#include "kreport_debug.h"

KReportPreRendererPrivate::KReportPreRendererPrivate()
{
    m_valid = false;
    m_document = 0;
    m_reportDocument = 0;
    m_page = 0;
    m_yOffset = 0.0;
    m_topMargin = m_bottomMargin = 0.0;
    m_leftMargin = m_rightMargin = 0.0;
    m_pageCounter = 0;
    m_maxHeight = m_maxWidth = 0.0;
    m_oneRecord = new KReportPrivate::OneRecordData();
    m_kodata = m_oneRecord;
#ifdef KREPORT_SCRIPTING
    m_scriptHandler = 0;
#endif
    asyncManager = new KReportPrivate::AsyncItemManager(this);

    connect(asyncManager, SIGNAL(finished()), this, SLOT(asyncItemsFinished()));
}

KReportPreRendererPrivate::~KReportPreRendererPrivate()
{
    delete m_reportDocument;
    delete m_oneRecord;

    m_postProcText.clear();
}

void KReportPreRendererPrivate::createNewPage()
{
    //kreportDebug();
    if (m_pageCounter > 0)
        finishCurPage();

    m_pageCounter++;

#ifdef KREPORT_SCRIPTING
    //Update the page count script value
    m_scriptHandler->setPageNumber(m_pageCounter);
    m_scriptHandler->newPage();
#endif

    m_page = new OROPage(0);
    m_document->addPage(m_page);

    //! @todo calculate past page
    bool lastPage = false;

    m_yOffset = m_topMargin;

    if (m_pageCounter == 1 && m_reportDocument->m_pageHeaderFirst)
        renderSection(*(m_reportDocument->m_pageHeaderFirst));
    else if (lastPage == true && m_reportDocument->m_pageHeaderLast)
        renderSection(*(m_reportDocument->m_pageHeaderLast));
    else if ((m_pageCounter % 2) == 1 && m_reportDocument->m_pageHeaderOdd)
        renderSection(*(m_reportDocument->m_pageHeaderOdd));
    else if ((m_pageCounter % 2) == 0 && m_reportDocument->m_pageHeaderAny)
        renderSection(*(m_reportDocument->m_pageHeaderAny));
    else if (m_reportDocument->m_pageHeaderAny)
        renderSection(*(m_reportDocument->m_pageHeaderAny));
}

qreal KReportPreRendererPrivate::finishCurPageSize(bool lastPage)
{
    qreal retval = 0.0;

    if (lastPage && m_reportDocument->m_pageFooterLast)
        retval = renderSectionSize(* (m_reportDocument->m_pageFooterLast));
    else if (m_pageCounter == 1 && m_reportDocument->m_pageFooterFirst)
        retval = renderSectionSize(* (m_reportDocument->m_pageFooterFirst));
    else if ((m_pageCounter % 2) == 1 && m_reportDocument->m_pageFooterOdd)
        retval = renderSectionSize(* (m_reportDocument->m_pageFooterOdd));
    else if ((m_pageCounter % 2) == 0 && m_reportDocument->m_pageFooterEven)
        retval = renderSectionSize(* (m_reportDocument->m_pageFooterEven));
    else if (m_reportDocument->m_pageFooterAny)
        retval = renderSectionSize(* (m_reportDocument->m_pageFooterAny));

    //kreportDebug() << retval;
    return retval;
}

qreal KReportPreRendererPrivate::finishCurPage(bool lastPage)
{

    qreal offset = m_maxHeight - m_bottomMargin;
    qreal retval = 0.0;
    //kreportDebug() << offset;

    if (lastPage && m_reportDocument->m_pageFooterLast) {
        //kreportDebug() << "Last Footer";
        m_yOffset = offset - renderSectionSize(* (m_reportDocument->m_pageFooterLast));
        retval = renderSection(* (m_reportDocument->m_pageFooterLast));
    } else if (m_pageCounter == 1 && m_reportDocument->m_pageFooterFirst) {
        //kreportDebug() << "First Footer";
        m_yOffset = offset - renderSectionSize(* (m_reportDocument->m_pageFooterFirst));
        retval = renderSection(* (m_reportDocument->m_pageFooterFirst));
    } else if ((m_pageCounter % 2) == 1 && m_reportDocument->m_pageFooterOdd) {
        //kreportDebug() << "Odd Footer";
        m_yOffset = offset - renderSectionSize(* (m_reportDocument->m_pageFooterOdd));
        retval = renderSection(* (m_reportDocument->m_pageFooterOdd));
    } else if ((m_pageCounter % 2) == 0 && m_reportDocument->m_pageFooterEven) {
        //kreportDebug() << "Even Footer";
        m_yOffset = offset - renderSectionSize(* (m_reportDocument->m_pageFooterEven));
        retval = renderSection(* (m_reportDocument->m_pageFooterEven));
    } else if (m_reportDocument->m_pageFooterAny) {
        //kreportDebug() << "Any Footer";
        m_yOffset = offset - renderSectionSize(* (m_reportDocument->m_pageFooterAny));
        retval = renderSection(* (m_reportDocument->m_pageFooterAny));
    }

    return retval;
}

void KReportPreRendererPrivate::renderDetailSection(KReportDetailSectionData *detailData)
{
    if (detailData->m_detailSection) {
        if (m_kodata/* && !curs->eof()*/) {
            QStringList keys;
            QStringList keyValues;
            QList<int> shownGroups;
            KReportDetailGroupSectionData * grp = 0;

            bool status = m_kodata->moveFirst();
            m_recordCount = m_kodata->recordCount();

            //kreportDebug() << "Record Count:" << m_recordCount;

            for (int i = 0; i < (int) detailData->m_groupList.count(); ++i) {
                grp = detailData->m_groupList[i];
                //If the group has a header or footer, then emit a change of group value
                if(grp->m_groupFooter || grp->m_groupHeader) {
                    // we get here only if group is *shown*
                    shownGroups << i;
                    keys.append(grp->m_column);
                    if (!keys.last().isEmpty())
                        keyValues.append(m_kodata->value(m_kodata->fieldNumber(keys.last())).toString());
                    else
                        keyValues.append(QString());

                    //Tell interested parties we're about to render a header
                    emit(enteredGroup(keys.last(), keyValues.last()));
                }
                if (grp->m_groupHeader)
                    renderSection(*(grp->m_groupHeader));
            }

            while (status) {
                long l = m_kodata->at();
                //kreportDebug() << "At:" << l << "Y:" << m_yOffset << "Max Height:" << m_maxHeight;
                if (renderSectionSize(*(detailData->m_detailSection)) + finishCurPageSize((l + 1 == m_recordCount)) + m_bottomMargin + m_yOffset >= m_maxHeight) {
                    //kreportDebug() << "Next section is too big for this page";
                    if (l > 0) {
                        m_kodata->movePrevious();
                        createNewPage();
                        m_kodata->moveNext();
                    }
                }

                renderSection(*(detailData->m_detailSection));
                if (m_kodata)
                    status = m_kodata->moveNext();

                if (status == true && keys.count() > 0) {
                    // check to see where it is we need to start
                    int pos = -1; // if it's still -1 by the time we are done then no keyValues changed
                    for (int i = 0; i < keys.count(); ++i) {
                        if (keyValues[i] != m_kodata->value(m_kodata->fieldNumber(keys[i])).toString()) {
                            pos = i;
                            break;
                        }
                    }
                    // don't bother if nothing has changed
                    if (pos != -1) {
                        // roll back the query and go ahead if all is good
                        status = m_kodata->movePrevious();
                        if (status == true) {
                            // print the footers as needed
                            // any changes made in this for loop need to be duplicated
                            // below where the footers are finished.
                            bool do_break = false;
                            for (int i = shownGroups.count() - 1; i >= 0; i--) {
                                if (do_break)
                                    createNewPage();
                                do_break = false;
                                grp = detailData->m_groupList[shownGroups.at(i)];

                                if (grp->m_groupFooter) {
                                    if (renderSectionSize(*(grp->m_groupFooter)) + finishCurPageSize() + m_bottomMargin + m_yOffset >= m_maxHeight)
                                        createNewPage();
                                    renderSection(*(grp->m_groupFooter));
                                }

                                if (KReportDetailGroupSectionData::BreakAfterGroupFooter == grp->m_pagebreak)
                                    do_break = true;
                            }
                            // step ahead to where we should be and print the needed headers
                            // if all is good
                            status = m_kodata->moveNext();
                            if (do_break)
                                createNewPage();
                            if (status == true) {
                                for (int i = 0; i < shownGroups.count(); ++i) {
                                    grp = detailData->m_groupList[shownGroups.at(i)];

                                    if (grp->m_groupHeader) {
                                        if (renderSectionSize(*(grp->m_groupHeader)) + finishCurPageSize() + m_bottomMargin + m_yOffset >= m_maxHeight) {
                                            m_kodata->movePrevious();
                                            createNewPage();
                                            m_kodata->moveNext();
                                        }

                                        if (!keys[i].isEmpty())
                                            keyValues[i] = m_kodata->value(m_kodata->fieldNumber(keys[i])).toString();

                                        //Tell interested parties thak key values changed
                                        renderSection(*(grp->m_groupHeader));
                                    }


                                }
                            }
                        }
                    }
                }
            }

            if (keys.size() > 0 && m_kodata->movePrevious()) {
                // finish footers
                // duplicated changes from above here
                for (int i = shownGroups.count() - 1; i >= 0; i--) {
                    grp = detailData->m_groupList[shownGroups.at(i)];

                    if (grp->m_groupFooter) {
                        if (renderSectionSize(*(grp->m_groupFooter)) + finishCurPageSize() + m_bottomMargin + m_yOffset >= m_maxHeight)
                            createNewPage();
                        renderSection(*(grp->m_groupFooter));
                        emit(exitedGroup(keys[i], keyValues[i]));
                    }
                }
            }
        }
        if (KReportDetailSectionData::BreakAtEnd == detailData->m_pageBreak)
            createNewPage();
    }
}

qreal KReportPreRendererPrivate::renderSectionSize(const KReportSectionData & sectionData)
{
    QScreen *srn = QApplication::screens().at(0);
    qreal intHeight = POINT_TO_INCH(sectionData.height()) * srn->logicalDotsPerInchX();

    int itemHeight = 0;

    if (sectionData.objects().count() == 0)
        return intHeight;

    QList<KReportItemBase*> objects = sectionData.objects();
    foreach(KReportItemBase *ob, objects) {
        QPointF offset(m_leftMargin, m_yOffset);
        QVariant itemData = m_kodata->value(ob->itemDataSource());

        //ASync objects cannot alter the section height
        KReportAsyncItemBase *async_ob = qobject_cast<KReportAsyncItemBase*>(ob);

        if (!async_ob) {
            itemHeight = ob->renderSimpleData(0, 0, offset, itemData, m_scriptHandler);

            if (itemHeight > intHeight) {
                intHeight = itemHeight;
            }
        }
    }

    return intHeight;
}

qreal KReportPreRendererPrivate::renderSection(const KReportSectionData & sectionData)
{
    QScreen *srn = QApplication::screens().at(0);
    qreal sectionHeight = POINT_TO_INCH(sectionData.height()) * srn->logicalDotsPerInchX();

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
    qreal w = m_page->document()->pageOptions().widthPx() - m_page->document()->pageOptions().getMarginRight() - m_leftMargin;

    bg->setRect(QRectF(m_leftMargin, m_yOffset, w, sectionHeight));
    m_page->addPrimitive(bg, true);

    QList<KReportItemBase*> objects = sectionData.objects();
    foreach(KReportItemBase *ob, objects) {
        QPointF offset(m_leftMargin, m_yOffset);
        QVariant itemData = m_kodata->value(ob->itemDataSource());

        if (ob->supportsSubQuery()) {
           itemHeight = ob->renderReportData(m_page, sec, offset, m_kodata, m_scriptHandler);
        } else {
            KReportAsyncItemBase *async_ob = qobject_cast<KReportAsyncItemBase*>(ob);
            if (async_ob){
                //kreportDebug() << "async object";
                asyncManager->addItem(async_ob, m_page, sec, offset, itemData, m_scriptHandler);
            } else {
                //kreportDebug() << "sync object";
                itemHeight = ob->renderSimpleData(m_page, sec, offset, itemData, m_scriptHandler);
            }
        }

        if (itemHeight > sectionHeight) {
            sectionHeight = itemHeight;
        }
    }
    for (int i = 0; i < m_page->primitives(); ++i) {
        OROPrimitive *prim = m_page->primitive(i);
        if (prim->type() == OROTextBox::TextBox) {
            OROTextBox *text = static_cast<OROTextBox*>(prim);
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
    m_scriptHandler = new KReportScriptHandler(m_kodata, m_reportDocument);

    connect(this, SIGNAL(enteredGroup(QString,QVariant)), m_scriptHandler, SLOT(slotEnteredGroup(QString,QVariant)));

    connect(this, SIGNAL(exitedGroup(QString,QVariant)), m_scriptHandler, SLOT(slotExitedGroup(QString,QVariant)));

    connect(this, SIGNAL(renderingSection(KReportSectionData*,OROPage*,QPointF)), m_scriptHandler, SLOT(slotEnteredSection(KReportSectionData*,OROPage*,QPointF)));
}
#endif

void KReportPreRendererPrivate::asyncItemsFinished()
{
    //kreportDebug() << "Finished rendering async items";
    delete asyncManager;
}


//===========================KReportPreRenderer===============================

KReportPreRenderer::KReportPreRenderer(const QDomElement & document)
    : d(new KReportPreRendererPrivate())
{
    setDocument(document);
}

KReportPreRenderer::~KReportPreRenderer()
{
    delete d;
}

void KReportPreRenderer::setName(const QString &n)
{
    d->m_reportDocument->setName(n);
}

ORODocument* KReportPreRenderer::generate()
{
    QScreen *srn = QApplication::screens().at(0);
    int dpiX = srn->logicalDotsPerInchX();
    int dpiY = srn->logicalDotsPerInchY();

    if (d == 0 || !d->m_valid || d->m_reportDocument == 0 || d->m_kodata == 0)
        return 0;

    // Do this check now so we don't have to undo a lot of work later if it fails
    KReportLabelSizeInfo label;
    if (d->m_reportDocument->page.getPageSize() == QLatin1String("Labels")) {
        label = KReportLabelSizeInfo::find(d->m_reportDocument->page.getLabelType());
        if (label.isNull())
            return 0;
    }

    //kreportDebug() << "Creating Document";
    d->m_document = new ORODocument(d->m_reportDocument->m_title);

    d->m_pageCounter  = 0;
    d->m_yOffset      = 0.0;

    //kreportDebug() << "Calculating Margins";
    if (!label.isNull()) {
        if (d->m_reportDocument ->page.isPortrait()) {
            d->m_topMargin = (label.startY() / 100.0);
            d->m_bottomMargin = 0;
            d->m_rightMargin = 0;
            d->m_leftMargin = (label.startX() / 100.0);
        } else {
            d->m_topMargin = (label.startX() / 100.0);
            d->m_bottomMargin = 0;
            d->m_rightMargin = 0;
            d->m_leftMargin = (label.startY() / 100.0);
        }
    } else {
        d->m_topMargin    = d->m_reportDocument->page.getMarginTop();
        d->m_bottomMargin = d->m_reportDocument->page.getMarginBottom();
        d->m_rightMargin  = d->m_reportDocument->page.getMarginRight();
        d->m_leftMargin   = d->m_reportDocument->page.getMarginLeft();
        //kreportDebug() << "Margins:" << d->m_topMargin << d->m_bottomMargin << d->m_rightMargin << d->m_leftMargin;
    }

    //kreportDebug() << "Calculating Page Size";
    KReportPageOptions rpo(d->m_reportDocument->page);
    // This should reflect the information of the report page size
    if (d->m_reportDocument->page.getPageSize() == QLatin1String("Custom")) {
        d->m_maxWidth = d->m_reportDocument->page.getCustomWidth();
        d->m_maxHeight = d->m_reportDocument->page.getCustomHeight();
    } else {
        if (!label.isNull()) {
            d->m_maxWidth = label.width();
            d->m_maxHeight = label.height();
            rpo.setPageSize(label.paper());
        } else {
            // lookup the correct size information for the specified size paper
            d->m_maxWidth = KReportPageFormat::width(KReportPageFormat::formatFromString(d->m_reportDocument->page.getPageSize()), KReportPageFormat::Portrait);
            d->m_maxHeight = KReportPageFormat::height(KReportPageFormat::formatFromString(d->m_reportDocument->page.getPageSize()), KReportPageFormat::Portrait);

            KReportUnit pageUnit(KReportUnit::Millimeter);
            d->m_maxWidth = KReportUnit::toInch(pageUnit.fromUserValue(d->m_maxWidth)) * dpiX;
            d->m_maxHeight = KReportUnit::toInch(pageUnit.fromUserValue(d->m_maxHeight)) * dpiY;
        }
    }

    if (!d->m_reportDocument->page.isPortrait()) {
        qreal tmp = d->m_maxWidth;
        d->m_maxWidth = d->m_maxHeight;
        d->m_maxHeight = tmp;
    }

    //kreportDebug() << "Page Size:" << d->m_maxWidth << d->m_maxHeight;

    d->m_document->setPageOptions(rpo);
    d->m_kodata->setSorting(d->m_reportDocument->m_detailSection->m_sortedFields);
    d->m_kodata->open();

#ifdef KREPORT_SCRIPTING
    d->initEngine();
    connect(d->m_scriptHandler, SIGNAL(groupChanged(QMap<QString, QVariant>)),
            this, SIGNAL(groupChanged(QMap<QString, QVariant>)));

    //Loop through all abjects that have been registered, and register them with the script handler
    if (d->m_scriptHandler) {
        QMapIterator<QString, QObject*> i(d->m_scriptObjects);
        while (i.hasNext()) {
            i.next();
            d->m_scriptHandler->registerScriptObject(i.value(), i.key());
        }
        //execute the script, if it fails, abort and return the empty document
        if (!d->m_scriptHandler->trigger()) {
            d->m_scriptHandler->displayErrors();
            return d->m_document;
        }
    }
#endif

    d->createNewPage();
    if (!label.isNull()) {
// Label Print Run
        // remember the initial margin setting as we will be modifying
        // the value and restoring it as we move around
        qreal margin = d->m_leftMargin;

        d->m_yOffset = d->m_topMargin;

        qreal w = (label.width() / 100.0);
        qreal wg = (label.xGap() / 100.0);
        qreal h = (label.height() / 100.0);
        qreal hg = (label.yGap() / 100.0);
        int numCols = label.columns();
        int numRows = label.rows();
        qreal tmp;

        // flip the value around if we are printing landscape
        if (!d->m_reportDocument->page.isPortrait()) {
            w = (label.height() / 100.0);
            wg = (label.yGap() / 100.0);
            h = (label.width() / 100.0);
            hg = (label.xGap() / 100.0);
            numCols = label.rows();
            numRows = label.columns();
        }

        KReportDetailSectionData * detailData = d->m_reportDocument->m_detailSection;
        if (detailData->m_detailSection) {
            KReportData *mydata = d->m_kodata;

            if (mydata && mydata->recordCount() > 0) { /* && !((query = orqThis->getQuery())->eof()))*/
                mydata->moveFirst();
                int row = 0;
                int col = 0;
                do {
                    tmp = d->m_yOffset; // store the value as renderSection changes it
                    d->renderSection(*(detailData->m_detailSection));
                    d->m_yOffset = tmp; // restore the value that renderSection modified

                    col++;
                    d->m_leftMargin += w + wg;
                    if (col >= numCols) {
                        d->m_leftMargin = margin; // reset back to original value
                        col = 0;
                        row++;
                        d->m_yOffset += h + hg;
                        if (row >= numRows) {
                            d->m_yOffset = d->m_topMargin;
                            row = 0;
                            d->createNewPage();
                        }
                    }
                } while (mydata->moveNext());
            }
        }

    } else {
// Normal Print Run
        if (d->m_reportDocument->m_reportHeader) {
            d->renderSection(*(d->m_reportDocument->m_reportHeader));
        }

        if (d->m_reportDocument->m_detailSection) {
            d->renderDetailSection(d->m_reportDocument->m_detailSection);
        }

        if (d->m_reportDocument->m_reportFooter) {
            if (d->renderSectionSize(*(d->m_reportDocument->m_reportFooter)) + d->finishCurPageSize(true) + d->m_bottomMargin + d->m_yOffset >= d->m_maxHeight) {
                d->createNewPage();
            }
            d->renderSection(*(d->m_reportDocument->m_reportFooter));
        }
    }
    d->finishCurPage(true);

#ifdef KREPORT_SCRIPTING
    // _postProcText contains those text boxes that need to be updated
    // with information that wasn't available at the time it was added to the document
    d->m_scriptHandler->setPageTotal(d->m_document->pages());

    for (int i = 0; i < d->m_postProcText.size(); i++) {
        OROTextBox * tb = d->m_postProcText.at(i);

        d->m_scriptHandler->setPageNumber(tb->page()->page() + 1);

        tb->setText(d->m_scriptHandler->evaluate(tb->text()).toString());
    }
#endif

    d->asyncManager->startRendering();

#ifdef KREPORT_SCRIPTING
    d->m_scriptHandler->displayErrors();
#endif

    d->m_kodata->close();
#ifdef KREPORT_SCRIPTING
    delete d->m_scriptHandler;
    d->m_scriptHandler = 0;
#endif
    //! @todo check ownership delete d->m_kodata;
    d->m_postProcText.clear();

    return d->m_document;
}

void KReportPreRenderer::setSourceData(KReportData *data)
{
    if (data) {
        d->m_kodata = data;
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

bool KReportPreRenderer::isValid() const
{
    if (d && d->m_valid)
        return true;
    return false;
}

#ifdef KREPORT_SCRIPTING
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
