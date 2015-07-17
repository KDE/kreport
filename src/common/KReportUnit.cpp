/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>
   Copyright (C) 2004, Nicolas GOUTTE <goutte@kde.org>
   Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "KReportUnit.h"
#include "kreport_debug.h"

#include <cmath>

#include <QTransform>
#include <QCoreApplication>

// ensure the same order as in KReportUnit::Unit
static const char* const unitNameList[KReportUnit::TypeCount] =
{
    "mm",
    "pt",
    "in",
    "cm",
    "dm",
    "pi",
    "cc",
    "px"
};

QString KReportUnit::unitDescription(KReportUnit::Type type)
{
    switch (type) {
    case KReportUnit::Millimeter:
        return QCoreApplication::translate("KReportUnit", "Millimeters (mm)");
    case KReportUnit::Centimeter:
        return QCoreApplication::translate("KReportUnit", "Centimeters (cm)");
    case KReportUnit::Decimeter:
        return QCoreApplication::translate("KReportUnit", "Decimeters (dm)");
    case KReportUnit::Inch:
        return QCoreApplication::translate("KReportUnit", "Inches (in)");
    case KReportUnit::Pica:
        return QCoreApplication::translate("KReportUnit", "Pica (pi)");
    case KReportUnit::Cicero:
        return QCoreApplication::translate("KReportUnit", "Cicero (cc)");
    case KReportUnit::Point:
        return QCoreApplication::translate("KReportUnit", "Points (pt)");
    case KReportUnit::Pixel:
        return QCoreApplication::translate("KReportUnit", "Pixels (px)");
    default:
        return QCoreApplication::translate("KReportUnit", "Unsupported unit");
    }
}

// grouped by units which are similar
static const KReportUnit::Type typesInUi[KReportUnit::TypeCount] =
{
    KReportUnit::Millimeter,
    KReportUnit::Centimeter,
    KReportUnit::Decimeter,
    KReportUnit::Inch,
    KReportUnit::Pica,
    KReportUnit::Cicero,
    KReportUnit::Point,
    KReportUnit::Pixel,
};

QStringList KReportUnit::listOfUnitNameForUi(ListOptions listOptions)
{
    QStringList lst;
    for (int i = 0; i < KReportUnit::TypeCount; ++i) {
        const Type type = typesInUi[i];
        if ((type != Pixel) || ((listOptions & HideMask) == ListAll))
            lst.append(unitDescription(type));
    }
    return lst;
}

KReportUnit KReportUnit::fromListForUi(int index, ListOptions listOptions, qreal factor)
{
    KReportUnit::Type type = KReportUnit::Point;

    if ((0 <= index) && (index < KReportUnit::TypeCount)) {
        // iterate through all enums and skip the Pixel enum if needed
        for (int i = 0; i < KReportUnit::TypeCount; ++i) {
            if ((listOptions&HidePixel) && (typesInUi[i] == Pixel)) {
                ++index;
                continue;
            }
            if (i == index) {
                type = typesInUi[i];
                break;
            }
        }
    }

    return KReportUnit(type, factor);
}

int KReportUnit::indexInListForUi(ListOptions listOptions) const
{
    if ((listOptions&HidePixel) && (m_type == Pixel)) {
        return -1;
    }

    int result = -1;

    int skipped = 0;
    for (int i = 0; i < KReportUnit::TypeCount; ++i) {
        if ((listOptions&HidePixel) && (typesInUi[i] == Pixel)) {
            ++skipped;
            continue;
        }
        if (typesInUi[i] == m_type) {
            result = i - skipped;
            break;
        }
    }

    return result;
}

qreal KReportUnit::toUserValue(qreal ptValue) const
{
    switch (m_type) {
    case Millimeter:
        return toMillimeter(ptValue);
    case Centimeter:
        return toCentimeter(ptValue);
    case Decimeter:
        return toDecimeter(ptValue);
    case Inch:
        return toInch(ptValue);
    case Pica:
        return toPica(ptValue);
    case Cicero:
        return toCicero(ptValue);
    case Pixel:
        return ptValue * m_pixelConversion;
    case Point:
    default:
        return toPoint(ptValue);
    }
}

qreal KReportUnit::ptToUnit(qreal ptValue, const KReportUnit &unit)
{
    switch (unit.m_type) {
    case Millimeter:
        return POINT_TO_MM(ptValue);
    case Centimeter:
        return POINT_TO_CM(ptValue);
    case Decimeter:
        return POINT_TO_DM(ptValue);
    case Inch:
        return POINT_TO_INCH(ptValue);
    case Pica:
        return POINT_TO_PI(ptValue);
    case Cicero:
        return POINT_TO_CC(ptValue);
    case Pixel:
        return ptValue * unit.m_pixelConversion;
    case Point:
    default:
        return ptValue;
    }
}

QString KReportUnit::toUserStringValue(qreal ptValue) const
{
    return QLocale::system().toString(toUserValue(ptValue));
}

