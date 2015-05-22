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
#include <KoReportPluginInterface.h>
#ifdef KREPORT_SCRIPTING
#include "krscriptmaps.h"
#endif

#include <QIcon>

KREPORT_PLUGIN_FACTORY(KoReportMapsPlugin, "kreport_mapsplugin.json")

KoReportMapsPlugin::KoReportMapsPlugin(QObject *parent, const QVariantList &args) : KoReportPluginInterface(parent)
{
    Q_UNUSED(args)
}

KoReportMapsPlugin::~KoReportMapsPlugin()
{
}

QObject* KoReportMapsPlugin::createRendererInstance(QDomNode& element)
{
    return new KoReportItemMaps(element);
}

QObject* KoReportMapsPlugin::createDesignerInstance(QDomNode& element, KoReportDesigner* designer, QGraphicsScene* scene)
{
    return new KoReportDesignerItemMaps(element, designer, scene);
}

QObject* KoReportMapsPlugin::createDesignerInstance(KoReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new KoReportDesignerItemMaps(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject* KoReportMapsPlugin::createScriptInstance(KoReportItemBase* /*item*/)
{
    /*KoReportItemMaps *image = dynamic_cast<KoReportItemMaps*>(item);
    if (image) {
        return new Scripting::Maps(image);
    }*/
    return 0;
}
#endif

#include "KoReportMapsPlugin.moc"
