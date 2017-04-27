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

#ifndef KREPORTDESIGNERWIDGET_H
#define KREPORTDESIGNERWIDGET_H

#include <QDockWidget>
#include <QPointer>
#include <QScrollArea>
#include <QToolBar>

#include <KPropertyEditorView>
#include <KPropertySet>

class KReportDesigner;
class QDomElement;
class QMainWindow;
class QScrollArea;

//! KReportExample designer widget
class ReportDesignerWidget : public QScrollArea
{
    Q_OBJECT

public:
    ReportDesignerWidget(QWidget *parent = nullptr);
    ~ReportDesignerWidget() override;

    //! Creates main toolbar for main window @a mainWindow
    //! @a mainWindow is required. To be called once.
    QToolBar* createMainToolBar(QMainWindow *mainWindow);

    //! Creates items toolbar for main window @a mainWindow
    //! @a mainWindow is required. To be called once.
    QToolBar* createItemsToolBar(QMainWindow *mainWindow);

    //! Creates a property editor dock widget for the main window @a mainWindow.
    //! The widget is added to the area @a area is it is not Qt::NoDockWidgetArea.
    //! @a mainWindow is required. To be called once.
    QDockWidget* createPropertyEditorDockWidget(QMainWindow *mainWindow,
                                                Qt::DockWidgetArea area = Qt::NoDockWidgetArea);

    //! @return current document
    QDomElement document() const;

Q_SIGNALS:
    void designChanged(const QDomElement&);

private Q_SLOTS:
    void slotItemInserted(const QString &itemId);
    void slotDesignerPropertySetChanged();
    void designDirty();

private:
    KReportDesigner *m_reportDesigner;

    QPointer<QToolBar> m_mainToolBar;
    QPointer<QToolBar> m_itemToolBar;

    QPointer<QDockWidget> m_propertyDock;
    QPointer<KPropertyEditorView> m_propertyEditor;
};

#endif // KREPORTDESIGNERWIDGET_H
