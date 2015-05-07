/* This file is part of the KDE project
 * Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>
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

#ifndef KREPORTPLUGINSTEST_H
#define KREPORTPLUGINSTEST_H

#include <QCoreApplication>

class PluginsTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void listPlugins();
    void checkBuiltInPlugins();
};

#endif
