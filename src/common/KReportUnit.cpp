/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (C) 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
   Copyright (C) 2017 Jarosław Staniek <staniek@kde.org>

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

class Q_DECL_HIDDEN KReportUnit::Private
{
public:
    KReportUnit::Type type;
    qreal pixelConversion;
};

// unit types
// Note: ensure the same order as in KReportUnit::Unit
static QList<KReportUnit::Type> s_unitTypes = {
    KReportUnit::Type::Millimeter,
    KReportUnit::Type::Centimeter,
    KReportUnit::Type::Decimeter,
    KReportUnit::Type::Inch,
    KReportUnit::Type::Pica,
    KReportUnit::Type::Cicero,
    KReportUnit::Type::Point,
    KReportUnit::Type::Pixel
};
static int firstUnitIndex()
{
    return static_cast<int>(KReportUnit::Type::Invalid) + 1;
}

static int lastUnitIndex()
{
    return static_cast<int>(KReportUnit::Type::Last); // without Invalid
}

// unit symbols
// Note: ensure the same order as in KReportUnit::Unit
static const char* const s_unitSymbols[] =
{
    nullptr,
    "mm",
    "cm",
    "dm",
    "in",
    "pi",
    "cc",
    "pt",
    "px"
};

KReportUnit::KReportUnit() : d(new Private)
{
    d->type = Type::Invalid;
    d->pixelConversion = 1.0;
}

KReportUnit::KReportUnit(Type type, qreal factor) : d(new Private)
{
    d->type = type;
    d->pixelConversion = factor;
}

KReportUnit::KReportUnit(const KReportUnit& other) : d(new Private)
{
    d->type = other.type();
    d->pixelConversion = other.factor();
}

KReportUnit::~KReportUnit()
{
    delete d;
}

bool KReportUnit::operator==(const KReportUnit& other) const 
{
    return d->type == other.d->type &&
           (d->type != Type::Pixel ||
            qFuzzyCompare(d->pixelConversion, other.d->pixelConversion));
}

bool KReportUnit::operator!=(const KReportUnit& other) const
{
    return !operator==(other);
}


KReportUnit& KReportUnit::operator=(Type type)
{
    d->type = type;
    d->pixelConversion = 1.0;
    return *this;
}

KReportUnit & KReportUnit::operator=(const KReportUnit& other)
{
    d->type = other.type();
    d->pixelConversion = other.factor();
    return *this;
}

//static
QList<KReportUnit::Type> KReportUnit::allTypes()
{
    return s_unitTypes;
}

//static
QString KReportUnit::description(KReportUnit::Type type)
{
    switch (type) {
    case KReportUnit::Type::Invalid:
        return tr("Invalid");
    case KReportUnit::Type::Millimeter:
        return tr("Millimeters (mm)");
    case KReportUnit::Type::Centimeter:
        return tr("Centimeters (cm)");
    case KReportUnit::Type::Decimeter:
        return tr("Decimeters (dm)");
    case KReportUnit::Type::Inch:
        return tr("Inches (in)");
    case KReportUnit::Type::Pica:
        return tr("Pica (pi)");
    case KReportUnit::Type::Cicero:
        return tr("Cicero (cc)");
    case KReportUnit::Type::Point:
        return tr("Points (pt)");
    case KReportUnit::Type::Pixel:
        return tr("Pixels (px)");
    default:
        return tr("Unsupported unit");
    }
}

QString KReportUnit::description() const
{
    return KReportUnit::description(type());
}

QStringList KReportUnit::descriptions(const QList<Type> &types)
{
    QStringList result;
    for (Type t : types) {
        result.append(description(t));
    }
    return result;
}

qreal KReportUnit::toUserValue(qreal ptValue) const
{
    switch (d->type) {
    case Type::Invalid:
        kreportWarning() << "Conversion for Invalid type not supported";
        return -1.0;
    case Type::Millimeter:
        return toMillimeter(ptValue);
    case Type::Centimeter:
        return toCentimeter(ptValue);
    case Type::Decimeter:
        return toDecimeter(ptValue);
    case Type::Inch:
        return toInch(ptValue);
    case Type::Pica:
        return toPica(ptValue);
    case Type::Cicero:
        return toCicero(ptValue);
    case Type::Pixel:
        return ptValue * d->pixelConversion;
    case Type::Point:
    default:
        return toPoint(ptValue);
    }
}

