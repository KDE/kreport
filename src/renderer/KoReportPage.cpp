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

#include "KoReportPage.h"

#include <KoReportRendererBase.h>

#include "common/KReportPageFormat.h"
#include "common/KReportUnit.h"
#include "common/renderobjects.h"

#include <QWidget>
#include "kreport_debug.h"
#include <QPixmap>
#include <QPainter>
#include <QTimer>

//! @internal
class KoReportPage::Private
{
public:
    explicit Private(ORODocument *document)
        : reportDocument(document)
        , page(0)
    {}

    ~Private()
    {
        delete renderer;
    }

    ORODocument *reportDocument;
    int page;
    QPixmap pixmap;
    KoReportRendererFactory factory;
    KoReportRendererBase *renderer;

    QTimer renderTimer;
};


KoReportPage::KoReportPage(QWidget *parent, ORODocument *document)
        : QObject(parent), QGraphicsRectItem()
        , d(new Private(document))
{
    //! @todo setAttribute(Qt::WA_NoBackground);
    //kreportDebug() << "CREATED PAGE";
    int pageWidth = 0;
    int pageHeight = 0;

    if (d->reportDocument) {
        QString pageSize = d->reportDocument->pageOptions().getPageSize();


        if (pageSize == QLatin1String("Custom")) {
            // if this is custom sized sheet of paper we will just use those values
            pageWidth = (int)(d->reportDocument->pageOptions().getCustomWidth());
            pageHeight = (int)(d->reportDocument->pageOptions().getCustomHeight());
        } else {
            // lookup the correct size information for the specified size paper
            pageWidth = d->reportDocument->pageOptions().widthPx();
            pageHeight = d->reportDocument->pageOptions().heightPx();
        }
    }
    setRect(0,0,pageWidth, pageHeight);
    //kreportDebug() << "PAGE IS " << pageWidth << "x" << pageHeight;
    d->pixmap = QPixmap(pageWidth, pageHeight);
    d->renderer = d->factory.createInstance(QLatin1String("screen"));
    connect(d->reportDocument, SIGNAL(updated(int)), this, SLOT(pageUpdated(int)));

    d->renderTimer.setSingleShot(true);
    connect(&d->renderTimer, SIGNAL(timeout()), this, SLOT(renderCurrentPage()));

    renderPage(1);
}

KoReportPage::~KoReportPage()
{
    delete d;
}

void KoReportPage::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawPixmap(QPoint(0, 0), d->pixmap);
}

void KoReportPage::renderPage(int page)
{
    d->page = page - 1;
    d->pixmap.fill();
    QPainter qp(&d->pixmap);
    if (d->reportDocument) {
        KoReportRendererContext cxt;
        cxt.painter = &qp;
        d->renderer->render(cxt, d->reportDocument, d->page);
    }
    update();
}

void KoReportPage::pageUpdated(int pageNo)
{
    //kreportDebug() << pageNo << d->page;
    //Refresh this page if it changes
    if (pageNo == d->page) {
        //kreportDebug() << "Current page updated";
        d->renderTimer.start(100);
    }
}

void KoReportPage::renderCurrentPage()
{
    renderPage(d->page + 1);
}
