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

#include "KReportCheckBoxPlugin.h"
#include "KReportItemCheck.h"
#include "KReportDesignerItemCheckBox.h"
#include "KReportPluginMetaData.h"
#include "KReportScriptCheck.h"

KREPORT_PLUGIN_FACTORY(KReportCheckBoxPlugin, "check.json")

KReportCheckBoxPlugin::KReportCheckBoxPlugin(QObject *parent, const QVariantList &args)
    : KReportPluginInterface(parent, args)
{
}

KReportCheckBoxPlugin::~KReportCheckBoxPlugin()
{

}

QObject* KReportCheckBoxPlugin::createRendererInstance(const QDomNode& element)
{
    return new KReportItemCheckBox(element);
}

QObject* KReportCheckBoxPlugin::createDesignerInstance(const QDomNode& element, KReportDesigner* designer, QGraphicsScene* scene)
{
    return new KReportDesignerItemCheckBox(element, designer, scene);
}

QObject* KReportCheckBoxPlugin::createDesignerInstance(KReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new KReportDesignerItemCheckBox(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject* KReportCheckBoxPlugin::createScriptInstance(KReportItemBase* item)
{
    KReportItemCheckBox *check = dynamic_cast<KReportItemCheckBox*>(item);
    if (check) {
       return new Scripting::CheckBox(check);
    }
    return nullptr;
}
#endif

#include "KReportCheckBoxPlugin.moc"
