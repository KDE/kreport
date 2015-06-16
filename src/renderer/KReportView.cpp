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
#include <KoReportPage.h>

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

#include <renderobjects.h>
#include <KoReportPreRenderer.h>

KReportView::KReportView(QWidget *parent)
        : QWidget(parent), m_reportDocument(0), m_reportPage(0), m_currentPage(0), m_pageCount(0)
{
    setObjectName(QLatin1String("KReportView"));

    m_reportView = new QGraphicsView(this);
    // page selector should be always visible:
    m_reportView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QVBoxLayout *l = new QVBoxLayout;
    setLayout(l);

    layout()->addWidget(m_reportView);

    m_reportScene = new QGraphicsScene(this);
    m_reportScene->setSceneRect(0,0,1000,2000);
    m_reportView->setScene(m_reportScene);

    m_reportScene->setBackgroundBrush(palette().brush(QPalette::Dark));
}

KReportView::~KReportView()
{
    qDebug();
}

void KReportView::moveToFirstPage()
{
        if (m_currentPage != 1) {
                m_currentPage = 1;
                m_reportPage->renderPage(m_currentPage);
        }
}

void KReportView::moveToLastPage()
{
        if (m_currentPage != m_pageCount) {
                m_currentPage = m_pageCount;
                m_reportPage->renderPage(m_currentPage);
        }
}

void KReportView::moveToNextPage()
{
        if (m_currentPage < m_pageCount) {
                m_currentPage++;
                m_reportPage->renderPage(m_currentPage);
        }
}

void KReportView::moveToPreviousPage()
{
        if (m_currentPage > 1) {
                m_currentPage--;
                m_reportPage->renderPage(m_currentPage);
        }
}

int KReportView::currentPage() const
{
    return m_currentPage;
}

int KReportView::pageCount() const
{
    return m_pageCount;
}

void KReportView::setDocument(ORODocument* doc)
{
    m_reportDocument = doc;

    if (m_reportPage) {
        delete m_reportPage;
    }

    m_reportPage = new KoReportPage(this, m_reportDocument);
    m_reportPage->setObjectName(QLatin1String("KReportPage"));

    m_reportScene->setSceneRect(0,0,m_reportPage->rect().width() + 40, m_reportPage->rect().height() + 40);
    m_reportScene->addItem(m_reportPage);
    m_reportPage->setPos(20,20);
    m_reportView->centerOn(0,0);

}
