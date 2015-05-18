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

#include <KoReportDesigner.h>

#include <QLayout>
#include <QActionGroup>
#include <QScrollArea>
#include <QToolBar>
#include <QAction>
#include <QDebug>
#include <QDockWidget>

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
        m_itemToolbar->addAction(action);
    }

    m_reportDesigner->plugItemActions(itemActions);

    connect(m_reportDesigner, SIGNAL(itemInserted(QString)), this, SLOT(slotItemInserted(QString)));

    // Set up the property editor
    m_propertyDock = new QDockWidget(tr("Property Editor"), this);
    m_propertyEditor = new KPropertyEditorView(this);
    m_propertyDock->setWidget(m_propertyEditor);

    addDockWidget(Qt::RightDockWidgetArea, m_propertyDock);
    m_propertyEditor->changeSet(m_reportDesigner->propertySet());

    connect(m_reportDesigner, SIGNAL(propertySetChanged()),
            this, SLOT(slotDesignerPropertySetChanged()));
}

DesignerWindow::~DesignerWindow()
{
}

void DesignerWindow::slotItemInserted(const QString &itemId)
{
    QList<QAction*> itemActions = m_itemToolbar->actions();
    foreach(QAction* action, itemActions) {
        if (action->objectName() == itemId) {
            action->setChecked(false);
        }
    }
}

void DesignerWindow::slotDesignerPropertySetChanged()
{
    m_propertyEditor->changeSet(m_reportDesigner->itemPropertySet());
}
