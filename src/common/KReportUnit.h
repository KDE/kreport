/* This file is part of the KDE project
   Copyright (C) 1998 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 1998 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (C) 2010 Thomas Zander <zander@kde.org>
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

#ifndef KREPORTUNIT_H
#define KREPORTUNIT_H

#include <math.h> // for floor

#include <QCoreApplication>
#include <QString>
#include <QDebug>
#include <QMetaType>

#include "kreport_export.h"

// 1 inch ^= 72 pt
// 1 inch ^= 25.399956 mm (-pedantic ;p)
// 1 pt = 1/12 pi
// 1 pt ^= 0.0077880997 cc
// 1 cc = 12 dd
// Note: I don't use division but multiplication with the inverse value
// because it's faster ;p (Werner)
#define POINT_TO_MM(px) qreal((px)*0.352777167)
#define MM_TO_POINT(mm) qreal((mm)*2.83465058)
#define POINT_TO_CM(px) qreal((px)*0.0352777167)
#define CM_TO_POINT(cm) qreal((cm)*28.3465058)
#define POINT_TO_DM(px) qreal((px)*0.00352777167)
#define DM_TO_POINT(dm) qreal((dm)*283.465058)
#define POINT_TO_INCH(px) qreal((px)*0.01388888888889)
#define INCH_TO_POINT(inch) qreal((inch)*72.0)
#define MM_TO_INCH(mm) qreal((mm)*0.039370147)
#define INCH_TO_MM(inch) qreal((inch)*25.399956)
#define POINT_TO_PI(px) qreal((px)*0.083333333)
#define POINT_TO_CC(px) qreal((px)*0.077880997)
#define PI_TO_POINT(pi) qreal((pi)*12)
#define CC_TO_POINT(cc) qreal((cc)*12.840103)

/*!
 * @brief Converts between different units
 *
 * KReportUnit stores everything in pt (using "qreal") internally.
 * When displaying a value to the user, the value is converted to the user's unit
 * of choice, and rounded to a reasonable precision to avoid 0.999999
 *
 * For implementing the selection of a unit type in the UI use the allTypes() method.
 * it ensure the same order of the unit types in all places, with the order not
 * bound to the order in the enum so ABI-compatible extension is possible.
 */
class KREPORT_EXPORT KReportUnit
{
    Q_DECLARE_TR_FUNCTIONS(KReportUnit)
public:
    /** Length units supported by %KReport. */
    enum class Type {
        Invalid,
        Millimeter,
        Centimeter,
        Decimeter,
        Inch,
        Pica,
        Cicero,
        Point,  ///< Postscript point, 1/72th of an Inco
        Pixel,
        Last = Pixel ///< internal
    };

    /**
     * @brief Constructs invalid unit
      *
      * @since 3.1
      */
    KReportUnit();

    /** Construct unit with given type and factor. */
    explicit KReportUnit(Type type, qreal factor = 1.0);

    KReportUnit(const KReportUnit &other);

    ~KReportUnit();

    /// Assigns specified type and factor 1.0 to the object
    /// @param unit Type of unit
    KReportUnit& operator=(Type type);

    KReportUnit& operator=(const KReportUnit& other);

    bool operator!=(const KReportUnit &other) const;

    bool operator==(const KReportUnit &other) const;

    /**
     * @brief Returns true if type of this unit is valid
     *
     * @since 3.1
     */
    bool isValid() const;

    /**
     * @brief Returns list of all supported types (without Invalid)
     *
     * @since 3.1
     */
    static QList<Type> allTypes();

    /** Returns the type of this unit */
    KReportUnit::Type type() const;

    /**
     * @brief Returns (translated) description string for type of this unit
     *
     * @since 3.1
     */
    QString description() const;

    /**
     * @brief Returns (translated) description string for given unit type
     *
     * @since 3.1
     */
    static QString description(KReportUnit::Type type);

    /**
     * @brief Returns the list of (translated) description strings for given list of types
     *
     * @since 3.1
     */
    static QStringList descriptions(const QList<Type> &types);

    void setFactor(qreal factor);

    qreal factor() const;

    /**
     * Prepare ptValue to be displayed in pt
     * This method will round to 0.001 precision
     */
    static inline qreal toPoint(qreal ptValue)
    {
        // No conversion, only rounding (to 0.001 precision)
        return floor(ptValue * 1000.0) / 1000.0;
    }

    /**
     * Prepare ptValue to be displayed in mm
     * This method will round to 0.0001 precision, use POINT_TO_MM() for lossless conversion.
     */
    static inline qreal toMillimeter(qreal ptValue)
    {
        // "mm" values are rounded to 0.0001 millimeters
        return floor(POINT_TO_MM(ptValue) * 10000.0) / 10000.0;
    }

    /**
     * Prepare ptValue to be displayed in cm
     * This method will round to 0.0001 precision, use POINT_TO_CM() for lossless conversion.
     */
    static inline qreal toCentimeter(qreal ptValue)
    {
        return floor(POINT_TO_CM(ptValue) * 10000.0) / 10000.0;
    }

    /**
     * Prepare ptValue to be displayed in dm
     * This method will round to 0.0001 precision, use POINT_TO_DM() for lossless conversion.
     */
    static inline qreal toDecimeter(qreal ptValue)
    {
        return floor(POINT_TO_DM(ptValue) * 10000.0) / 10000.0;
    }

