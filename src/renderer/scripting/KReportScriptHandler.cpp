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

#include "KReportScriptHandler.h"

#include "KReportScriptSection.h"
#include "KReportScriptDebug.h"
#include "KReportScriptReport.h"
#include "KReportScriptDraw.h"
#include "KReportScriptConstants.h"
#include "KReportSectionData.h"
#include "KReportItemBase.h"
#include "KReportDocument.h"
#include "KReportDetailSectionData.h"
#include "KReportRenderObjects.h"
#include "kreport_debug.h"

#include <QMessageBox>
#include <QJSEngine>
#include <QJSValue>

class Q_DECL_HIDDEN KReportScriptHandler::Private
{
public:
    Private();
    ~Private();
    KReportScriptConstants *constants;
    KReportScriptDebug *debug;
    KReportScriptDraw *draw;
    Scripting::Report *report;
    const KReportDataSource *reportDataSource;
    QString source;
    KReportDocument  *reportDocument;
    QJSEngine engine;
    QJSValue scriptValue;
    QMap<QString, QVariant> groups;
    QMap<KReportSectionData*, Scripting::Section*> sectionMap;
};

KReportScriptHandler::Private::Private() : constants(new KReportScriptConstants), debug(new KReportScriptDebug), draw(new KReportScriptDraw)
{
}

KReportScriptHandler::Private::~Private()
{
}

KReportScriptHandler::KReportScriptHandler(const KReportDataSource* reportDataSource, KReportDocument* reportDocument) : d(new Private())
{
    d->reportDocument = reportDocument;
    d->reportDataSource = reportDataSource;

    //Add a general report object
    d->report = new Scripting::Report(d->reportDocument);
    
    registerScriptObject(d->constants, QLatin1String("constants"));
    registerScriptObject(d->debug, QLatin1String("debug"));
    registerScriptObject(d->draw, QLatin1String("draw"));

    QJSValue r = registerScriptObject(d->report, d->reportDocument->name());

    //Add the sections
    QList<KReportSectionData*> secs = d->reportDocument->sections();
    foreach(KReportSectionData *sec, secs) {
        d->sectionMap[sec] = new Scripting::Section(sec);
        d->sectionMap[sec]->setParent(d->report);
        d->sectionMap[sec]->setObjectName(sec->name().replace(QLatin1Char('-'), QLatin1Char('_'))
                                         .remove(QLatin1String("report:")));
        QJSValue s = d->engine.newQObject(d->sectionMap[sec]);
        r.setProperty(d->sectionMap[sec]->objectName(), s);
        //kreportDebug() << "Added" << d->sectionMap[sec]->objectName() << "to report" << d->reportData->name();
    }

    //kreportDebug() << "Report name is" << d->reportData->name();
}

bool KReportScriptHandler::trigger()
{
    QString code = d->reportDataSource->scriptCode(d->reportDocument->script());
    //kreportDebug() << code;

    if (code.isEmpty()) {
        return true;
    }

    d->scriptValue = d->engine.evaluate(code, d->reportDocument->script());

    if (d->scriptValue.isError()) {
        return false;
    }/*TODO else {
        kreportDebug() << "Function Names:" << d->engine->functionNames();
    }*/
    d->report->eventOnOpen();
    return true;
}

KReportScriptHandler::~KReportScriptHandler()
{
    delete d;
}

void KReportScriptHandler::newPage()
{
    if (d->report) {
        d->report->eventOnNewPage();
    }
}

void KReportScriptHandler::slotEnteredGroup(const QString &key, const QVariant &value)
{
    //kreportDebug() << key << value;
    d->groups[key] = value;
    emit(groupChanged(d->groups));
}
void KReportScriptHandler::slotExitedGroup(const QString &key, const QVariant &value)
{
    Q_UNUSED(value);
    //kreportDebug() << key << value;
    d->groups.remove(key);
    emit(groupChanged(d->groups));
}

void KReportScriptHandler::slotEnteredSection(KReportSectionData *section, OROPage* cp, QPointF off)
{
    if (cp)
        d->draw->setPage(cp);
    d->draw->setOffset(off);

    Scripting::Section *ss = d->sectionMap[section];
    if (ss) {
        ss->eventOnRender();
    }
}

QVariant KReportScriptHandler::evaluate(const QString &code)
{
    if (!d->scriptValue.isError()) {
        QJSValue result = d->engine.evaluate(code);
        if (!result.isError()) {
            return result.toVariant();
        } else {
            QMessageBox::warning(0, tr("Script Error"), d->scriptValue.toString());
        }
    }
    return QVariant();
}

void KReportScriptHandler::displayErrors()
{
    if (d->scriptValue.isError()) {
        QMessageBox::warning(0, tr("Script Error"), d->scriptValue.toString());
    }
}

//! @todo KEXI3 move to kexi
#if 0
QString KReportScriptHandler::where()
{
    QString w;
    QMap<QString, QVariant>::const_iterator i = d->groups.constBegin();
    while (i != d->groups.constEnd()) {
        w += QLatin1Char('(') + i.key() + QLatin1String(" = '") + i.value().toString() + QLatin1String("') AND ");
        ++i;
    }
    w.chop(4);
    //kreportDebug() << w;
    return w;
}
#endif

QJSValue KReportScriptHandler::registerScriptObject(QObject* obj, const QString& name)
{
    QJSValue val;
    val = d->engine.newQObject(obj);
    d->engine.globalObject().setProperty(name, val);
    return val;
}

void KReportScriptHandler::setPageNumber(int p)
{
    d->constants->setPageNumber(p);
}

void KReportScriptHandler::setPageTotal(int t)
{
    d->constants->setPageTotal(t);
}
