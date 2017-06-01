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

#ifndef KRSCRIPTHANDLER_H
#define KRSCRIPTHANDLER_H

#include "kreport_export.h"
#include "config-kreport.h"

#ifdef KREPORT_SCRIPTING
#include "KReportScriptConstants.h"
#include "KReportDataSource.h"
#include <QtQml/QJSValue>

class KReportScriptDebug;
class KReportScriptDraw;
class KReportSectionData;
class QJSEngine;
class KReportDocument;
class OROPage;
class KReportScriptSource;

namespace Scripting
{
class Report;
class Section;
}

class KREPORT_EXPORT KReportScriptHandler : public QObject
{
    Q_OBJECT
public:
    KReportScriptHandler(const KReportDataSource *reportDataSource, KReportScriptSource *scriptSource, KReportDocument* reportDocument);
    ~KReportScriptHandler() override;

    QVariant evaluate(const QString&);
    void displayErrors();
    QJSValue registerScriptObject(QObject*, const QString&);
    bool trigger();

public Q_SLOTS:

    void slotEnteredSection(KReportSectionData*, OROPage*, QPointF);
    void slotEnteredGroup(const QString&, const QVariant&);
    void slotExitedGroup(const QString&, const QVariant&);
    void setPageNumber(int p);
    void setPageTotal(int t);
    void newPage();

Q_SIGNALS:
    void groupChanged(const QMap<QString, QVariant> &groupData);

private:
    //! @todo KEXI3 QString where();
    Q_DISABLE_COPY(KReportScriptHandler)
    class Private;
    Private * const d;
};

#else // !KREPORT_SCRIPTING
#define KReportScriptHandler void
#endif

#endif