    /**
     * Prepare ptValue to be displayed in inch
     * This method will round to 0.00001 precision, use POINT_TO_INCH() for lossless conversion.
     */
    static inline qreal toInch(qreal ptValue)
    {
        // "in" values are rounded to 0.00001 inches
        return floor(POINT_TO_INCH(ptValue) * 100000.0) / 100000.0;
    }

    /**
     * Prepare ptValue to be displayed in pica
     * This method will round to 0.00001 precision, use POINT_TO_PI() for lossless conversion.
     */
    static inline qreal toPica(qreal ptValue)
    {
        // "pi" values are rounded to 0.00001 inches
        return floor(POINT_TO_PI(ptValue) * 100000.0) / 100000.0;
    }

    /**
     * Prepare ptValue to be displayed in cicero
     * This method will round to 0.00001 precision, use POINT_TO_CC() for lossless conversion.
     */
    static inline qreal toCicero(qreal ptValue)
    {
        // "cc" values are rounded to 0.00001 inches
        return floor(POINT_TO_CC(ptValue) * 100000.0) / 100000.0;
    }

    /**
     * convert the given value directly from one unit to another with high accuracy
     */
    static qreal convertFromUnitToUnit(qreal value, const KReportUnit &fromUnit,
                                       const KReportUnit &toUnit, qreal factor = 1.0);

    /**
     * convert the given value directly from one unit to another with high accuracy
     */
    static QPointF convertFromUnitToUnit(const QPointF &value,
                                         const KReportUnit &fromUnit,
                                         const KReportUnit &toUnit);

    /**
     * convert the given value directly from one unit to another with high accuracy
     */
    static QSizeF convertFromUnitToUnit(const QSizeF &value, const KReportUnit &fromUnit,
                                        const KReportUnit &toUnit);

    /**
     * This method is the one to use to display a value in a dialog
     * \return the value @a ptValue converted to unit and rounded, ready to be displayed
     */
    qreal toUserValue(qreal ptValue) const;

    /**
     * Convert the value @a ptValue to a given unit @a unit
     * Unlike KReportUnit::ptToUnit the return value remains unrounded, so that it can be used in complex calculation
     * \return the converted value
     */
    static qreal ptToUnit(qreal ptValue, const KReportUnit &unit);

    /// This method is the one to use to display a value in a dialog
    /// @return the value @a ptValue converted the unit and rounded, ready to be displayed
    QString toUserStringValue(qreal ptValue) const;

    //! @return the value converted to points with high accuracy
    qreal convertToPoint(qreal value) const;

    //! @return the value converted from points to the actual unit with high accuracy
    qreal convertFromPoint(qreal ptValue) const;

    //! @return the value converted from points to the actual unit with high accuracy
    QPointF convertFromPoint(const QPointF &ptValue) const;

    //! @return the value converted from points to the actual unit with high accuracy
    QSizeF convertFromPoint(const QSizeF &ptValue) const;

    //! Equal to convertToPoint(), use convertToPoint() instead for clarity
    inline qreal fromUserValue(qreal value) const { return convertToPoint(value); }

    /// @return the value converted to points with high accuracy
    QPointF convertToPoint(const QPointF &value) const;

    /// @return the value converted to points with high accuracy
    QSizeF convertToPoint(const QSizeF &value) const;

    /// @return the value converted to points with high accuracy
    /// @param value value entered by the user
    /// @param ok if set, the pointed bool is set to true if the value could be
    /// converted to a qreal, and to false otherwise.
    /// @return the value converted to points for internal use
    qreal convertToPoint(const QString &value, bool *ok = nullptr) const;

    //! Equal to convertToPoint(), use convertToPoint() instead for clarity
    inline qreal fromUserValue(const QString &value, bool *ok = nullptr) const {
        return convertToPoint(value, ok);
    }

    //! Returns the symbol string of given unit type
    //! Symbol for Invalid type is empty string.
    static QString symbol(KReportUnit::Type type);

    //! Returns the symbol string of the unit
    //! Symbol for Invalid type is empty string.
    QString symbol() const;

    /**
     * Equal to symbol(): returns the symbol string of the unit.
     */
    inline QString toString() const
    {
        return symbol();
    }

    /**
     * @brief Returns a unit symbol string to type
     *
     * @param symbol symbol to convert, must be lowercase
     * @return Invalid type for unsupported symbol
     *
     * @since 3.1
     */
    static KReportUnit::Type symbolToType(const QString &symbol);

    /**
     * @brief Returns the list of unit symbols for the given types
     *
     * @since 3.1
     */
    static QStringList symbols(const QList<Type> &types);

    /// Parses common %KReport and ODF values, like "10cm", "5mm" to pt.
    /// If no unit is specified, pt is assumed.
    /// @a defaultVal is in Points
    static qreal parseValue(const QString &value, qreal defaultVal = 0.0);

    /// parse an angle to its value in degrees
    /// @a defaultVal is in degrees
    static qreal parseAngle(const QString &value, qreal defaultVal = 0.0);

private:
    class Private;
    Private * const d;
};

#ifndef QT_NO_DEBUG_STREAM
KREPORT_EXPORT QDebug operator<<(QDebug, const KReportUnit &);
#endif

Q_DECLARE_METATYPE(KReportUnit)

#endif
