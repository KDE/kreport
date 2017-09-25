/* This file is part of the KDE project
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
#include "KReportScriptSection.h"

#include "KReportScriptLine.h"
#include "KReportItemBase.h"
#include "KReportPluginManager.h"
#include "KReportPluginInterface.h"
#include "KReportItemLine.h"
#include "KReportSectionData.h"
#include "kreport_debug.h"

namespace Scripting
{
Section::Section(KReportSectionData* sec)
{
    m_section = sec;
    m_scriptObject = 0;
}


Section::~Section()
{
}

QColor Section::backgroundColor() const
{
    return m_section->backgroundColor->value().value<QColor>();
}

void   Section::setBackgroundColor(const QColor &c)
{
    //kreportDebug() << c.name();
    m_section->backgroundColor->setValue(c);
}

qreal Section::height() const
{
    return m_section->height->value().toDouble();
}

void Section::setHeight(qreal h)
{
    m_section->height->setValue(h);
}

QString Section::name() const
{
    return m_section->objectName();
}

QObject* Section::objectAt(int i)
{
    if (m_section->objects()[i]->typeName() == QLatin1String("line")) {
        return new Scripting::Line(dynamic_cast<KReportItemLine *>(m_section->objects()[i]));
    } else {
        KReportPluginManager *manager = KReportPluginManager::self();
        KReportPluginInterface *plugin = manager->plugin(m_section->objects()[i]->typeName());
        if (plugin) {
            QObject *obj = plugin->createScriptInstance(m_section->objects()[i]);
            if (obj) {
                return obj;
            }
        } else {
            kreportWarning() << "Encountered unknown node while parsing section:"
                             << m_section->objects()[i]->typeName();
        }
    }

    return new QObject();
}

QObject* Section::objectForName(const QString& n)
{
    for (int i = 0; i < m_section->objects().count(); ++i) {
        if (m_section->objects()[i]->entityName() == n) {
            return objectAt(i);
        }
    }
    return nullptr;
}

void Section::initialize(const QJSValue &s)
{
    m_scriptObject = s;
}

void Section::eventOnRender()
{
    if (m_scriptObject.isObject() && m_scriptObject.hasProperty(QLatin1String("OnRender")))
        m_scriptObject.property(QLatin1String("OnRender")).call();
}
}
