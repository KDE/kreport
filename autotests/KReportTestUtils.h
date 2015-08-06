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

#ifndef KREPORT_TEST_UTILS_H
#define KREPORT_TEST_UTILS_H

#include <QTest>
#include <QRect>

#include "kreporttestutils_export.h"

namespace QTest
{
//! For convenience.
KREPORTTESTUTILS_EXPORT bool qCompare(qreal val1, qreal val2, qreal epsilon, const char *actual,
                                      const char *expected, const char *file, int line);

//! For convenience.
KREPORTTESTUTILS_EXPORT bool qCompare(const QString &val1, const char* val2, const char *actual,
                                      const char *expected, const char *file, int line);
}

#define QFUZZYCOMPARE(actual, expected, epsilon) \
    do {\
        if (!QTest::qCompare(actual, expected, epsilon, #actual, #expected, __FILE__, __LINE__))\
            return;\
    } while (0)

#endif // KREPORT_TEST_UTILS_H
