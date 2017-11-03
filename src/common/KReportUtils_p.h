/* This file is part of the KDE project
   Copyright (C) 2015-2016 Jarosław Staniek <staniek@kde.org>

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

#ifndef KREPORTUTILS_P_H
#define KREPORTUTILS_P_H

#include "KReportUnit.h"

#include <QPageLayout>
#include <QRect>
#include <QStandardPaths>

const bool DEFAULT_SHOW_GRID = true;
const bool DEFAULT_SNAP_TO_GRID = true;
const int DEFAULT_GRID_DIVISIONS = 4;
const KReportUnit::Type DEFAULT_UNIT_TYPE = KReportUnit::Type::Centimeter;
#define DEFAULT_UNIT KReportUnit(DEFAULT_UNIT_TYPE)
#define DEFAULT_UNIT_STRING QLatin1String("cm")
const QPointF DEFAULT_ELEMENT_POS_PT(CM_TO_POINT(1.0), CM_TO_POINT(1.0));
const QSizeF DEFAULT_ELEMENT_SIZE_PT(CM_TO_POINT(1.0), CM_TO_POINT(1.0));
const QRectF DEFAULT_ELEMENT_RECT_PT(DEFAULT_ELEMENT_POS_PT, DEFAULT_ELEMENT_SIZE_PT);
#define DEFAULT_ELEMENT_POS_STRING QLatin1String("1.0cm") // both X and Y
#define DEFAULT_ELEMENT_SIZE_STRING QLatin1String("1.0cm") // both width and height
const qreal DEFAULT_SECTION_SIZE_PT = CM_TO_POINT(2.0);
#define DEFAULT_SECTION_SIZE_STRING QLatin1String("2.0cm")
const qreal DEFAULT_PAGE_MARGIN_PT = CM_TO_POINT(1.0);
#define DEFAULT_PAGE_MARGIN_STRING QLatin1String("1.0cm")
const QPageSize::PageSizeId DEFAULT_PAGE_SIZE = QPageSize::A4;
const QPageLayout::Orientation DEFAULT_PAGE_ORIENTATION = QPageLayout::Landscape;
const QSizeF DEFAULT_CUSTOM_PAGE_SIZE(10.0,10.0);
const qreal SMALLEST_PAGE_SIZE_PT = MM_TO_POINT(5);

// This is a private code made inline for use in the lib and examples.
//! @todo Move to a shared lib to use in other Kexi libraries as well.

namespace KReportPrivate {

//! @todo Support other themes
const QString supportedIconTheme = QLatin1String("breeze");

//! @brief @return true if @a path is readable
bool fileReadable(const QString &path);

//! @brief Used for a workaround: locations for QStandardPaths::AppDataLocation end with app name.
//! If this is not an expected app but for example a test app, replace
//! the subdir name with app name so we can find resource file(s).
QStringList correctStandardLocations(const QString &privateName,
                                     QStandardPaths::StandardLocation location,
                                     const QString &extraLocation);



/*! @brief Locates a file path for specified parameters
 * @param privateName Name to be used instead of application name for resource lookup
 * @param path Relative path to the resource file
 * @param location Standard file location to use for file lookup
 * @param extraLocation Extra directory path for file lookup
 * @return Empty string on failure
 */
QString locateFile(const QString &privateName,
                   const QString& path, QStandardPaths::StandardLocation location,
                   const QString &extraLocation);

/*! @brief Registers icons resource file
 * @param privateName Name to be used instead of application name for resource lookup
 * @param path Relative path to the resource file
 * @param location Standard file location to use for file lookup
 * @param resourceRoot A resource root for QResource::registerResource()
 * @param errorMessage On failure it is set to a brief error message.
 * @param errorDescription On failure it is set to a detailed error message.
 * other for warning
 */
bool registerIconsResource(const QString &privateName, const QString& path,
                             QStandardPaths::StandardLocation location,
                             const QString &resourceRoot, const QString &extraLocation,
                             QString *errorMessage, QString *detailedErrorMessage);

