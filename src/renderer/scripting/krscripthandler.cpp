/* This file is part of the KDE project
 * Copyright (C) 2007-2008 by Adam Pigg <adam@piggz.co.uk>
 * Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>
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

#include "krscripthandler.h"

#include "krscriptsection.h"
#include "krscriptdebug.h"
#include "krscriptreport.h"
#include "krscriptdraw.h"
#include "krscriptconstants.h"

#include "krsectiondata.h"
#include "KoReportItemBase.h"
#include "krreportdata.h"
#include "krdetailsectiondata.h"
#include "renderobjects.h"

#include <QtGui/QMessageBox>

#include "kreport_debug.h"
#include <QtQml/QJSEngine>
#include <QtQml/QJSValue>

KRScriptHandler::KRScriptHandler(const KoReportData* kodata, KoReportReportData* d)
{
    m_reportData = d;
    m_koreportData = kodata;

    m_engine = 0;
    m_constants = 0;
    m_debug = 0;
    m_draw = 0;

    // Create the script engine instance .
    m_engine = new QJSEngine(this);

    //Add constants object
    m_constants = new KRScriptConstants();
    registerScriptObject(m_constants, QLatin1String("constants"));

    //A simple debug function to allow printing from functions
    m_debug = new KRScriptDebug();
    registerScriptObject(m_debug, QLatin1String("debug"));

    //A simple drawing object
    m_draw = new KRScriptDraw();
    registerScriptObject(m_draw, QLatin1String("draw"));

    //Add a general report object
    m_report = new Scripting::Report(m_reportData);
    QJSValue r = registerScriptObject(m_report, m_reportData->name());

    //Add the sections
    QList<KRSectionData*> secs = m_reportData->sections();
    foreach(KRSectionData *sec, secs) {
        m_sectionMap[sec] = new Scripting::Section(sec);
        m_sectionMap[sec]->setParent(m_report);
        m_sectionMap[sec]->setObjectName(sec->name().replace(QLatin1Char('-'), QLatin1Char('_'))
                                         .remove(QLatin1String("report:")));
        QJSValue s = m_engine->newQObject(m_sectionMap[sec]);
        r.setProperty(m_sectionMap[sec]->objectName(), s);
        kreportDebug() << "Added" << m_sectionMap[sec]->objectName() << "to report" << m_reportData->name();
    }


    kreportDebug() << "Report name is" << m_reportData->name();
}

void KRScriptHandler::trigger()
{
    //kreportDebug() << m_engine->code();
    QString code = m_koreportData->scriptCode(m_reportData->script(), QLatin1String(""));
    qDebug() << code;
    m_scriptValue = m_engine->evaluate(code);

    if (m_scriptValue.isError()) {
        QMessageBox::warning(0, tr("Script Error"), m_scriptValue.toString());
    }/*TODO else {
        kreportDebug() << "Function Names:" << m_engine->functionNames();
    }*/
    m_report->eventOnOpen();
}

KRScriptHandler::~KRScriptHandler()
{
    delete m_report;
    delete m_constants;
    delete m_debug;
    delete m_draw;
    delete m_engine;
}

void KRScriptHandler::newPage()
{
    if (m_report) {
        m_report->eventOnNewPage();
    }
}

void KRScriptHandler::slotEnteredGroup(const QString &key, const QVariant &value)
{
    //kreportDebug() << key << value;
    m_groups[key] = value;
    emit(groupChanged(where()));
}
void KRScriptHandler::slotExitedGroup(const QString &key, const QVariant &value)
{
    Q_UNUSED(value);
    //kreportDebug() << key << value;
    m_groups.remove(key);
    emit(groupChanged(where()));
}

void KRScriptHandler::slotEnteredSection(KRSectionData *section, OROPage* cp, QPointF off)
{
    if (cp)
        m_draw->setPage(cp);
    m_draw->setOffset(off);

    Scripting::Section *ss = m_sectionMap[section];
    if (ss) {
        ss->eventOnRender();
    }
}

QVariant KRScriptHandler::evaluate(const QString &code)
{
    if (!m_scriptValue.isError()) {
        QJSValue result = m_engine->evaluate(code);
        return result.toVariant();
    } else {
        return QVariant();
    }
}

void KRScriptHandler::displayErrors()
{
    if (m_scriptValue.isError()) {
        QMessageBox::warning(0, tr("Script Error"), m_scriptValue.toString());
    }
}

QString KRScriptHandler::where()
{
    QString w;
    QMap<QString, QVariant>::const_iterator i = m_groups.constBegin();
    while (i != m_groups.constEnd()) {
        w += QLatin1Char('(') + i.key() + QLatin1String(" = '") + i.value().toString() + QLatin1String("') AND ");
        ++i;
    }
    w.chop(4);
    //kreportDebug() << w;
    return w;
}

QJSValue KRScriptHandler::registerScriptObject(QObject* obj, const QString& name)
{
    QJSValue val;
    if (m_engine) {
        val = m_engine->newQObject(obj);
        m_engine->globalObject().setProperty(name, val);
    }
    return val;
}

