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
#include <QList>

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

//! Compares @a actual and @a expected set, fails when not equal
//! On failure, all elements are displayed.
#define QCOMPARE_SET(type, actual, expected) \
do { \
    if (actual == expected) { \
        break; \
    } \
    QTest::qFail(#actual " != " #expected, __FILE__, __LINE__); \
    QList<type> actualList = actual.toList(); \
    QList<type> expectedList = expected.toList(); \
    qSort(actualList); \
    qSort(expectedList); \
    QList<type>::ConstIterator actualListIt = actualList.constBegin(); \
    QList<type>::ConstIterator expectedListIt = expectedList.constBegin(); \
    const int count = qMax(actualList.count(), expectedList.count()); \
    int i = 0; \
    bool stop; \
    forever { \
        stop = true; \
        QByteArray sign = "!="; \
        QString v1; \
        if (actualListIt == actualList.constEnd()) { \
            v1 = "<none>"; \
        } \
        else { \
            v1 = QVariant(*actualListIt).toString(); \
            stop = false; \
        } \
        QString v2; \
        if (expectedListIt == expectedList.constEnd()) { \
            v2 = "<none>"; \
        } \
        else { \
            v2 = QVariant(*expectedListIt).toString(); \
            stop = false; \
            if (actualListIt != actualList.constEnd()) { \
                if (*actualListIt == *expectedListIt) { \
                    sign = "=="; \
                } \
            } \
        } \
        if (stop) { \
            break; \
        } \
        qDebug() << QString("Actual/expected item %1 of %2: %3 %4 %5").arg(i + 1).arg(count) \
                    .arg(v1).arg(sign.constData()).arg(v2).toUtf8().constData(); \
        if (actualListIt != actualList.constEnd()) { \
            ++actualListIt; \
        } \
        if (expectedListIt != expectedList.constEnd()) { \
            ++expectedListIt; \
        } \
        ++i; \
    } \
    return; \
} while (0)

void PluginsTest::checkBuiltInPlugins()
{
    KoReportPluginManager* manager = KoReportPluginManager::self();
    QStringList pluginNames = manager->pluginNames();
    QCOMPARE(pluginNames.toSet().count(), pluginNames.count());
    QSet<QString> builtInPlugins(
        QSet<QString>() << "check" << "field" << "image" << "label" << "text");
    QSet<QString> foundBuiltInPlugins;
    foreach(const QString &pluginName, pluginNames) {
        KoReportPluginInterface* iface = manager->plugin(pluginName);
        QVERIFY(iface);
        if (iface->isBuiltIn()) {
            foundBuiltInPlugins.insert(pluginName);
        }
    }
    QCOMPARE_SET(QString, builtInPlugins, foundBuiltInPlugins);
}
