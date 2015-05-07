/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
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

/*
 *     This file contains the code that will render the extended 3of9 barcode.
 * This code will parse a string and build a compliant 3of9 string and then
 * call the 3of9 renderer to do the actual drawing.
 */

#include <QString>
#include <QRect>
#include <QPainter>

#include "barcodepaint.h"

QString convertTo3of9(const QString &str);

void renderExtended3of9(const QRect & r, const QString & str, int align, QPainter * pPainter)
{
    render3of9(r, convertTo3of9(str), align, pPainter);
}
