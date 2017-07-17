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

#include "KReportZoomMode_p.h"

#include <QCoreApplication>

namespace{
static const char* const s_types[] =
{
    QT_TRANSLATE_NOOP("KReportZoomMode", "%1%"),
    QT_TRANSLATE_NOOP("KReportZoomMode", "Fit Page Width"),
    QT_TRANSLATE_NOOP("KReportZoomMode", "Fit Page"),
    nullptr,
    QT_TRANSLATE_NOOP("KReportZoomMode", "Actual Pixels"),
    nullptr,
    nullptr,
    nullptr,
    QT_TRANSLATE_NOOP("KReportZoomMode", "Fit Text Width")
};

qreal s_minimumZoomValue = 0.2;
qreal s_maximumZoomValue = 5.0;
}

QString KReportZoomMode::toString(Type type)
{
    return tr(s_types[static_cast<int>(type)]);
}

KReportZoomMode::Type KReportZoomMode::toType(const QString& string)
{
    if(string == toString(Type::Width)) {
        return Type::Width;
    } else if (string == toString(Type::Page)) {
        return Type::Page;
    } else if (string == toString(Type::Pixels)) {
        return Type::Pixels;
    } else if (string == toString(Type::Text)) {
        return Type::Text;
    } else {
        // we return Constant else because then we can pass '10%' or '15%'
        // or whatever, it's automatically converted. Constant is
        // changeable, whereas all other zoom modes (non-constants) are normal
        // text like "Fit to xxx". they let the view grow/shrink according
        // to windowsize, hence the term 'non-constant'
        return Type::Constant;
    }
}

qreal KReportZoomMode::minimumZoom()
{
    return s_minimumZoomValue;
}

qreal KReportZoomMode::maximumZoom()
{
    return s_maximumZoomValue;
}

qreal KReportZoomMode::clampZoom(qreal zoom)
{
    return qMin(s_maximumZoomValue, qMax(s_minimumZoomValue, zoom));
}

void KReportZoomMode::setMinimumZoom(qreal zoom)
{
    Q_ASSERT(zoom > 0.0f);
    s_minimumZoomValue = zoom;
}

void KReportZoomMode::setMaximumZoom(qreal zoom)
{
    Q_ASSERT(zoom > 0.0f);
    s_maximumZoomValue = zoom;
}
