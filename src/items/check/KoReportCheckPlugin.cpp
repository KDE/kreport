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

#include "KoReportCheckPlugin.h"
#include "KoReportItemCheck.h"
#include "KoReportDesignerItemCheck.h"
#include "common/KReportPluginMetaData.h"
#include "KoReportScriptCheck.h"

KREPORT_PLUGIN_FACTORY(KoReportCheckPlugin, "check.json")

KoReportCheckPlugin::KoReportCheckPlugin(QObject *parent, const QVariantList &args)
    : KoReportPluginInterface(parent, args)
{
}

KoReportCheckPlugin::~KoReportCheckPlugin()
{

}

QObject* KoReportCheckPlugin::createRendererInstance(QDomNode& element)
{
    return new KoReportItemCheck(element);
}

QObject* KoReportCheckPlugin::createDesignerInstance(QDomNode& element, KoReportDesigner* designer, QGraphicsScene* scene)
{
    return new KoReportDesignerItemCheck(element, designer, scene);
}

QObject* KoReportCheckPlugin::createDesignerInstance(KoReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new KoReportDesignerItemCheck(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject* KoReportCheckPlugin::createScriptInstance(KoReportItemBase* item)
{
    KoReportItemCheck *check = dynamic_cast<KoReportItemCheck*>(item);
    if (check) {
       return new Scripting::Check(check);
    }
    return 0;
}
#endif

#include "KoReportCheckPlugin.moc"