qreal KReportUnit::fromUserValue(qreal value) const
{
    switch (m_type) {
    case Millimeter:
        return MM_TO_POINT(value);
    case Centimeter:
        return CM_TO_POINT(value);
    case Decimeter:
        return DM_TO_POINT(value);
    case Inch:
        return INCH_TO_POINT(value);
    case Pica:
        return PI_TO_POINT(value);
    case Cicero:
        return CC_TO_POINT(value);
    case Pixel:
        return value / m_pixelConversion;
    case Point:
    default:
        return value;
    }
}

qreal KReportUnit::fromUserValue(const QString &value, bool *ok) const
{
    return fromUserValue(QLocale::system().toDouble(value, ok));
}

qreal KReportUnit::parseValue(const QString& _value, qreal defaultVal)
{
    if (_value.isEmpty())
        return defaultVal;

    QString value(_value.simplified());
    value.remove(QLatin1Char(' '));

    int firstLetter = -1;
    for (int i = 0; i < value.length(); ++i) {
        if (value.at(i).isLetter()) {
            if (value.at(i) == QLatin1Char('e'))
                continue;
            firstLetter = i;
            break;
        }
    }

    if (firstLetter == -1)
        return value.toDouble();

    const QString symbol = value.mid(firstLetter);
    value.truncate(firstLetter);
    const qreal val = value.toDouble();

    if (symbol == QLatin1String("pt"))
        return val;

    bool ok;
    KReportUnit u = KReportUnit::fromSymbol(symbol, &ok);
    if (ok)
        return u.fromUserValue(val);

    if (symbol == QLatin1String("m"))
        return DM_TO_POINT(val * 10.0);
    else if (symbol == QLatin1String("km"))
        return DM_TO_POINT(val * 10000.0);
    kreportWarning() << "KReportUnit::parseValue: Unit" << symbol << "is not supported, please report.";

    //! @todo add support for mi/ft ?
    return defaultVal;
}

KReportUnit KReportUnit::fromSymbol(const QString &symbol, bool *ok)
{
    Type result = Point;

    if (symbol == QLatin1String("inch") /*compat*/) {
        result = Inch;
        if (ok)
            *ok = true;
    } else {
        if (ok)
            *ok = false;

        for (int i = 0; i < TypeCount; ++i) {
            if (symbol == QLatin1String(unitNameList[i])) {
                result = static_cast<Type>(i);
                if (ok)
                    *ok = true;
            }
        }
    }

    return KReportUnit(result);
}

qreal KReportUnit::convertFromUnitToUnit(qreal value, const KReportUnit &fromUnit, const KReportUnit &toUnit, qreal factor)
{
    qreal pt;
    switch (fromUnit.type()) {
    case Millimeter:
        pt = MM_TO_POINT(value);
        break;
    case Centimeter:
        pt = CM_TO_POINT(value);
        break;
    case Decimeter:
        pt = DM_TO_POINT(value);
        break;
    case Inch:
        pt = INCH_TO_POINT(value);
        break;
    case Pica:
        pt = PI_TO_POINT(value);
        break;
    case Cicero:
        pt = CC_TO_POINT(value);
        break;
    case Pixel:
        pt = value / factor;
        break;
    case Point:
    default:
        pt = value;
    }

    switch (toUnit.type()) {
    case Millimeter:
        return POINT_TO_MM(pt);
    case Centimeter:
        return POINT_TO_CM(pt);
    case Decimeter:
        return POINT_TO_DM(pt);
    case Inch:
        return POINT_TO_INCH(pt);
    case Pica:
        return POINT_TO_PI(pt);
    case Cicero:
        return POINT_TO_CC(pt);
    case Pixel:
        return pt * factor;
    case Point:
    default:
        return pt;
    }

}

QString KReportUnit::symbol() const
{
    return QLatin1String(unitNameList[m_type]);
}

qreal KReportUnit::parseAngle(const QString& _value, qreal defaultVal)
{
    if (_value.isEmpty())
        return defaultVal;

    QString value(_value.simplified());
    value.remove(QLatin1Char(' '));

    int firstLetter = -1;
    for (int i = 0; i < value.length(); ++i) {
        if (value.at(i).isLetter()) {
            if (value.at(i) == QLatin1Char('e'))
                continue;
            firstLetter = i;
            break;
        }
    }

    if (firstLetter == -1)
        return value.toDouble();

    const QString type = value.mid(firstLetter);
    value.truncate(firstLetter);
    const qreal val = value.toDouble();

    if (type == QLatin1String("deg"))
        return val;
    else if (type == QLatin1String("rad"))
        return val * 180 / M_PI;
    else if (type == QLatin1String("grad"))
        return val * 0.9;

    return defaultVal;
}

qreal KReportUnit::approxTransformScale(const QTransform &t)
{
    return std::sqrt(t.determinant());
}

void KReportUnit::adjustByPixelTransform(const QTransform &t)
{
    m_pixelConversion *= approxTransformScale(t);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const KReportUnit &unit)
{
#ifndef NDEBUG
    debug.nospace() << unit.symbol();
#else
    Q_UNUSED(unit);
#endif
    return debug.space();

}
#endif
