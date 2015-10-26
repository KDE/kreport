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

#ifndef KREPORTWEBPLUGIN_H
#define KREPORTWEBPLUGIN_H

#include "KReportPluginInterface.h"

class KReportWebPlugin : public KReportPluginInterface
{
    Q_OBJECT
public:
    explicit KReportWebPlugin(QObject *parent, const QVariantList &args = QVariantList());
    virtual ~KReportWebPlugin();

    virtual QObject *createRendererInstance(const QDomNode &element);
    virtual QObject *createDesignerInstance(const QDomNode &element, KoReportDesigner *designer,
                                            QGraphicsScene *scene);
    virtual QObject *createDesignerInstance(KoReportDesigner *designer,
                                            QGraphicsScene *scene,const QPointF &pos);
#ifdef KREPORT_SCRIPTING
    virtual QObject *createScriptInstance(KReportItemBase *item);
#endif
};

#endif // KOREPORTWEBPLUGIN_H