qreal KReportUnit::ptToUnit(qreal ptValue, const KReportUnit &unit)
{
    switch (unit.d->type) {
    case Type::Invalid:
        return -1.0;
    case Type::Millimeter:
        return POINT_TO_MM(ptValue);
    case Type::Centimeter:
        return POINT_TO_CM(ptValue);
    case Type::Decimeter:
        return POINT_TO_DM(ptValue);
    case Type::Inch:
        return POINT_TO_INCH(ptValue);
    case Type::Pica:
        return POINT_TO_PI(ptValue);
    case Type::Cicero:
        return POINT_TO_CC(ptValue);
    case Type::Pixel:
        return ptValue * unit.d->pixelConversion;
    case Type::Point:
    default:
        return ptValue;
    }
}

QString KReportUnit::toUserStringValue(qreal ptValue) const
{
    return QLocale::system().toString(toUserValue(ptValue));
}

qreal KReportUnit::convertFromPoint(qreal ptValue) const
{
    switch (d->type) {
    case Type::Millimeter:
        return POINT_TO_MM(ptValue);
    case Type::Centimeter:
        return POINT_TO_CM(ptValue);
    case Type::Decimeter:
        return POINT_TO_DM(ptValue);
    case Type::Inch:
        return POINT_TO_INCH(ptValue);
    case Type::Pica:
        return POINT_TO_PI(ptValue);
    case Type::Cicero:
        return POINT_TO_CC(ptValue);
    case Type::Pixel:
        return ptValue * d->pixelConversion;
    case Type::Point:
    default:
        return ptValue;
    }
}

QPointF KReportUnit::convertFromPoint(const QPointF &ptValue) const
{
    return QPointF(convertFromPoint(ptValue.x()), convertFromPoint(ptValue.y()));
}

QSizeF KReportUnit::convertFromPoint(const QSizeF &ptValue) const
{
    return QSizeF(convertFromPoint(ptValue.width()), convertFromPoint(ptValue.height()));
}

qreal KReportUnit::convertToPoint(qreal value) const
{
    switch (d->type) {
    case Type::Invalid:
        return -1.0;
    case Type::Millimeter:
        return MM_TO_POINT(value);
    case Type::Centimeter:
        return CM_TO_POINT(value);
    case Type::Decimeter:
        return DM_TO_POINT(value);
    case Type::Inch:
        return INCH_TO_POINT(value);
    case Type::Pica:
        return PI_TO_POINT(value);
    case Type::Cicero:
        return CC_TO_POINT(value);
    case Type::Pixel:
        return value / d->pixelConversion;
    case Type::Point:
    default:
        return value;
    }
}

qreal KReportUnit::convertToPoint(const QString &value, bool *ok) const
{
    return convertToPoint(QLocale::system().toDouble(value, ok));
}

QPointF KReportUnit::convertToPoint(const QPointF &value) const
{
    return QPointF(KReportUnit::convertToPoint(value.x()),
                   KReportUnit::convertToPoint(value.y()));
}

QSizeF KReportUnit::convertToPoint(const QSizeF &value) const
{
    return QSizeF(KReportUnit::convertToPoint(value.width()),
                  KReportUnit::convertToPoint(value.height()));
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

    bool ok;
    if (firstLetter == -1) {
        qreal result = QVariant(value).toReal(&ok);
        return ok ? result : defaultVal;
    }

    const QByteArray symbol = value.mid(firstLetter).toLatin1();
    value.truncate(firstLetter);
    const qreal val = value.toDouble();

    if (symbol == "pt" || symbol.isEmpty())
        return val;

    KReportUnit u(KReportUnit::symbolToType(QLatin1String(symbol)));
    if (u.isValid()) {
        return u.fromUserValue(val);
    }

    if (symbol == "m")
        return DM_TO_POINT(val * 10.0);
    else if (symbol == "km")
        return DM_TO_POINT(val * 10000.0);
    kreportWarning() << "KReportUnit::parseValue: Unit" << symbol << "is not supported, please report.";

    //! @todo add support for mi/ft ?
    return defaultVal;
}

