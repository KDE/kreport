/* This file is part of the KDE project
   Copyright (C) 2011, 2012 by Dag Andersen (danders@get2net.dk)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KoOdtFrameReportPrimitive.h"
#include "KReportRenderObjects.h"

#include <KoXmlWriter.h>
#include <KReportDpi.h>
#include <KoGenStyle.h>
#include <KoGenStyles.h>

KoOdtFrameReportPrimitive::KoOdtFrameReportPrimitive(OROPrimitive *primitive)
    : m_primitive(primitive)
    , m_uid(0)
{
}

KoOdtFrameReportPrimitive::~KoOdtFrameReportPrimitive()
{
}

bool KoOdtFrameReportPrimitive::isValid() const
{
    return (bool)m_primitive;
}

void KoOdtFrameReportPrimitive::setPrimitive(OROPrimitive *primitive)
{
    m_primitive = primitive;
}

int KoOdtFrameReportPrimitive::pageNumber() const
{
    return isValid() && m_primitive->page() ? m_primitive->page()->pageNumber() + 1 : 0;
}

void KoOdtFrameReportPrimitive::setUID(int uid)
{
    m_uid = uid;
}

int KoOdtFrameReportPrimitive::uid() const
{
    return m_uid;
}

QString KoOdtFrameReportPrimitive::itemName() const
{
    return QString("Item_%1").arg(m_uid);
}

void KoOdtFrameReportPrimitive::createStyle(KoGenStyles *coll)
{
    KoGenStyle gs(KoGenStyle::GraphicStyle, "graphic");
    gs.addProperty("draw:fill", "none");
    gs.addPropertyPt("fo:margin", 0);
    gs.addProperty("style:horizontal-pos", "from-left");
    gs.addProperty("style:horizontal-rel", "page");
    gs.addProperty("style:vertical-pos", "from-top");
    gs.addProperty("style:vertical-rel", "page");
    gs.addProperty("style:wrap", "dynamic");
    gs.addPropertyPt("style:wrap-dynamic-threshold", 0);

    m_frameStyleName = coll->insert(gs, "F");
}

void KoOdtFrameReportPrimitive::createBody(KoXmlWriter *bodyWriter) const
{
    Q_UNUSED(bodyWriter);
}

void KoOdtFrameReportPrimitive::commonAttributes(KoXmlWriter *bodyWriter) const
{
    // convert to inches
    qreal x = m_primitive->position().x() / KReportPrivate::dpiX();
    qreal y = m_primitive->position().y() / KReportPrivate::dpiX();
    qreal w = m_primitive->size().width() / KReportPrivate::dpiX();
    qreal h = m_primitive->size().height() / KReportPrivate::dpiY();

    bodyWriter->addAttribute("svg:x", QString("%1in").arg(x));
    bodyWriter->addAttribute("svg:y", QString("%1in").arg(y));
    bodyWriter->addAttribute("svg:width", QString("%1in").arg(w));
    bodyWriter->addAttribute("svg:height", QString("%1in").arg(h));
    bodyWriter->addAttribute("draw:z-index", "3");
}

bool KoOdtFrameReportPrimitive::saveData(KoStore */*store*/, KoXmlWriter*) const
{
    return true;
}

