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

#ifndef KREPORTVIEW_H
#define KREPORTVIEW_H

#include <QWidget>

#include "kreport_export.h"

class ORODocument;
class QAbstractScrollArea;

/**
 * @brief Provides a simple widget for viewing a rendered report on screen
 *
 */
class KREPORT_EXPORT KReportView : public QWidget
{
    Q_OBJECT
public:
    explicit KReportView(QWidget *parent);

    ~KReportView();

    void moveToFirstPage();
    void moveToLastPage();
    void moveToNextPage();
    void moveToPreviousPage();
    int currentPage() const;
    int pageCount() const;
    void refreshCurrentPage();
    void setDocument(ORODocument* doc);

    QAbstractScrollArea* scrollArea();

private:
    Q_DISABLE_COPY(KReportView)
    class Private;
    Private * const d;
};

#endif
