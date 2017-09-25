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

#ifndef KREPORTPRERENDERER_P_H
#define KREPORTPRERENDERER_P_H

#include "config-kreport.h"

#include "KReportRenderObjects.h"
#include "KReportAsyncItemManager_p.h"
#include "KReportDetailSectionData.h"

#include <QMap>

#ifdef KREPORT_SCRIPTING
#include <KReportScriptSource>
#endif

namespace KReportPrivate{
  class OneRecordDataSource;
}
class KReportPreRenderer;

/*! This class is the private class that houses all the internal
  variables so we can provide a cleaner interface to the user
  without presenting to them things that they don't need to see
  and may change over time. */
class KReportPreRendererPrivate : public QObject
{
    Q_OBJECT
public:
    KReportPreRendererPrivate(KReportPreRenderer *preRenderer);
    ~KReportPreRendererPrivate() override;

    KReportPreRenderer * const preRenderer;
    bool valid;

    ORODocument* document;
    OROPage*     page;
    KReportDocument* reportDocument;

    qreal yOffset;      // how far down the current page are we
    qreal topMargin;    // value stored in the correct units
    qreal bottomMargin; // -- same as above --
    qreal leftMargin;   // -- same as above --
    qreal rightMargin;  // -- same as above --
    qreal maxHeight;    // -- same as above --
    qreal maxWidth;     // -- same as above --
    int pageCounter;    // what page are we currently on?

    KReportDataSource* dataSource;
    KReportPrivate::OneRecordDataSource *oneRecord;

    QList<OROTextBox*> postProcText;

#ifdef KREPORT_SCRIPTING
    QMap<QString, QObject*> scriptObjects;
    KReportScriptSource *scriptSource = nullptr;
#endif

    void createNewPage();
    qreal finishCurPage(bool lastPage);
    qreal finishCurPageSize(bool lastPage);

    void renderDetailSection(KReportDetailSectionData *detailData);
    qreal renderSection(const KReportSectionData &);
    qreal renderSectionSize(const KReportSectionData &);

    ///Scripting Stuff
    KReportScriptHandler *scriptHandler;
#ifdef KREPORT_SCRIPTING
    void initEngine();
#endif

    //! Generates m_document. Returns true on success.
    //! @note m_document is not removed on failure, caller should remove it.
    bool generateDocument();

    KReportPrivate::AsyncItemManager* asyncManager;

private Q_SLOTS:
    void asyncItemsFinished();

Q_SIGNALS:
    void enteredGroup(const QString&, const QVariant&);
    void exitedGroup(const QString&, const QVariant&);
    void renderingSection(KReportSectionData*, OROPage*, QPointF);
    void finishedAllASyncItems();
};

#endif // __KOREPORTPRERENDERER_P_H__
