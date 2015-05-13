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

#ifndef DESIGNERWINDOW_H
#define DESIGNERWINDOW_H

#include <qt5/QtWidgets/QMainWindow>

class QScrollArea;
class KoReportDesigner;

#include <kproperty/KPropertySet.h>

class DesignerWindow : public QMainWindow
{
    Q_OBJECT

public:
    DesignerWindow();
    ~DesignerWindow();

private:
    QScrollArea * m_scrollArea;
    KoReportDesigner *m_reportDesigner;
    KPropertySet *m_propertySet;

    QToolBar *m_mainToolbar;
    QToolBar *m_itemToolbar;
};

#endif // DESIGNERWINDOW_H