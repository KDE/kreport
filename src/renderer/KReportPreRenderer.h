/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
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

#ifndef KREPORTPRERENDERER_H
#define KREPORTPRERENDERER_H

#include "config-kreport.h"
#include "kreport_export.h"

#include <QObject>

#ifdef KREPORT_SCRIPTING
class KReportScriptHandler;
class KReportScriptSource;
#else
#define KReportScriptHandler void
#endif
class KReportPreRendererPrivate;
class ORODocument;
class KReportDataSource;
class KReportDocument;
class QDomElement;

/*!
 * @brief Takes a report definition and prerenders the result to
 * an ORODocument that can be used to pass to any number of renderers.
 */
class KREPORT_EXPORT KReportPreRenderer : public QObject
{
    Q_OBJECT
public:
    explicit KReportPreRenderer(const QDomElement& document);

    ~KReportPreRenderer() override;

    //! Sets data source to @a data, takes ownership
    void setDataSource(KReportDataSource* dataSource);

#ifdef KREPORT_SCRIPTING
    //!Sets the script source to @a source, does NOT take ownership as it may be an application window
    void setScriptSource(KReportScriptSource* source);

    KReportScriptHandler *scriptHandler();
    void registerScriptObject(QObject *obj, const QString &name);
#endif

    bool generateDocument();

    ORODocument *document();

    /**
    @brief Set the name of the report so that it can be used internally by the script engine
    */
    void setName(const QString &);

    bool isValid() const;

    const KReportDocument *reportData() const;

Q_SIGNALS:
    void groupChanged(const QMap<QString, QVariant> &groupData);
    void finishedAllASyncItems();

private:
    bool setDocument(const QDomElement &document);

    Q_DISABLE_COPY(KReportPreRenderer)
    KReportPreRendererPrivate *const d;
};

#endif // KREPORTPRERENDERER_H
