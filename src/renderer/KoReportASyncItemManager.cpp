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


#include "KoReportASyncItemManager.h"
#include "common/KoReportASyncItemBase.h"

#include "kreport_debug.h"

KoReportASyncItemManager::KoReportASyncItemManager(QObject* parent): QObject(parent)
{

}

KoReportASyncItemManager::~KoReportASyncItemManager()
{

}

void KoReportASyncItemManager::addItem(KoReportASyncItemBase* item, OROPage* page, OROSection* section, QPointF offset, QVariant data, KRScriptHandler* script)
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

void KoReportASyncItemManager::itemFinished()
{
    //kreportDebug();
    if (m_renderList.count() > 0) {
        RenderData *rdata = m_renderList.dequeue();
        rdata->item->renderSimpleData(rdata->page, rdata->section, rdata->offset, rdata->data, rdata->script);
    } else {
        emit(finished());
    }
}

void KoReportASyncItemManager::startRendering()
{
    //kreportDebug();
    if (m_renderList.count() > 0) {
        RenderData *rdata = m_renderList.dequeue();
        rdata->item->renderSimpleData(rdata->page, rdata->section, rdata->offset, rdata->data, rdata->script);
    } else {
        emit(finished());
    }
}
