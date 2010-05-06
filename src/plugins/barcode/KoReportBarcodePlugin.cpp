/*
   KoReport Library
   Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)

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

#include "KoReportBarcodePlugin.h"
#include "KoReportItemBarcode.h"
#include "KoReportDesignerItemBarcode.h"
#include "KoReportPluginInfo.h"

KoReportBarcodePlugin::KoReportBarcodePlugin()
{
    KoReportPluginInfo *info = new KoReportPluginInfo();
    info->setEntityName("report:barcode");
    info->setIconName("view-barcode");
    info->setUserName(i18n("Barcode"));
    info->setPriority(50);
    setInfo(info);
}

KoReportBarcodePlugin::~KoReportBarcodePlugin()
{

}

QObject* KoReportBarcodePlugin::createRendererInstance(QDomNode& element)
{
    return new KoReportItemBarcode(element);
}

QObject* KoReportBarcodePlugin::createDesignerInstance(QDomNode& element, KoReportDesigner* designer , QGraphicsScene* scene)
{
    return new KoReportDesignerItemBarcode(element, designer, scene);
}

QObject* KoReportBarcodePlugin::createDesignerInstance(KoReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new KoReportDesignerItemBarcode(designer, scene, pos);
}

