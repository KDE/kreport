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


#include "KReportAsyncItemManager_p.h"
#include "KReportAsyncItemBase.h"
#include "kreport_debug.h"

#include <QPointF>

namespace KReportPrivate {

class RenderData {
public:
    KReportAsyncItemBase* item;
    OROPage* page;
    OROSection* section;
    QPointF offset;
    QVariant data;
    KReportScriptHandler* script;
};

AsyncItemManager::AsyncItemManager(QObject *parent) : QObject(parent), m_curPage(nullptr)
{
}

AsyncItemManager::~AsyncItemManager()
{

}

void AsyncItemManager::addItem(KReportAsyncItemBase* item, OROPage* page, OROSection* section, QPointF offset, QVariant data, KReportScriptHandler* script)
{
    RenderData *rdata = new RenderData();
    rdata->item = item;
    rdata->page = page;
    rdata->section = section;
    rdata->offset = offset;
    rdata->data = data;

#ifdef KREPORT_SCRIPTING
    rdata->script = script;
#else
    Q_UNUSED(script);
#endif
    m_renderList.enqueue(rdata);

    //Just connect the first instance
    if (!m_itemList.contains(item)) {
        m_itemList.append(item);
        connect(item, SIGNAL(finishedRendering()), this, SLOT(itemFinished()));
    }
    //kreportDebug() << m_renderList.count();
}

void AsyncItemManager::itemFinished()
{
    m_curPage->document()->updated(m_curPage->pageNumber());
    //kreportDebug();
    if (m_renderList.count() > 0) {
        RenderData *rdata = m_renderList.dequeue();
        m_curPage = rdata->page;
        rdata->item->renderSimpleData(rdata->page, rdata->section, rdata->offset, rdata->data, rdata->script);
    } else {
        emit(finished());
    }
}

void AsyncItemManager::startRendering()
{
    //kreportDebug();
    if (m_renderList.count() > 0) {
        RenderData *rdata = m_renderList.dequeue();
        m_curPage = rdata->page;
        rdata->item->renderSimpleData(rdata->page, rdata->section, rdata->offset, rdata->data, rdata->script);
    } else {
        emit(finished());
    }
}

}
