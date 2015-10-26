/* This file is part of the KDE project
   Copyright (C) 2015 by Adam Pigg (adam@piggz.co.uk)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KReportView.h"
#include "KReportPage.h"
#include "KReportRenderObjects.h"
#include "KReportPreRenderer.h"
#include "KReportRendererBase.h"
#include "kreport_debug.h"

#include <QLabel>
#include <QBoxLayout>
#include <QScrollArea>
#include <QLayout>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QScrollBar>

//! @internal
class KReportView::Private
{
public:
    explicit Private()
        : reportDocument(0)
        , reportPage(0)
        , currentPage(0)
        , pageCount(0)
    {}

    ~Private()
    {}

    ORODocument *reportDocument;
    QGraphicsView *reportView;
    QGraphicsScene *reportScene;
    KReportPage *reportPage;

    int currentPage;
    int pageCount;

    KReportRendererFactory factory;
};


KReportView::KReportView(QWidget *parent)
        : QWidget(parent), d(new Private())
{
    setObjectName(QLatin1String("KReportView"));

    d->reportView = new QGraphicsView(this);
    // page selector should be always visible:
    d->reportView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QVBoxLayout *l = new QVBoxLayout;
    setLayout(l);

    layout()->addWidget(d->reportView);

    d->reportScene = new QGraphicsScene(this);
    d->reportScene->setSceneRect(0,0,1000,2000);
    d->reportView->setScene(d->reportScene);

    d->reportScene->setBackgroundBrush(palette().brush(QPalette::Dark));
}

KReportView::~KReportView()
{
    kreportDebug();
    delete d;
}

void KReportView::moveToFirstPage()
{
        if (d->currentPage != 1) {
                d->currentPage = 1;
                d->reportPage->renderPage(d->currentPage);
        }
}

void KReportView::moveToLastPage()
{
        if (d->currentPage != d->pageCount) {
                d->currentPage = d->pageCount;
                d->reportPage->renderPage(d->currentPage);
        }
}

void KReportView::moveToNextPage()
{
        if (d->currentPage < d->pageCount) {
                d->currentPage++;
                d->reportPage->renderPage(d->currentPage);
        }
}

void KReportView::moveToPreviousPage()
{
        if (d->currentPage > 1) {
                d->currentPage--;
                d->reportPage->renderPage(d->currentPage);
        }
}

int KReportView::currentPage() const
{
    return d->currentPage;
}

int KReportView::pageCount() const
{
    return d->pageCount;
}

void KReportView::setDocument(ORODocument* doc)
{
    d->reportDocument = doc;

    if (d->reportPage) {
        delete d->reportPage;
    }

    d->pageCount = doc->pages();

    d->reportPage = new KReportPage(this, d->reportDocument);
    d->reportPage->setObjectName(QLatin1String("KReportPage"));

    d->reportScene->setSceneRect(0,0,d->reportPage->rect().width() + 40, d->reportPage->rect().height() + 40);
    d->reportScene->addItem(d->reportPage);
    d->reportPage->setPos(20,20);
    d->reportView->centerOn(0,0);

}

QAbstractScrollArea* KReportView::scrollArea()
{
    return d->reportView;
}

