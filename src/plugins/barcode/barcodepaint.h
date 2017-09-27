/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
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

#ifndef BARCODEPAINT_H
#define BARCODEPAINT_H

#include <QRect>
#include <QString>

//
// i2of5
//
void renderI2of5(const QRectF &, const QString &, Qt::Alignment align, QPainter *);

//
// 3of9
//
void render3of9(const QRect &, const QString &, Qt::Alignment align, QPainter *);

//
// 3of9+
//
void renderExtended3of9(const QRect &, const QString &, Qt::Alignment align, QPainter *);

//
// Code 128
//
void renderCode128(const QRect &, const QString &, Qt::Alignment align, QPainter *);

//
// Code EAN/UPC
//
void renderCodeEAN13(const QRect &, const QString &, Qt::Alignment align, QPainter *);
void renderCodeEAN8(const QRect &, const QString &, Qt::Alignment align, QPainter *);
void renderCodeUPCA(const QRect &, const QString &, Qt::Alignment align, QPainter *);
void renderCodeUPCE(const QRect &, const QString &, Qt::Alignment align, QPainter *);

#endif

