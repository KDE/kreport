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

#ifndef __KOREPORTPRERENDERER_P_H__
#define __KOREPORTPRERENDERER_P_H__

#include "config-kreport.h"

#include "renderobjects.h"
#include "KoReportASyncItemManager.h"

#include <QMap>

/*! This class is the private class that houses all the internal
  variables so we can provide a cleaner interface to the user
  without presenting to them things that they don't need to see
  and may change over time. */
class KoReportPreRendererPrivate : public QObject
{
    Q_OBJECT
public:
    KoReportPreRendererPrivate();
    virtual ~KoReportPreRendererPrivate();

    bool m_valid;

    ORODocument* m_document;
    OROPage*     m_page;
    KoReportReportData* m_reportData;

    qreal m_yOffset;      // how far down the current page are we
    qreal m_topMargin;    // value stored in the correct units
    qreal m_bottomMargin; // -- same as above --
    qreal m_leftMargin;   // -- same as above --
    qreal m_rightMargin;  // -- same as above --
    qreal m_maxHeight;    // -- same as above --
    qreal m_maxWidth;     // -- same as above --
    int m_pageCounter;    // what page are we currently on?
    int m_recordCount;

    KoReportData* m_kodata;
    QList<OROTextBox*> m_postProcText;

#ifdef KREPORT_SCRIPTING
    QMap<QString, QObject*> m_scriptObjects;
#endif

    void createNewPage();
    qreal finishCurPage(bool = false);
    qreal finishCurPageSize(bool = false);

    void renderDetailSection(KRDetailSectionData &);
    qreal renderSection(const KRSectionData &);
    qreal renderSectionSize(const KRSectionData &);

    ///Scripting Stuff
    KRScriptHandler *m_scriptHandler;
#ifdef KREPORT_SCRIPTING
    void initEngine();
#endif

    KoReportASyncItemManager* asyncManager;

private Q_SLOTS:
    void asyncItemsFinished();

Q_SIGNALS:
    void enteredGroup(const QString&, const QVariant&);
    void exitedGroup(const QString&, const QVariant&);
    void renderingSection(KRSectionData*, OROPage*, QPointF);
};

#endif // __KOREPORTPRERENDERER_P_H__
