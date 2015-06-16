/* This file is part of the KDE project
   Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)
   Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KREPORTPLUGININTERFACE_H
#define KREPORTPLUGININTERFACE_H

#include "config-kreport.h"
#include "kreport_export.h"

#include <KPluginFactory>

class QGraphicsScene;
class QDomNode;
class KReportPluginMetaData;
class KoReportDesigner;

//! Implementation of report plugin's entry point
#define KREPORT_PLUGIN_FACTORY(class_name, name) \
    K_PLUGIN_FACTORY_WITH_JSON(class_name ## Factory, name, registerPlugin<class_name>();)

//! An interface for plugins delivering KReport elements
class KREPORT_EXPORT KoReportPluginInterface : public QObject
{
    Q_OBJECT
public:
    explicit KoReportPluginInterface(QObject *parent = 0,
                                     const QVariantList &args = QVariantList());

    virtual ~KoReportPluginInterface();

    virtual QObject* createDesignerInstance(KoReportDesigner *designer, QGraphicsScene * scene,
                                            const QPointF &pos) = 0;

    virtual QObject* createDesignerInstance(QDomNode &element, KoReportDesigner *designer,
                                            QGraphicsScene *scene) = 0;

    virtual QObject* createRendererInstance(QDomNode &element) = 0;

#ifdef KREPORT_SCRIPTING
    virtual QObject* createScriptInstance(KoReportItemBase* item) = 0;
#endif

    //! @return information about the plugin
    const KReportPluginMetaData* metaData() const;

private:
    friend class KReportPluginEntry;
    void setMetaData(KReportPluginMetaData* metaData);

    Q_DISABLE_COPY(KoReportPluginInterface)
    class Private;
    Private * const d;
};

#endif // KREPORTPLUGININTERFACE_H
