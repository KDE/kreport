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

#include "KReportScriptDebug.h"
#include "kreport_debug.h"

#include <KMessageBox>

KReportScriptDebug::KReportScriptDebug(QObject *parent)
        : QObject(parent)
{
}


KReportScriptDebug::~KReportScriptDebug()
{
}

void KReportScriptDebug::print(const QString& s)
{
    kreportDebug() << s;
}

void KReportScriptDebug::message(const QString &t, const QString &m)
{
    KMessageBox::information(nullptr, m, t);
}
