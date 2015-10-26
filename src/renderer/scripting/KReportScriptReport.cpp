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
#include "KReportScriptReport.h"
#include "KReportDocument.h"
#include "KReportItemBase.h"
#include "KReportPluginManager.h"
#include "KReportPluginInterface.h"
#include "KReportScriptLine.h"
#include "KReportScriptSection.h"
#include "KReportItemLine.h"
#include "kreport_debug.h"

namespace Scripting
{

Report::Report(KReportDocument *r)
{
    m_reportData = r;
    m_scriptObject = 0;
}


Report::~Report()
{

}

QString Report::title()
{
    return m_reportData->m_title;
}

QString Report::name()
{
    return m_reportData->name();
}

QString Report::recordSource()
{
    return m_reportData->query();
}

QObject* Report::objectByName(const QString &n)
{
    QList<KReportItemBase *>obs = m_reportData->objects();
    foreach(KReportItemBase *o, obs) {
        if (o->entityName() == n) {

            if (o->typeName() == QLatin1String("line")) {
                        return new Scripting::Line(dynamic_cast<KReportItemLine*>(o));
            }
            else {
                KReportPluginManager* manager = KReportPluginManager::self();
                KReportPluginInterface *plugin = manager->plugin(o->typeName());
                if (plugin) {
                    QObject *obj = plugin->createScriptInstance(o);
                    if (obj) {
                        return obj;
                    }
                }
                else {
                    kreportWarning() << "Encountered unknown node while parsing section: " << o->typeName();
                }
            }
        }
    }
    return 0;
}

QObject* Report::sectionByName(const QString &n)
{
    KReportSectionData *sec = m_reportData->section(n);
    if (sec) {
        return new Scripting::Section(sec);
    } else {
        return new QObject();
    }
}

void Report::initialize(const QJSValue &val)
{
    m_scriptObject = val;
}

void Report::eventOnOpen()
{
    if (m_scriptObject.isObject() && m_scriptObject.hasProperty(QLatin1String("OnOpen")))
        m_scriptObject.property(QLatin1String("OnOpen")).call();
}

void Report::eventOnComplete()
{
    if (m_scriptObject.isObject() && m_scriptObject.hasProperty(QLatin1String("OnComlete")))
        m_scriptObject.property(QLatin1String("OnComplete")).call();
}

void Report::eventOnNewPage()
{
    if (m_scriptObject.isObject() && m_scriptObject.hasProperty(QLatin1String("OnNewPage")))
        m_scriptObject.property(QLatin1String("OnNewPage")).call();
}

}
