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

class KoReportReportData;
class OROPage;

#ifdef KREPORT_SCRIPTING
#include "krscriptconstants.h"
#include "KoReportData.h"
#include <QtQml/QJSValue>

class KRScriptDebug;
class KRScriptDraw;
class KRSectionData;
class QJSEngine;

namespace Scripting
{
class Report;
class Section;
}
class KREPORT_EXPORT KRScriptHandler : public QObject
{
    Q_OBJECT
public:
    KRScriptHandler(const KoReportData *, KoReportReportData*);
    ~KRScriptHandler();

    QVariant evaluate(const QString&);
    void displayErrors();
    QJSValue registerScriptObject(QObject*, const QString&);
    bool trigger();

public Q_SLOTS:

    void slotEnteredSection(KRSectionData*, OROPage*, QPointF);
    void slotEnteredGroup(const QString&, const QVariant&);
    void slotExitedGroup(const QString&, const QVariant&);
    void setPageNumber(int p) {
        m_constants->setPageNumber(p);
    }
    void setPageTotal(int t) {
        m_constants->setPageTotal(t);
    }
    void newPage();

Q_SIGNALS:
    void groupChanged(const QMap<QString, QVariant> &groupData);

private:
    KRScriptConstants *m_constants;
    KRScriptDebug *m_debug;
    KRScriptDraw *m_draw;

    Scripting::Report *m_report;

    const KoReportData *m_koreportData;

    QString m_source;
    KoReportReportData  *m_reportData;

    QJSEngine* m_engine;
    QJSValue m_scriptValue;

    QMap<QString, QVariant> m_groups;
    QMap<KRSectionData*, Scripting::Section*> m_sectionMap;
//! @todo KEXI3 QString where();
};

#else // !KREPORT_SCRIPTING
#define KRScriptHandler void
#endif

#endif
