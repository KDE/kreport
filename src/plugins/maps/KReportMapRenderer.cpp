/*
 * Copyright (C) 2015  Radosław Wicik <radoslaw@wicik.pl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "KReportMapRenderer.h"
#include "KReportItemMaps.h"
#include <KReportRenderObjects.h>

#include <marble/MarbleWidget.h>
#include <marble/MarbleGlobal.h>
#include <marble/MarbleModel.h>
#include <marble/HttpDownloadManager.h>
#include <marble/AbstractFloatItem.h>
#include <marble/GeoPainter.h>

#include "kreportplugin_debug.h"

KReportMapRenderer::KReportMapRenderer(QObject* parent)
    : QObject(parent)
    , m_currentJob(0)
{
    m_marble.setMapThemeId(QLatin1String("earth/openstreetmap/openstreetmap.dgml"));
    m_marble.setShowOverviewMap(false);
    m_marble.setMapQualityForViewContext(Marble::PrintQuality, Marble::Still);
    m_marble.setShowCrosshairs(true);

    foreach(Marble::AbstractFloatItem* floatItem, m_marble.floatItems()){
        if(floatItem->nameId() == QString(QLatin1String("navigation"))){
            floatItem->setVisible(false);
        }
    }

    connect(m_marble.model()->downloadManager(), &Marble::HttpDownloadManager::progressChanged, this, &KReportMapRenderer::downloadProgres);
    connect(&m_marble, &Marble::MarbleMap::renderStatusChanged, this, &KReportMapRenderer::onRenderStatusChange);
    connect(&m_retryTimer,  &QTimer::timeout, this, &KReportMapRenderer::retryRender);
}

KReportMapRenderer::~KReportMapRenderer()
{

}

void KReportMapRenderer::renderJob(KReportItemMaps* reportItemMaps)
{
    m_currentJob = reportItemMaps;
    int zoom = m_currentJob->zoom();
    //kreportpluginDebug() << "Map Renderer rendering" << m_currentJob->longtitude() << m_currentJob->latitude();

    m_marble.setMapThemeId(m_currentJob->themeId());
    //some themes enable overview map, and this must be disabled after theme switch.
    m_marble.setShowOverviewMap(false);
    m_marble.setSize(KReportItemBase::sceneSize(m_currentJob->size()).toSize());
    m_marble.centerOn(m_currentJob->longtitude(), m_currentJob->latitude());
    m_marble.setRadius(pow(M_E, (zoom / 200.0)));
    
    // Create a painter that will do the painting.
    Marble::GeoPainter geoPainter( m_currentJob->oroImage()->picture(), m_marble.viewport(), m_marble.mapQuality() );
    m_marble.paint( geoPainter, QRect() );
    
    if (m_marble.renderStatus() == Marble::Complete) {
        m_currentJob->renderFinished();
    } else {
        m_retryTimer.start(1000);
    }
            
}

void KReportMapRenderer::onRenderStatusChange(Marble::RenderStatus renderStatus)
{
    //kreportpluginDebug() << m_marble.renderStatus() << "|" << renderStatus;

    if(m_currentJob){
        /*kreportpluginDebug()
            << this
            << m_currentJob
            << m_currentJob->longtitude()
            << m_currentJob->latitude()
            << m_currentJob->zoom()
            << " | status: " << renderStatus;*/

        if(renderStatus == Marble::Complete){
            m_currentJob->renderFinished();
        }
    }
}

void KReportMapRenderer::downloadProgres(int active, int queued)
{
    Q_UNUSED(active)
    Q_UNUSED(queued)
    //if(m_currentJob){
    //    kreportpluginDebug() << "job:" << m_currentJob
    //    << "(" << m_currentJob->latitude() << "," << m_currentJob->longtitude() << ")"
    //    << "active/queued:" << active << "/" << queued;
    //}
}

void KReportMapRenderer::retryRender()
{
    //kreportpluginDebug() << "Retrying map render";

    // Create a painter that will do the painting.
    Marble::GeoPainter geoPainter( m_currentJob->oroImage()->picture(), m_marble.viewport(), m_marble.mapQuality() );
    m_marble.paint( geoPainter, QRect() );
    
    if (m_marble.renderStatus() == Marble::Complete) {
        m_retryTimer.stop();
        m_currentJob->renderFinished();
    }
}