//static
QString KReportUnit::symbol(KReportUnit::Type type)
{
    return QLatin1String(s_unitSymbols[static_cast<int>(type)]);
}

//static
KReportUnit::Type KReportUnit::symbolToType(const QString &symbol)
{
    Type result = Type::Invalid;

    if (symbol == QLatin1String("inch") /*compat*/) {
        result = Type::Inch;
    } else {
        for (int i = firstUnitIndex(); i <= lastUnitIndex(); ++i) {
            if (symbol == QLatin1String(s_unitSymbols[i])) {
                result = static_cast<Type>(i);
                break;
            }
        }
    }
    return result;
}

//static
QStringList KReportUnit::symbols(const QList<Type> &types)
{
    QStringList result;
    for (Type t : types) {
        result.append(symbol(t));
    }
    return result;
}

qreal KReportUnit::convertFromUnitToUnit(qreal value, const KReportUnit &fromUnit, const KReportUnit &toUnit, qreal factor)
{
    qreal pt;
    switch (fromUnit.type()) {
    case Type::Invalid:
        pt = -1.0;
        break;
    case Type::Millimeter:
        pt = MM_TO_POINT(value);
        break;
    case Type::Centimeter:
         pt = CM_TO_POINT(value);
         break;
    case Type::Decimeter:
         pt = DM_TO_POINT(value);
         break;
    case Type::Inch:
         pt = INCH_TO_POINT(value);
         break;
    case Type::Pica:
         pt = PI_TO_POINT(value);
         break;
    case Type::Cicero:
         pt = CC_TO_POINT(value);
         break;
    case Type::Pixel:
         pt = value / factor;
         break;
    case Type::Point:
    default:
         pt = value;
     }
 
    switch (toUnit.type()) {
    case Type::Millimeter:
         return POINT_TO_MM(pt);
    case Type::Centimeter:
         return POINT_TO_CM(pt);
    case Type::Decimeter:
         return POINT_TO_DM(pt);
    case Type::Inch:
         return POINT_TO_INCH(pt);
    case Type::Pica:
         return POINT_TO_PI(pt);
    case Type::Cicero:
         return POINT_TO_CC(pt);
    case Type::Pixel:
         return pt * factor;
    case Type::Invalid:
    default:
         return pt;
     }
}
 
QPointF KReportUnit::convertFromUnitToUnit(const QPointF &value,
                                           const KReportUnit &fromUnit,
                                           const KReportUnit &toUnit)
{
    return QPointF(
        KReportUnit::convertFromUnitToUnit(value.x(), fromUnit, toUnit),
        KReportUnit::convertFromUnitToUnit(value.y(), fromUnit, toUnit));
}

QSizeF KReportUnit::convertFromUnitToUnit(const QSizeF &value,
                                           const KReportUnit &fromUnit,
                                           const KReportUnit &toUnit)
{
    return QSizeF(
        KReportUnit::convertFromUnitToUnit(value.width(), fromUnit, toUnit),
        KReportUnit::convertFromUnitToUnit(value.height(), fromUnit, toUnit));
}

QString KReportUnit::symbol() const
{
    return QLatin1String(s_unitSymbols[static_cast<int>(d->type)]);
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

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const KReportUnit &unit)
{
#ifndef NDEBUG
    if (unit.isValid()) {
        debug.nospace() << QString::fromLatin1("Unit(%1, %2)").arg(unit.symbol()).arg(unit.factor());
    } else {
        debug.nospace() << QString::fromLatin1("Unit(Invalid)");
    }
#else
    Q_UNUSED(unit);
#endif
    return debug.space();
}

void KReportUnit::setFactor(qreal factor) 
{
    d->pixelConversion = factor;
}

qreal KReportUnit::factor() const
{
    return d->pixelConversion;
}

KReportUnit::Type KReportUnit::type() const 
{
    return d->type;
}

bool KReportUnit::isValid() const
{
    return d->type != KReportUnit::Type::Invalid;
}

#endif
