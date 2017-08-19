/* This file is part of the KDE project
   Copyright Shreya Pandit <shreya@shreyapandit.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KReportWebPlugin.h"
#include "KReportDesignerItemWeb.h"
#include "KReportPluginMetaData.h"
#include <QIcon>

KREPORT_PLUGIN_FACTORY(KReportWebPlugin, "kreport_webplugin.json")

KReportWebPlugin::KReportWebPlugin(QObject *parent, const QVariantList &args)
    : KReportPluginInterface(parent)
{
    Q_UNUSED(args)
}

KReportWebPlugin::~KReportWebPlugin()
{
}

QObject *KReportWebPlugin::createRendererInstance(const QDomNode &element)
{
    return new KReportItemWeb(element);
}

QObject *KReportWebPlugin::createDesignerInstance(const QDomNode &element, KReportDesigner *designer,
                                                   QGraphicsScene *scene)
{
    return new KReportDesignerItemWeb(element, designer, scene);
}

QObject *KReportWebPlugin::createDesignerInstance(KReportDesigner *designer,
                                                   QGraphicsScene *scene,const QPointF &pos)
{
    return new KReportDesignerItemWeb(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject *KReportWebPlugin::createScriptInstance(KReportItemBase *item)
{
    Q_UNUSED(item);
//   KoReportItemweb *image = dynamic_cast<KoReportItemweb*>(item);
    // if (image) {
    //   return new Scripting::Web(image);
    return nullptr;
}
#endif

#include "KReportWebPlugin.moc"
