/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright     2007       David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KReportJsonTrader_p.h"

#include <QDebug>
#include <QApplication>
#include <QList>
#include <QPluginLoader>
#include <QJsonObject>
#include <QJsonArray>
#include <QDirIterator>
#include <QDir>
#include <QGlobalStatic>

Q_GLOBAL_STATIC(KReportJsonTrader, KReportJsonTrader_instance)

KReportJsonTrader::KReportJsonTrader()
{
    Q_ASSERT(!KReportJsonTrader_instance.exists());
}

KReportJsonTrader* KReportJsonTrader::self()
{
    return KReportJsonTrader_instance;
}

QList<QPluginLoader *> KReportJsonTrader::query(const QString &servicetype, const QString &mimetype)
{
    if (m_pluginPath.isEmpty()) {

        QList<QDir> searchDirs;

        QDir appDir(qApp->applicationDirPath());
        appDir.cdUp();

        searchDirs << appDir;
#ifdef CMAKE_INSTALL_PREFIX
        searchDirs << QDir{CMAKE_INSTALL_PREFIX};
#endif

        foreach(const QDir& dir, searchDirs) {
            foreach(QString entry, dir.entryList()) {
                QFileInfo info(dir, entry);
                if (info.isDir() && info.fileName().contains(QLatin1String("lib"))) {
                    QDir libDir(info.absoluteFilePath());

                    QString pluginSubdir(QLatin1String("kreport"));
                    // on many systems this will be the actual lib dir (and a subdir that contains plugins)
                    if (!libDir.entryList(QStringList() << pluginSubdir).isEmpty()) {
                        m_pluginPath = info.absoluteFilePath() + QLatin1Char('/') + pluginSubdir;
                        break;
                    }

                    // on debian at least the actual libdir is a subdir named like "lib/x86_64-linux-gnu"
                    // so search there for the subdir which will contain our plugins
                    foreach(const QString &subEntry, libDir.entryList()) {
                        QFileInfo subInfo(libDir, subEntry);
                        if (subInfo.isDir()) {
                            if (!QDir(subInfo.absoluteFilePath()).entryList(QStringList() << pluginSubdir).isEmpty()) {
                                m_pluginPath = subInfo.absoluteFilePath() + QLatin1Char('/') + pluginSubdir;
                                break; // will only break inner loop so we need the extra check below
                            }
                        }
                    }

                    if (!m_pluginPath.isEmpty()) {
                        break;
                    }
                }
            }

            if(!m_pluginPath.isEmpty()) {
                break;
            }
        }
        //qDebug() << "KReportJsonTrader will load its plugins from" << m_pluginPath;

    }

    QList<QPluginLoader *>list;
    QDirIterator dirIter(m_pluginPath, QDirIterator::Subdirectories);
    while (dirIter.hasNext()) {
        dirIter.next();
        if (dirIter.fileInfo().isFile()) {
            QPluginLoader *loader = new QPluginLoader(dirIter.filePath());
            QJsonObject json = loader->metaData().value(QLatin1String("MetaData")).toObject();

            if (json.isEmpty()) {
                qDebug() << dirIter.filePath() << "has no json!";
            }
            if (!json.isEmpty()) {
                QJsonObject pluginData = json.value(QLatin1String("KPlugin")).toObject();
                if (!pluginData.value(QLatin1String("ServiceTypes")).toArray()
                        .contains(QJsonValue(servicetype)))
                {
                    continue;
                }

                if (!mimetype.isEmpty()) {
                    QStringList mimeTypes = json.value(QLatin1String("X-KDE-ExtraNativeMimeTypes"))
                            .toString().split(QLatin1Char(','));
                    mimeTypes += json.value(QLatin1String("MimeType")).toString().split(QLatin1Char(';'));
                    mimeTypes += json.value(QLatin1String("X-KDE-NativeMimeType")).toString();
                    if (! mimeTypes.contains(mimetype)) {
                        continue;
                    }
                }
                list.append(loader);
            }
        }

    }

    return list;
}
