/* This file is part of the KDE project
   Copyright (C) 2015-2016 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2016 Adam Pigg <adam@piggz.co.uk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KReportUtils_p.h"
#include "config-kreport.h"

#ifdef Q_OS_WIN
#define KPATH_SEPARATOR ';'
#else
#define KPATH_SEPARATOR ':'
#endif

class KReportDpiSingleton
{
public:
    KReportDpiSingleton();

    int m_dpiX;
    int m_dpiY;
};

KReportDpiSingleton::KReportDpiSingleton()
{
    // Another way to get the DPI of the display would be QPaintDeviceMetrics,
    // but we have no widget here (and moving this to KoView wouldn't allow
    // using this from the document easily).
#ifdef Q_WS_X11
    m_dpiX = QX11Info::appDpiX();
    m_dpiY = QX11Info::appDpiY();
#else
    QDesktopWidget *w = QApplication::desktop();
    if (w) {
        m_dpiX = w->logicalDpiX();
        m_dpiY = w->logicalDpiY();
    } else {
        m_dpiX = 96;
        m_dpiY = 96;
    }
#endif
}

Q_GLOBAL_STATIC(KReportDpiSingleton, s_instance)



namespace KReportPrivate
{

bool fileReadable(const QString &path)
{
    return !path.isEmpty() && QFileInfo(path).isReadable();
}

QStringList correctStandardLocations(const QString &privateName,
                                     QStandardPaths::StandardLocation location,
                                     const QString &extraLocation)
 {
    QStringList result;
    if (!privateName.isEmpty()) {
        QRegularExpression re(QLatin1Char('/') + QCoreApplication::applicationName() + QLatin1Char('$'));
        QStringList standardLocations(QStandardPaths::standardLocations(location));
        if (!extraLocation.isEmpty()) {
            standardLocations.append(extraLocation);
        }
        for(const QString &dir : standardLocations) {
            if (dir.indexOf(re) != -1) {
                QString realDir(dir);
                realDir.replace(re, QLatin1Char('/') + privateName);
                result.append(realDir);
            }
        }
    }
    return result;
}

QString locateFile(const QString &privateName,
                   const QString& path, QStandardPaths::StandardLocation location,
                   const QString &extraLocation)
{
    // let QStandardPaths handle this, it will look for app local stuff
    QString fullPath = QFileInfo(
        QStandardPaths::locate(location, path)).canonicalFilePath();
    if (fileReadable(fullPath)) {
        return fullPath;
    }

    // Try extra location
    fullPath = QFileInfo(extraLocation + QLatin1Char('/') + path).canonicalFilePath();
    if (fileReadable(fullPath)) {
        return fullPath;
    }
    // Try in PATH subdirs, useful for running apps from the build dir, without installing
    for(const QByteArray &pathDir : qgetenv("PATH").split(KPATH_SEPARATOR)) {
        const QString dataDirFromPath = QFileInfo(QFile::decodeName(pathDir) + QStringLiteral("/data/")
                                                  + path).canonicalFilePath();
        if (fileReadable(dataDirFromPath)) {
            return dataDirFromPath;
        }
    }

    const QStringList correctedStandardLocations(correctStandardLocations(privateName, location, extraLocation));
    for(const QString &dir : correctedStandardLocations) {
        fullPath = QFileInfo(dir + QLatin1Char('/') + path).canonicalFilePath();
        if (fileReadable(fullPath)) {
            return fullPath;
        }
    }
    return QString();
}

bool registerIconsResource(const QString &privateName, const QString& path,
                             QStandardPaths::StandardLocation location,
                             const QString &resourceRoot, const QString &extraLocation,
                             QString *errorMessage, QString *detailedErrorMessage)
{
    const QString fullPath = locateFile(privateName, path, location, extraLocation);
    if (fullPath.isEmpty() || !QFileInfo(fullPath).isReadable()
        || !QResource::registerResource(fullPath, resourceRoot))
    {
        QStringList triedLocations(QStandardPaths::standardLocations(location));
        if (!extraLocation.isEmpty()) {
            triedLocations.append(extraLocation);
        }
        triedLocations.append(correctStandardLocations(privateName, location, extraLocation));
        const QString triedLocationsString = QLocale().createSeparatedList(triedLocations);
#ifdef QT_ONLY
        *errorMessage = QString("Could not open icon resource file %1.").arg(path);
        *detailedErrorMessage = QString("Tried to find in %1.").arg(triedLocationsString);
#else
        //! @todo 3.1 Re-add translation
        *errorMessage = /*QObject::tr*/ QString::fromLatin1(
            "Could not open icon resource file \"%1\". "
            "Application will not start. "
            "Please check if it is properly installed.")
            .arg(QFileInfo(path).fileName());
        //! @todo 3.1 Re-add translation
        *detailedErrorMessage = QString::fromLatin1("Tried to find in %1.").arg(triedLocationsString);
#endif
        return false;
    }
    *errorMessage = QString();
    *detailedErrorMessage = QString();
    return true;
}

bool registerGlobalIconsResource(const QString &themeName,
                                 QString *errorMessage,
                                 QString *detailedErrorMessage)
{
    QString extraLocation;
#ifdef CMAKE_INSTALL_FULL_ICONDIR
    extraLocation = QDir::fromNativeSeparators(QFile::decodeName(CMAKE_INSTALL_FULL_ICONDIR));
    if (extraLocation.endsWith("/icons")) {
        extraLocation.chop(QLatin1String("/icons").size());
    }
#elif defined(Q_OS_WIN)
    extraLocation = QCoreApplication::applicationDirPath() + QStringLiteral("/data");
#endif
    return registerIconsResource(QString(), QString::fromLatin1("icons/%1/%1-icons.rcc").arg(themeName),
                            QStandardPaths::GenericDataLocation,
                            QStringLiteral("/icons/") + themeName,
                            extraLocation, errorMessage,
                            detailedErrorMessage);
}

