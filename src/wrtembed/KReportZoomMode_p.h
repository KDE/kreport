/* This file is part of the KDE project
   Copyright (C) 2005 Johannes Schaub <litb_devel@web.de>
   Copyright (C) 2011 Arjen Hiemstra <ahiemstra@heimr.nl>

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

#ifndef _KREPORTZOOMMODE_H_
#define _KREPORTZOOMMODE_H_

#include <QCoreApplication>

#include "kreport_export.h"

/**
 * The ZoomMode container
 */
class KReportZoomMode
{
    Q_DECLARE_TR_FUNCTIONS(KReportZoomMode)
public:
    enum class Type
    {
        Constant, ///< zoom x %
        Width,    ///< zoom pagewidth
        Page,     ///< zoom to pagesize
        Pixels,   ///< zoom to actual pixels
        Text      ///< zoom to text
    };

    /// \param type name
    /// \return type converted
    static Type toType(const QString &string);

    /// \return QString converted and translated for type
    static QString toString(Type type);

    /// \param type name
    /// \return true if \c mode isn't dependent on windowsize
    static bool isConstant(const QString& mode)
    { return toType(mode) == Type::Constant; }

    /**
     * Return the minimum zoom possible for documents.
     *
     * \return The minimum zoom possible.
     */
    static qreal minimumZoom();
    /**
     * Return the maximum zoom possible for documents.
     *
     * \return The maximum zoom possible.
     */
    static qreal maximumZoom();
    /**
     * Clamp the zoom value so that mimimumZoom <= zoom <= maximumZoom.
     *
     * \param zoom The value to clamp.
     *
     * \return minimumZoom if zoom < minimumZoom, maximumZoom if zoom >
     * maximumZoom, zoom if otherwise.
     */
    static qreal clampZoom(qreal zoom);

    /**
     * Set the minimum zoom possible for documents.
     *
     * Note that after calling this, any existing KoZoomAction instances
     * should be recreated.
     *
     * \param zoom The minimum zoom to use.
     */
    static void setMinimumZoom(qreal zoom);
    /**
     * Set the maximum zoom possible for documents.
     *
     * Note that after calling this, any existing KoZoomAction instances
     * should be recreated.
     *
     * \param zoom The maximum zoom to use.
     */
    static void setMaximumZoom(qreal zoom);
};

#endif
