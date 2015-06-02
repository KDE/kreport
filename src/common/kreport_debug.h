/* This file is part of the KDE project
 * Copyright (C) 2015 Laurent Montel <montel@kde.org>
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

#ifndef KREPORT_DEBUG_H
#define KREPORT_DEBUG_H

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(KREPORT_LOG)

#define kreportDebug(...) qCDebug(KREPORT_LOG, __VA_ARGS__)
#define kreportWarning(...) qCWarning(KREPORT_LOG, __VA_ARGS__)
#define kreportCritical(...) qCCritical(KREPORT_LOG, __VA_ARGS__)

#endif
