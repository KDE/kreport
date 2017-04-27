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

#ifndef KREPORTCHECKPLUGIN_H
#define KREPORTCHECKPLUGIN_H

#include "KReportStaticPluginInterface.h"

KREPORT_DECLARE_STATIC_PLUGIN(KReportCheckBoxPlugin)

class KReportCheckBoxPlugin : public KReportPluginInterface
{
public:
    explicit KReportCheckBoxPlugin(QObject *parent, const QVariantList &args = QVariantList());
    ~KReportCheckBoxPlugin() override;

    QObject *createRendererInstance(const QDomNode &element) override;
    QObject *createDesignerInstance(const QDomNode &element, KReportDesigner *designer,
                                    QGraphicsScene *scene) override;
    QObject *createDesignerInstance(KReportDesigner *designer, QGraphicsScene *scene,
                                    const QPointF &pos) override;
#ifdef KREPORT_SCRIPTING
    QObject *createScriptInstance(KReportItemBase *item) override;
#endif
};

#endif // KREPORTCHECKPLUGIN_H
