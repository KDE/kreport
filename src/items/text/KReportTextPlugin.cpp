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

#include "KReportTextPlugin.h"
#include "KReportItemText.h"
#include "KReportDesignerItemText.h"
#include "KReportPluginMetaData.h"
#ifdef KREPORT_SCRIPTING
#include "KReportScriptText.h"
#endif

K_PLUGIN_CLASS_WITH_JSON(KReportTextPlugin, "text.json")

KReportTextPlugin::KReportTextPlugin(QObject *parent, const QVariantList &args)
    : KReportPluginInterface(parent, args)
{
}

KReportTextPlugin::~KReportTextPlugin()
{

}

QObject* KReportTextPlugin::createRendererInstance(const QDomNode& element)
{
    return new KReportItemText(element);
}

QObject* KReportTextPlugin::createDesignerInstance(const QDomNode& element, KReportDesigner* designer, QGraphicsScene* scene)
{
    return new KReportDesignerItemText(element, designer, scene);
}

QObject* KReportTextPlugin::createDesignerInstance(KReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new KReportDesignerItemText(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject* KReportTextPlugin::createScriptInstance(KReportItemBase* item)
{
    KReportItemText *text = dynamic_cast<KReportItemText*>(item);
    if (text) {
        return new Scripting::Text(text);
    }
    return nullptr;
}
#endif

#include "KReportTextPlugin.moc"
