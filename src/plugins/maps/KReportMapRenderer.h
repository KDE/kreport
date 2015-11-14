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

#ifndef MAPRENDERER_H
#define MAPRENDERER_H

#include <marble/MarbleGlobal.h>
#include <marble/MarbleWidget.h>
#include <marble/RenderState.h>

#include "kreportplugin_debug.h"

class KReportItemMaps;
namespace Marble{
    /**
     * Workaround Marble error in signal specification.
     *
     * renderStatusChange and renderStateChange signals are using params that are in
     * Marble namespace and are not prefixed with Marble:: in their declaration in
     * MarbleWidget.h this class is workaround of the bug.
     */
    class RenderStatusSignalProxy: public QObject{
        Q_OBJECT
    public:
        explicit RenderStatusSignalProxy(QObject* parent):QObject(parent){};
        void setConnection(MarbleWidget* marble){
            connect(marble,SIGNAL(renderStatusChanged(RenderStatus)),this,SLOT(onRenderStatusChange(RenderStatus)));
            connect(marble,SIGNAL(renderStateChanged(RenderState)),this,SLOT(onRenderStateChange(RenderState)));
        };
        void disconnect(MarbleWidget* marble){
            disconnect(marble,SIGNAL(renderStatusChanged(RenderStatus)),this,SLOT(onRenderStatusChange(RenderStatus)));
            disconnect(marble,SIGNAL(renderStateChanged(RenderState)),this,SLOT(onRenderStateChange(RenderState)));
        }
    public Q_SLOTS:
        void onRenderStatusChange(RenderStatus renderStatus){
            kreportpluginDebug() << "!!!!!!!!!!!!!!!!  STATUS change";
            emit renderStatusChanged(static_cast<int>(renderStatus));
        };
        void onRenderStateChange(const RenderState &state){
            kreportpluginDebug() << "################  STATE change";
            emit renderStatusChanged(static_cast<int>(state.status()));
        };
    Q_SIGNALS:
        void renderStatusChanged(int renderStatus);
    };
}

class KReportMapRenderer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MapRenderer)
public:
    KReportMapRenderer(QObject* parent = 0);
    virtual ~KReportMapRenderer();
    void renderJob(KReportItemMaps* reportItemMaps);
Q_SIGNALS:
    void jobFinished();
private Q_SLOTS:
    void onRenderStatusChange(int renderStatus);
    void downloadProgres(int active, int queued);
    void downloadFinished();

private:
    Marble::MarbleWidget m_marble;
    KReportItemMaps* m_currentJob;
    Marble::RenderStatusSignalProxy m_renderStatusProxy;
};

#endif // MAPRENDERER_H