bool registerGlobalIconsResource(const QString &themeName)
{
    QString errorMessage;
    QString detailedErrorMessage;
    if (!registerGlobalIconsResource(themeName, &errorMessage, &detailedErrorMessage)) {
        if (detailedErrorMessage.isEmpty()) {
            KMessageBox::error(nullptr, errorMessage);
        } else {
            KMessageBox::detailedError(nullptr, errorMessage, detailedErrorMessage);
        }
        qWarning() << qPrintable(errorMessage);
        return false;
    }
    return true;
}

bool registerGlobalIconsResource()
{
    return registerGlobalIconsResource(supportedIconTheme);
}

bool setupPrivateIconsResource(const QString &privateName, const QString& path,
                               const QString &themeName,
                               QString *errorMessage, QString *detailedErrorMessage,
                               const QString &prefix)
{
    // Register application's resource first to have priority over the theme.
    // Some icons may exists in both resources.
    if (!registerIconsResource(privateName, path,
                          QStandardPaths::AppDataLocation,
                          QString(), QString(), errorMessage, detailedErrorMessage))
    {
        return false;
    }
    bool changeTheme = false;
#ifdef QT_GUI_LIB
    QIcon::setThemeSearchPaths(QStringList() << prefix << QIcon::themeSearchPaths());
    changeTheme = 0 != QIcon::themeName().compare(themeName, Qt::CaseInsensitive);
    if (changeTheme) {
        QIcon::setThemeName(themeName);
    }
#endif

    KConfigGroup cg(KSharedConfig::openConfig(), "Icons");
    changeTheme = changeTheme || 0 != cg.readEntry("Theme", QString()).compare(themeName, Qt::CaseInsensitive);
    // tell KIconLoader an co. about the theme
    if (changeTheme) {
        cg.writeEntry("Theme", themeName);
        cg.sync();
    }
    return true;
}

bool setupPrivateIconsResourceWithMessage(const QString &privateName, const QString& path,
                                          const QString &themeName,
                                          QString *errorMessage, QString *detailedErrorMessage,
                                          const QString &prefix)
{
    if (!setupPrivateIconsResource(privateName, path, themeName,
                                   errorMessage, detailedErrorMessage, prefix))
    {
        if (detailedErrorMessage->isEmpty()) {
            KMessageBox::error(nullptr, *errorMessage);
        } else {
            KMessageBox::detailedError(nullptr, *errorMessage, *detailedErrorMessage);
        }
        return false;
    }
    return true;
}

bool setupPrivateIconsResourceWithMessage(const QString &privateName, const QString& path,
                                          QString *errorMessage, QString *detailedErrorMessage,
                                          const QString &prefix)
{
    return setupPrivateIconsResourceWithMessage(privateName, path, supportedIconTheme,
                                                errorMessage, detailedErrorMessage, prefix);
}

bool setupPrivateIconsResourceWithMessage(const QString &privateName, const QString& path,
                                          QtMsgType messageType,
                                          const QString &prefix)
{
    QString errorMessage;
    QString detailedErrorMessage;
    if (!setupPrivateIconsResourceWithMessage(privateName, path,
                                              &errorMessage, &detailedErrorMessage, prefix)) {
        if (messageType == QtFatalMsg) {
            qFatal("%s %s", qPrintable(errorMessage), qPrintable(detailedErrorMessage));
        } else {
            qWarning() << qPrintable(errorMessage) << qPrintable(detailedErrorMessage);
        }
        return false;
    }
    return true;
}

bool setupGlobalIconTheme()
{
    if (0 != QIcon::themeName().compare(supportedIconTheme, Qt::CaseInsensitive)) {
        const QString message = QString::fromLatin1(
            "\"%1\"  supports only \"%2\" icon theme but current system theme is \"%3\". "
            "Application's icon theme will be changed to \"%2\". "
            "Please consider adding support for other themes to %4.")
            .arg(QLatin1String(KREPORT_BASE_NAME)).arg(supportedIconTheme).arg(QIcon::themeName())
            .arg(QCoreApplication::applicationName());
        qDebug() << qPrintable(message);
        if (!registerGlobalIconsResource()) {
            // don't fail, just warn
            const QString message = QString::fromLatin1(
                "Failed to set icon theme to \"%1\". Icons in the application will be inconsistent. "
                "Please install .rcc file(s) for the system theme.")
                .arg(supportedIconTheme);
            qDebug() << qPrintable(message);
            return false;
        }
    }
    return true;
}

int dpiX()
{
    return s_instance->m_dpiX;
}

int dpiY()
{
    return s_instance->m_dpiY;
}

PageLayout::PageLayout() : QPageLayout(QPageSize(QPageSize::A4), QPageLayout::Portrait, QMarginsF(0,0,0,0))
{
}

PageLayout::PageLayout(const QPageLayout& pageLayout) : QPageLayout(pageLayout)
{
}

KReportPrivate::PageLayout & PageLayout::operator=(const QPageLayout& other)
{
    QPageLayout::operator=(other);
    return *this;
}

}
