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

#include "DesignerWidget.h"
#include "KReportExampleDataSource.h"

#include <KReportDesigner>
#include <KReportDesignerSectionDetail>
#include <KReportDesignerSection>

#include <QAction>
#include <QActionGroup>
#include <QDockWidget>
#include <QDomElement>
#include <QMainWindow>
#include <QScrollArea>
#include <QToolBar>

ReportDesignerWidget::ReportDesignerWidget(QWidget *parent)
    : QScrollArea(parent)
{
    m_reportDesigner = new KReportDesigner(this);
    setWidget(m_reportDesigner);

    connect(m_reportDesigner, SIGNAL(itemInserted(QString)),
            this, SLOT(slotItemInserted(QString)));
    connect(m_reportDesigner, SIGNAL(propertySetChanged()),
            this, SLOT(slotDesignerPropertySetChanged()));
    connect(m_reportDesigner, SIGNAL(dirty()), this, SLOT(designDirty()));

    m_reportDesigner->setDataSource(new KReportExampleDataSource);
}

ReportDesignerWidget::~ReportDesignerWidget()
{
}

QToolBar* ReportDesignerWidget::createMainToolBar(QMainWindow *mainWindow)
{
    Q_ASSERT(mainWindow);
    if (!m_mainToolBar) {
        m_mainToolBar = mainWindow->addToolBar(tr("Main"));
        m_mainToolBar->setObjectName("MainToolBar"); // needed by QMainWindow::saveState()
        QList<QAction*> designerActions = m_reportDesigner->designerActions();
        foreach(QAction* action, designerActions) {
            m_mainToolBar->addAction(action);
        }
    }
    return m_mainToolBar;
}

QToolBar* ReportDesignerWidget::createItemsToolBar(QMainWindow *mainWindow)
{
    if (!m_itemToolBar) {
        m_itemToolBar = new QToolBar(tr("Items"));
        mainWindow->addToolBar(Qt::LeftToolBarArea, m_itemToolBar); // good position for a toolbox
        m_itemToolBar->setObjectName("ItemsToolBar"); // needed by QMainWindow::saveState()
        QActionGroup *group = new QActionGroup(this);
        QList<QAction*> itemActions = KReportDesigner::itemActions(group);
        foreach(QAction* action, itemActions) {
            m_itemToolBar->addAction(action);
        }
        m_reportDesigner->plugItemActions(itemActions);
    }
    return m_itemToolBar;
}

QDockWidget* ReportDesignerWidget::createPropertyEditorDockWidget(QMainWindow *mainWindow,
                                                                  Qt::DockWidgetArea area)
{
    if (!m_propertyDock) {
        m_propertyDock = new QDockWidget(tr("Property Editor"), mainWindow);
        m_propertyDock->setObjectName("PropertyEditorDockWidget"); // needed by QMainWindow::saveState()
        m_propertyEditor = new KPropertyEditorView;
        m_propertyEditor->changeSet(m_reportDesigner->propertySet());
        m_propertyDock->setWidget(m_propertyEditor);
        mainWindow->addDockWidget(area, m_propertyDock);
        m_propertyEditor->resize(200, m_propertyEditor->height());
        slotDesignerPropertySetChanged();
    }
    emit designDirty();
    return m_propertyDock;
}

void ReportDesignerWidget::slotItemInserted(const QString &itemId)
{
    QList<QAction*> itemActions = m_itemToolBar->actions();
    foreach(QAction* action, itemActions) {
        if (action->objectName() == itemId) {
            action->setChecked(false);
        }
    }
}

void ReportDesignerWidget::slotDesignerPropertySetChanged()
{
    if (m_propertyEditor) {
        m_propertyEditor->changeSet(m_reportDesigner->itemPropertySet());
    }
}

void ReportDesignerWidget::designDirty()
{
    emit designChanged(m_reportDesigner->document());
}

QDomElement ReportDesignerWidget::document() const
{
    return m_reportDesigner->document();
}
