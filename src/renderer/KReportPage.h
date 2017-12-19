/* This file is part of the KDE project
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

#ifndef KREPORTPAGE_H
#define KREPORTPAGE_H

#include "kreport_export.h"

#include <QGraphicsRectItem>
#include <QObject>

class ORODocument;

/*!
 * @brief Provides a widget that renders a specific page of
 * an ORODocument
 * The widget is sized to the document size in pixels
 */
class KREPORT_EXPORT KReportPage : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    KReportPage(QWidget *parent, ORODocument *document);

    ~KReportPage() override;

    //! Renders page @a page (counted from 1).
    void renderPage(int page);

public Q_SLOTS:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    void pageUpdated(int pageNo);

private Q_SLOTS:
    void renderCurrentPage();

private:
    Q_DISABLE_COPY(KReportPage)
    class Private;
    Private * const d;
};

#endif
