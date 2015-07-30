/* This file is part of the KDE project
 * Copyright (C) 2012 Dag Andersen <danders@get2net.dk>
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

#ifndef FORMATTEST_H
#define FORMATTEST_H

#include <QObject>

//! Tests for the .kreport format
class FormatTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testPageOptions();
    void testLineItem();
    //! Use a label to test basic rect properties
    void testRectItem();
};

#endif // FORMATTEST_H
