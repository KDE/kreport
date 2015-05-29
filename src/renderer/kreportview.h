/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KREPORTVIEW_H
#define KREPORTVIEW_H

#include <QDomDocument>
#include <KoReportRendererBase.h>
#include <QGraphicsView>

#include "kreport_export.h"

class KoReportPreRenderer;
class ORODocument;
class KoReportPage;

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
    void setDocument(ORODocument* doc);

private:
    KoReportPreRenderer *m_preRenderer;
    ORODocument *m_reportDocument;
    QGraphicsView *m_reportView;
    QGraphicsScene *m_reportScene;
    KoReportPage *m_reportPage;

    int m_currentPage;
    int m_pageCount;

    KoReportRendererFactory m_factory;

};

#endif
