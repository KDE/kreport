/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __KOREPORTPRERENDERER_H__
#define __KOREPORTPRERENDERER_H__

#include <QObject>
#include <QString>
#include <QMap>

#include "kreport_export.h"
#include "krscripthandler.h"

class KoReportPreRendererPrivate;
class ORODocument;
class KoReportData;
class QDomElement;

//
// ORPreRender
// This class takes a report definition and prerenders the result to
// an ORODocument that can be used to pass to any number of renderers.
//
class KREPORT_EXPORT KoReportPreRenderer : public QObject
{
public:
    explicit KoReportPreRenderer(const QDomElement&);

    virtual ~KoReportPreRenderer();

    void setSourceData(KoReportData*);

#ifdef KREPORT_SCRIPTING
    void registerScriptObject(QObject*, const QString&);
#endif

    ORODocument * generate();

    /**
    @brief Set the name of the report so that it can be used internally by the script engine
    */
    void setName(const QString &);

    bool isValid() const;

    const KoReportReportData *reportData() const;

protected:

private:
    KoReportPreRendererPrivate *const d;
    bool setDom(const QDomElement &);
    QMap<QString, QObject*> m_scriptObjects;
};

#endif // __KOREPORTPRERENDERER_H__
