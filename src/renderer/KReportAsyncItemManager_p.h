/*  This file is part of the KDE project
    Copyright (C) 2011  Adam Pigg <adam@piggz.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef KREPORTASYNCITEMMANAGER_H
#define KREPORTASYNCITEMMANAGER_H

#include "config-kreport.h"
#include "KReportAsyncItemBase.h"

#include <QObject>
#include <QQueue>
#include <KReportRenderObjects.h>

namespace KReportPrivate {

class RenderData;

class AsyncItemManager : public QObject
{
    Q_OBJECT

public:
    explicit AsyncItemManager(QObject *parent);
    ~AsyncItemManager() override;

    void addItem(KReportAsyncItemBase *item, OROPage *page, OROSection *section, QPointF offset, QVariant data, KReportScriptHandler *script);

    void startRendering();

Q_SIGNALS:
    void finished();

private Q_SLOTS:
    void itemFinished();

private:
    QQueue<RenderData*> m_renderList;
    QList<KReportAsyncItemBase*> m_itemList;
    OROPage *m_curPage;
};

}
#endif // KREPORTASYNCITEMMANAGER_H
