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

#include "KReportImagePlugin.h"
#include "KReportItemImage.h"
#include "KReportDesignerItemImage.h"
#include "KReportPluginMetaData.h"
#ifdef KREPORT_SCRIPTING
#include "KReportScriptImage.h"
#endif

K_PLUGIN_CLASS_WITH_JSON(KReportImagePlugin, "image.json")

KReportImagePlugin::KReportImagePlugin(QObject *parent, const QVariantList &args)
    : KReportPluginInterface(parent, args)
{
    Q_UNUSED(args)
}

KReportImagePlugin::~KReportImagePlugin()
{

}

QObject* KReportImagePlugin::createRendererInstance(const QDomNode& element)
{
    return new KReportItemImage(element);
}

QObject* KReportImagePlugin::createDesignerInstance(const QDomNode& element, KReportDesigner* designer, QGraphicsScene* scene)
{
    return new KReportDesignerItemImage(element, designer, scene);
}

QObject* KReportImagePlugin::createDesignerInstance(KReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new KReportDesignerItemImage(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject* KReportImagePlugin::createScriptInstance(KReportItemBase* item)
{
    KReportItemImage *image = dynamic_cast<KReportItemImage*>(item);
    if (image) {
        return new Scripting::Image(image);
    }
    return nullptr;
}
#endif

#include "KReportImagePlugin.moc"
