/* This file is part of the KDE project
   Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>

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

#include "window.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

#include <KoReportPluginManager>

Window::Window()
    : QMainWindow()
{
    createMenus();

    m_view = new QGraphicsView(this);
    setCentralWidget(m_view);

    // For now based on KexiReportView
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 1000, 2000);
    m_view->setScene(m_scene);

    if (loadDocument()) {
        m_preRenderer = new KoReportPreRenderer(m_document.documentElement());
        if (!m_preRenderer->isValid()) {
            return;
        }
    }

    KoReportPluginManager* manager = KoReportPluginManager::self();
    // TODO
}

Window::~Window()
{
}

bool Window::loadDocument()
{
    qDebug() << KREPORTEXAMPLE_DATA_DIR;
    QFile file(QLatin1String(KREPORTEXAMPLE_DATA_DIR) + "/report.xml");
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!m_document.setContent(&file,
                               &errorMsg, &errorLine, &errorColumn))
    {
        qWarning() << "Error reading XML from" << file.fileName()
                   << "Message:" << errorMsg
                   << "Line:" << errorLine
                   << "Column:" << errorColumn;
        return false;
    }
    return true;
}

void Window::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcuts(QKeySequence::Quit);
    connect(m_exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    m_fileMenu->addAction(m_exitAction);
}
