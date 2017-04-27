/* This file is part of the KDE project
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

#include "KReportBarcodePlugin.h"
#include "KReportItemBarcode.h"
#include "KReportDesignerItemBarcode.h"
#include "KReportPluginMetaData.h"
#ifdef KREPORT_SCRIPTING
#include "KReportScriptBarcode.h"
#endif


KREPORT_PLUGIN_FACTORY(KReportBarcodePlugin, "kreport_barcodeplugin.json")

KReportBarcodePlugin::KReportBarcodePlugin(QObject *parent, const QVariantList &args)
    : KReportPluginInterface(parent, args)
{
}

KReportBarcodePlugin::~KReportBarcodePlugin()
{
}

QObject* KReportBarcodePlugin::createRendererInstance(const QDomNode& element)
{
    return new KReportItemBarcode(element);
}

QObject* KReportBarcodePlugin::createDesignerInstance(const QDomNode& element, KReportDesigner* designer , QGraphicsScene* scene)
{
    return new KReportDesignerItemBarcode(element, designer, scene);
}

QObject* KReportBarcodePlugin::createDesignerInstance(KReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new KReportDesignerItemBarcode(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject* KReportBarcodePlugin::createScriptInstance(KReportItemBase* item)
{
    KReportItemBarcode *barcode = dynamic_cast<KReportItemBarcode*>(item);
    if (barcode) {
        return new Scripting::Barcode(barcode);
    }
    return nullptr;
}
#endif

#include "KReportBarcodePlugin.moc"
