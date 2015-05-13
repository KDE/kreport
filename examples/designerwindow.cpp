/* This file is part of the KDE project
   Copyright (C) 2015 by Adam Pigg <adam@piggz.co.uk>

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

#include "designerwindow.h"
#include <QScrollArea>
#include <KoReportDesigner.h>
#include <QLayout>
#include <qactiongroup.h>
#include <QToolBar>
#include <QDebug>

DesignerWindow::DesignerWindow()
{
    m_scrollArea = new QScrollArea(this);
    setCentralWidget(m_scrollArea);

    m_reportDesigner = new KoReportDesigner(this);
    m_scrollArea->setWidget(m_reportDesigner);

    m_mainToolbar = addToolBar(tr("main"));
    m_itemToolbar = addToolBar(tr("items"));

    QActionGroup *group = new QActionGroup(this);
    QList<QAction*> itemActions = KoReportDesigner::itemActions(group);
    foreach(QAction* action, itemActions) {
        connect(action, SIGNAL(triggered(bool)), this, SLOT(slotToolboxActionTriggered(bool)));
        m_itemToolbar->addAction(action);
    }

    m_reportDesigner->plugItemActions(itemActions);
}

DesignerWindow::~DesignerWindow()
{
}
