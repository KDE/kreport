/* This file is part of the KDE project
   Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)

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

#ifndef KREPORTRENDERERBASE_H
#define KREPORTRENDERERBASE_H

#include <QUrl>

#include "kreport_export.h"

class QPainter;
class QPrinter;
class ORODocument;

/*!
 * @bref Context for executing rendering.
 */
class KREPORT_EXPORT KReportRendererContext
{
    public:
        KReportRendererContext();
        ~KReportRendererContext();

        void setUrl(const QUrl& url);
        void setPainter(QPainter* painter);
        void setPrinter(QPrinter* printer);

        QPrinter *printer();
        QPainter *painter();
        QPrinter *printer() const;
        QPainter *painter() const;

        QUrl url() const;

    private:
        Q_DISABLE_COPY(KReportRendererContext)
        class Private;
        Private * const d;
};

/*!
 * @brief Base class for report renderers.
 *
 * A renderer combines the report design with
 * data to produce a visual output
 */
class KREPORT_EXPORT KReportRendererBase
{
    public:
        KReportRendererBase();

        virtual ~KReportRendererBase();

        //! Render the page of the given document within the given context.
        //! If page == -1, renders the entire document.
        virtual bool render(const KReportRendererContext& context, ORODocument *document, int page = -1) = 0;
};

/*!
 * @brief Factory for creating renderers
 * @todo make it use plugins
 */
class KREPORT_EXPORT KReportRendererFactory
{
    public:
        KReportRendererFactory();
        ~KReportRendererFactory();

        KReportRendererBase* createInstance(const QString& key);

    private:
        Q_DISABLE_COPY(KReportRendererFactory)
        class Private;
        Private * const d;
};

#endif // KREPORTRENDERERBASE_H
