/*
 * Copyright (C) 2007-2016 by Adam Pigg (adam@piggz.co.uk)
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

#include "KReportScriptMaps.h"
#include "KReportItemMaps.h"

namespace Scripting
{

Maps::Maps(KReportItemMaps *i)
{
    m_map = i;
    m_map->m_latDataSetFromScript = false;
    m_map->m_longDataSetFromScript = false;
    m_map->m_zoomDataSetFromScript = false;
}

Maps::~Maps()
{
}

QPointF Maps::position() const
{
    return m_map->m_pos.toPoint();
}

void Maps::setPosition(const QPointF& p)
{
    m_map->m_pos.setPointPos(p);
}

QSizeF Maps::size() const
{
    return m_map->m_size.toPoint();
}

void Maps::setSize(const QSizeF& s)
{
    m_map->m_size.setPointSize(s);
}

void Maps::setLatitude(qreal latitude)
{
    m_map->m_latitude = latitude;
    m_map->m_latDataSetFromScript = true;
}

void Maps::setLongitude(qreal longitude)
{
    m_map->m_longtitude = longitude;
    m_map->m_longDataSetFromScript = true;
}

void Maps::setZoom(int zoom)
{
    m_map->m_zoom = zoom;
    m_map->m_zoomDataSetFromScript = true;
}

}