/*! @brief Registers a global icon resource file
 * @param themeName A name of icon theme to use.
 * @param errorMessage On failure it is set to a brief error message.
 * @param errorDescription On failure it is set to a detailed error message.
 * other for warning
 */
bool registerGlobalIconsResource(const QString &themeName,
                                 QString *errorMessage,
                                 QString *detailedErrorMessage);

/*! @brief Registers a global icon resource file
 * @param themeName A name of icon theme to use.
 */
bool registerGlobalIconsResource(const QString &themeName);

/*! @brief Registers a global icon resource file for default theme name.
 */
bool registerGlobalIconsResource();

/*! @brief Sets up a private icon resource file
 * @return @c false on failure and sets error message. Does not warn or exit on failure.
 * @param privateName Name to be used instead of application name for resource lookup
 * @param path Relative path to the resource file
 * @param themeName Icon theme to use. It affects filename.
 * @param errorMessage On failure it is set to a brief error message
 * @param errorDescription On failure it is set to a detailed error message
 * other for warning
 * @param prefix Resource path prefix. The default is useful for library-global resource,
 * other values is useful for plugins.
 */
bool setupPrivateIconsResource(const QString &privateName, const QString& path,
                               const QString &themeName,
                               QString *errorMessage, QString *detailedErrorMessage,
                               const QString &prefix = QLatin1String(":/icons"));

/*! @brief Sets up a private icon resource file
 * @return @c false on failure and sets error message.
 * @param privateName Name to be used instead of application name for resource lookup
 * @param path Relative path to the resource file
 * @param themeName Icon theme to use. It affects filename.
 * @param errorMessage On failure it is set to a brief error message.
 * @param errorDescription On failure it is set to a detailed error message.
 * other for warning
 * @param prefix Resource path prefix. The default is useful for library-global resource,
 * other values is useful for plugins.
 */
bool setupPrivateIconsResourceWithMessage(const QString &privateName, const QString& path,
                                          const QString &themeName,
                                          QString *errorMessage, QString *detailedErrorMessage,
                                          const QString &prefix = QLatin1String(":/icons"));

/*! @overload setupPrivateIconsResourceWithMessage(QString &privateName, const QString& path,
                                          const QString &themeName,
                                          QString *errorMessage, QString *detailedErrorMessage,
                                          const QString &prefix = QLatin1String(":/icons"))
    Uses default theme name.
 */
bool setupPrivateIconsResourceWithMessage(const QString &privateName, const QString& path,
                                          QString *errorMessage, QString *detailedErrorMessage,
                                          const QString &prefix = QLatin1String(":/icons"));

/*! @brief Sets up a private icon resource file
 * Warns on failure and returns @c false.
 * @param privateName Name to be used instead of application name for resource lookup
 * @param path Relative path to the resource file
 * @param messageType Type of message to use on error, QtFatalMsg for fatal exit and any
 * other for warning
 * @param prefix Resource path prefix. The default is useful for library-global resource,
 * other values is useful for plugins.
 */
bool setupPrivateIconsResourceWithMessage(const QString &privateName, const QString& path,
                                          QtMsgType messageType,
                                          const QString &prefix = QLatin1String(":/icons"));

//! Sets up a global icon theme if it is different from supported.
//! Warns on failure and returns @c false.
bool setupGlobalIconTheme();

int dpiX();

int dpiY();

//! This class is wrapper that fixes a critical QTBUG-47551 bug in default constructor of QPageLayout
//! Default constructor of QPageLayout does not initialize units.
//! https://bugreports.qt.io/browse/QTBUG-47551
//! @todo remove this class and go back to QPageLayout when the faulty QPageLayout implementations are no longer on the wild. That's probably for Qt 6.
class PageLayout: public QPageLayout {
public:
        PageLayout();
        PageLayout(const QPageLayout& pageLayout);
        PageLayout& operator=(const QPageLayout& other);
};

} // KReportPrivate

#endif
