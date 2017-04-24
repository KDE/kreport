/* This file is part of the KDE project
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

#include "KReportPage.h"

#include "KReportRendererBase.h"
#include "KReportUnit.h"
#include "KReportRenderObjects.h"
#include "KReportUtils_p.h"
#include "kreport_debug.h"

#include <QPainter>
#include <QPixmap>
#include <QPointer>
#include <QTimer>
#include <QWidget>

//! @internal
class Q_DECL_HIDDEN KReportPage::Private
{
public:
    explicit Private(ORODocument *document)
        : reportDocument(document)
    {}

    ~Private()
    {
        delete renderer;
    }

    QPointer<ORODocument> reportDocument;
    int page;
    QPixmap pixmap;
    KReportRendererFactory factory;
    KReportRendererBase *renderer;
    QTimer renderTimer;
};


KReportPage::KReportPage(QWidget *parent, ORODocument *document)
        : QObject(parent), QGraphicsRectItem()
        , d(new Private(document))
{
    Q_ASSERT(document);
    
    int pageWidth;
    int pageHeight;

    QString pageSize = d->reportDocument->pageLayout().pageSize().name();

    pageWidth = d->reportDocument->pageLayout().fullRectPixels(KReportPrivate::dpiX()).width();
    pageHeight = d->reportDocument->pageLayout().fullRectPixels(KReportPrivate::dpiX()).height();

//TODO remove after check    
#if 0    
    if (pageSize == QLatin1String("Custom")) {
        // if this is custom sized sheet of paper we will just use those values

    } else {
        // lookup the correct size information for the specified size paper
        pageWidth = d->reportDocument->pageOptions().pixelSize().width();
        pageHeight = d->reportDocument->pageOptions().pixelSize().height();
    }
#endif

    setRect(0, 0, pageWidth, pageHeight);
    //kreportDebug() << "PAGE IS " << pageWidth << "x" << pageHeight;
    d->pixmap = QPixmap(pageWidth, pageHeight);
    d->renderer = d->factory.createInstance(QLatin1String("screen"));
    connect(d->reportDocument, SIGNAL(updated(int)), this, SLOT(pageUpdated(int)));

    d->renderTimer.setSingleShot(true);
    connect(&d->renderTimer, SIGNAL(timeout()), this, SLOT(renderCurrentPage()));

    renderPage(1);
}

KReportPage::~KReportPage()
{
    delete d;
}

void KReportPage::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawPixmap(QPoint(0, 0), d->pixmap);
}

void KReportPage::renderPage(int page)
{
    d->page = page - 1;
    d->pixmap.fill();
    QPainter qp(&d->pixmap);
    if (d->reportDocument) {
        KReportRendererContext cxt;
        cxt.setPainter(&qp);
        (void)d->renderer->render(cxt, d->reportDocument, d->page);
    }
    update();
}

void KReportPage::pageUpdated(int pageNo)
{
    //kreportDebug() << pageNo << d->page;
    //Refresh this page if it changes
    if (pageNo == d->page) {
        //kreportDebug() << "Current page updated";
        d->renderTimer.start(100);
    }
}

void KReportPage::renderCurrentPage()
{
    renderPage(d->page + 1);
}
