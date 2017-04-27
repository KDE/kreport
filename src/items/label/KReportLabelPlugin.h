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

#ifndef KReportLabelPlugin_H
#define KReportLabelPlugin_H

#include "KReportStaticPluginInterface.h"

KREPORT_DECLARE_STATIC_PLUGIN(KReportLabelPlugin)

class KReportLabelPlugin : public KReportPluginInterface
{
    Q_OBJECT
public:
    explicit KReportLabelPlugin(QObject *parent = nullptr,
                                const QVariantList &args = QVariantList());

    ~KReportLabelPlugin() override;

    QObject *createRendererInstance(const QDomNode &) override;
    QObject *createDesignerInstance(KReportDesigner *designer, QGraphicsScene *scene,
                                    const QPointF &) override;
    QObject *createDesignerInstance(const QDomNode &element, KReportDesigner *designer,
                                    QGraphicsScene *scene) override;
    KReportElement createElement() override;
    bool loadElement(KReportElement *el, const QDomElement &dom,
                     KReportDesignReadingStatus *status) override;
#ifdef KREPORT_SCRIPTING
    QObject *createScriptInstance(KReportItemBase *item) override;
#endif
};

#endif // KReportLabelPlugin_H
