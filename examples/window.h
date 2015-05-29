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

#ifndef KREPORTEXAMPLE_WINDOW_H
#define KREPORTEXAMPLE_WINDOW_H

#include "kreportexampledata.h"

#include <QMainWindow>
#include <QDomDocument>
#include <QAction>

#include <KoReportPreRenderer>
#include <KoReportPage.h>
#include <kreportview.h>

/*! @short KReportExample application's main window */
class Window : public QMainWindow
{
    Q_OBJECT
public:
    Window();
    virtual ~Window();

public Q_SLOTS:
    void showDesign(QDomElement d);

private:
    void createMenus();
    bool loadDocument();

    QMenu *m_fileMenu;
    QAction *m_exitAction;

    KReportView *m_reportView;
    KoReportPreRenderer *m_preRenderer;
    QDomDocument m_document;
    ORODocument *m_oDocument;

    uint m_currentPage;

    KReportExampleData *m_testData;
};

#endif
