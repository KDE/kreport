/* This file is part of the KDE project
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

#ifndef SCRIPTINGKRSCRIPTBARCODE_H
#define SCRIPTINGKRSCRIPTBARCODE_H

#include <QObject>

#include "KReportItemBarcode.h"

namespace Scripting
{

/**
*/
class Barcode : public QObject
{
    Q_OBJECT
public:
    explicit Barcode(KReportItemBarcode *f);

    ~Barcode() override;

public Q_SLOTS:


    /**
     * Get the position of the barcode
     * @return position in points
     */
    QPointF position() const;

    /**
     * Sets the position of the barcode in points
     * @param Position
     */
    void setPosition(const QPointF&);

    /**
     * Get the size of the barcode
     * @return size in points
     */
    QSizeF size() const;

    /**
     * Set the size of the barcode in points
     * @param Size
     */
    void setSize(const QSizeF&);

    /**
     * Get the horizontal alignment
     * Qt::AlignLeft Left
     * Qt::AlignHCenter Center
     * Qt::AlignRight Right
     * @return alignment
    */
    Qt::Alignment horizontalAlignment() const;

    /**
     * Sets the horizontal alignment
     * @param  Alignemnt
     */
    void setHorizonalAlignment(Qt::Alignment value);


    /**
     * @return the data source for the element
     * The source can be a column name or a valid script expression if prefixed with a '='.
     */
    QString source() const;


    /**
     * Sets the data source for the element.
     * @see source()
     */
    void setSource(const QString &s);


    /**
     * Get the barcode format
     * @return format as string
     */
    QString format() const;

    /**
     * Set the barcode format
     * @param format
     */
    void setFormat(const QString&);


private:
    KReportItemBarcode *m_barcode;
};

}

#endif
