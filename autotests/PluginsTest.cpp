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

#include "PluginsTest.h"

#include <KoReportPluginManager>
#include <KoReportPluginInfo>

#include <QtTest/QTest>
#include <QDebug>
#include <QSet>

QTEST_MAIN(PluginsTest)

void PluginsTest::initTestCase()
{
    //QCoreApplication::setLibraryPaths(QStringList() << QDir::currentPath());
}

void PluginsTest::listPlugins()
{
    KoReportPluginManager* manager = KoReportPluginManager::self();
    QStringList pluginNames = manager->pluginNames();
    qDebug() << pluginNames;
    QCOMPARE(pluginNames.toSet().count(), pluginNames.count());
    foreach(const QString &pluginName, pluginNames) {
        KoReportPluginInterface* iface = manager->plugin(pluginName);
        QVERIFY(iface);
        QVERIFY(!iface->info()->className().isEmpty());
        //QVERIFY(!iface->info()->icon().isNull());
        QVERIFY(!iface->info()->name().isEmpty());
    }
}
