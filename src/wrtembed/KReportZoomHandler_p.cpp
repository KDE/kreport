/* This file is part of the KDE project
   Copyright (C) 2001-2005 David Faure <faure@kde.org>
   Copyright (C) 2006 Thomas Zander <zander@kde.org>
   Copyright (C) 2010 KO GmbH <boud@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KReportZoomHandler_p.h"
#include "KReportUnit.h" // for POINT_TO_INCH
#include "KReportUtils_p.h"

#include <QPointF>
#include <QRectF>

KReportZoomHandler::KReportZoomHandler()
    : m_zoomMode(KReportZoomMode::ZOOM_CONSTANT)
    , m_resolutionX(0)
    , m_resolutionY(0)
    , m_zoomedResolutionX(0)
    , m_zoomedResolutionY(0)
    , m_zoomLevel(1.0)
{
    setZoom(1.0);
    setZoomMode( KReportZoomMode::ZOOM_CONSTANT );
    setDpi(KReportPrivate::dpiX(), KReportPrivate::dpiY());
}

KReportZoomHandler::~KReportZoomHandler()
{
}

void KReportZoomHandler::setResolutionToStandard()
{
    setDpi(KReportPrivate::dpiX(), KReportPrivate::dpiY());
}

void KReportZoomHandler::setDpi(int dpiX, int dpiY)
{
    setResolution(POINT_TO_INCH(static_cast<qreal>(dpiX)),
                  POINT_TO_INCH(static_cast<qreal>(dpiY)));
}

void KReportZoomHandler::setResolution( qreal resolutionX, qreal resolutionY )
{

    m_resolutionX = resolutionX;
    m_resolutionY = resolutionY;

    if (qFuzzyCompare(m_resolutionX, 1))
        m_resolutionX = 1;
    if (qFuzzyCompare(m_resolutionY, 1))
        m_resolutionY = 1;

    m_zoomedResolutionX = zoom() * resolutionX;
    m_zoomedResolutionY = zoom() * resolutionY;
}

void KReportZoomHandler::setZoomedResolution( qreal zoomedResolutionX, qreal zoomedResolutionY )
{
    // zoom() doesn't matter, it's only used in setZoom() to calculated the zoomed resolutions
    // Here we know them. The whole point of this method is to allow a different zoom factor
    // for X and for Y, as can be useful for e.g. fullscreen kpresenter presentations.
    m_zoomedResolutionX = zoomedResolutionX;
    m_zoomedResolutionY = zoomedResolutionY;
}

void KReportZoomHandler::setZoom( qreal zoom )
{
    if (qFuzzyCompare(zoom, qreal(0.0)) || qFuzzyCompare(zoom, qreal(1.0))) {
        zoom = 1.0;
    }

    m_zoomLevel = zoom;

    if( zoom == 1.0 ) {
        m_zoomedResolutionX = m_resolutionX;
        m_zoomedResolutionY = m_resolutionY;
    } else {
        m_zoomedResolutionX = zoom * m_resolutionX;
        m_zoomedResolutionY = zoom * m_resolutionY;
    }
}

QPointF KReportZoomHandler::documentToView( const QPointF &documentPoint )  const
{
    return QPointF( zoomItX( documentPoint.x() ),
                    zoomItY( documentPoint.y() ));
}

QPointF KReportZoomHandler::viewToDocument( const QPointF &viewPoint )  const
{
    return QPointF( unzoomItX( viewPoint.x() ),
                    unzoomItY( viewPoint.y() ) );
}

QRectF KReportZoomHandler::documentToView( const QRectF &documentRect )  const
{
    QRectF r (zoomItX( documentRect.x() ),
              zoomItY( documentRect.y() ),
              zoomItX( documentRect.width() ),
              zoomItY( documentRect.height() ) );
    return r;
}

QRectF KReportZoomHandler::viewToDocument( const QRectF &viewRect )  const
{
    QRectF r (  unzoomItX( viewRect.x() ),
                unzoomItY( viewRect.y()),
                unzoomItX( viewRect.width() ),
                unzoomItY( viewRect.height() ) );
    return r;
}

QSizeF KReportZoomHandler::documentToView( const QSizeF &documentSize ) const
{
    return QSizeF( zoomItX( documentSize.width() ),
                   zoomItY( documentSize.height() ) );
}

QSizeF KReportZoomHandler::viewToDocument( const QSizeF &viewSize ) const
{
    return QSizeF( unzoomItX( viewSize.width() ),
                   unzoomItY( viewSize.height() ) );
}

qreal KReportZoomHandler::documentToViewX( qreal documentX ) const
{
    return zoomItX( documentX );
}

qreal KReportZoomHandler::documentToViewY( qreal documentY ) const
{
    return zoomItY( documentY );
}

qreal KReportZoomHandler::viewToDocumentX( qreal viewX ) const
{
    return unzoomItX( viewX );
}

qreal KReportZoomHandler::viewToDocumentY( qreal viewY ) const
{
    return unzoomItY( viewY );
}

void KReportZoomHandler::zoom(qreal *zoomX, qreal *zoomY) const
{
    *zoomX = zoomItX(100.0) / 100.0;
    *zoomY = zoomItY(100.0) / 100.0;
}

qreal KReportZoomHandler::zoom() const
{
    return m_zoomLevel;
}
