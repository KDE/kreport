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

#include "KReportFieldPlugin.h"
#include "KReportItemField.h"
#include "KReportDesignerItemField.h"
#include "KReportPluginMetaData.h"
#ifdef KREPORT_SCRIPTING
#include "KReportScriptField.h"
#endif

KREPORT_PLUGIN_FACTORY(KReportFieldPlugin, "field.json")

KReportFieldPlugin::KReportFieldPlugin(QObject *parent, const QVariantList &args)
    : KReportPluginInterface(parent, args)
{
}

KReportFieldPlugin::~KReportFieldPlugin()
{

}

QObject* KReportFieldPlugin::createRendererInstance(const QDomNode& element)
{
    return new KReportItemField(element);
}

QObject* KReportFieldPlugin::createDesignerInstance(const QDomNode& element, KReportDesigner* designer, QGraphicsScene* scene)
{
    return new KReportDesignerItemField(element, designer, scene);
}

QObject* KReportFieldPlugin::createDesignerInstance(KReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new KReportDesignerItemField(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject* KReportFieldPlugin::createScriptInstance(KReportItemBase* item)
{
    KReportItemField *field = dynamic_cast<KReportItemField*>(item);
    if (field) {
        return new Scripting::Field(field);
    }
    return 0;
}
#endif

#include "KReportFieldPlugin.moc"
