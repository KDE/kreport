/*
   Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)
   Copyright (C) 2011 by Radoslaw Wicik (radoslaw@wicik.pl)

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

#include "KoReportMapsPlugin.h"
#include "KoReportItemMaps.h"
#include "KoReportDesignerItemMaps.h"
#include <KReportPluginInterface.h>
#ifdef KREPORT_SCRIPTING
#include "krscriptmaps.h"
#endif


KREPORT_PLUGIN_FACTORY(KoReportMapsPlugin, "kreport_mapsplugin.json")

KReportMapsPlugin::KReportMapsPlugin(QObject *parent, const QVariantList &args) : KReportPluginInterface(parent)
{
    Q_UNUSED(args)
}

KReportMapsPlugin::~KReportMapsPlugin()
{
}

QObject* KReportMapsPlugin::createRendererInstance(const QDomNode& element)
{
    return new KReportItemMaps(element);
}

QObject* KReportMapsPlugin::createDesignerInstance(const QDomNode& element, KoReportDesigner* designer, QGraphicsScene* scene)
{
    return new KReportDesignerItemMaps(element, designer, scene);
}

QObject* KReportMapsPlugin::createDesignerInstance(KoReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new KReportDesignerItemMaps(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject* KoReportMapsPlugin::createScriptInstance(KReportItemBase* /*item*/)
{
    /*KoReportItemMaps *image = dynamic_cast<KoReportItemMaps*>(item);
    if (image) {
        return new Scripting::Maps(image);
    }*/
    return 0;
}
#endif

#include "KoReportMapsPlugin.moc"
