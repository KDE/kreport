/* This file is part of the KDE project
 * Copyright (C) 2007-2010 by Adam Pigg (adam@piggz.co.uk)
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
#ifndef KREPORTDATA_H
#define KREPORTDATA_H

#include <QStringList>
#include <QVariant>

#include "kreport_export.h"
#include "config-kreport.h"

/*!
 * @brief Abstraction of report data source.
 *
 * A data source provides data to the report engine, usually from a
 * database, but could also be implemented for text and other file formats
 */
class KREPORT_EXPORT KReportDataSource
{

public:
    KReportDataSource();
    virtual ~KReportDataSource();

    /*!
     * @brief Describes sorting for single field,
     * By default the order is ascending.
     */
    class KREPORT_EXPORT SortedField
    {
    public:
        SortedField();
        SortedField(const SortedField& other);
        ~SortedField();
        SortedField& operator=(const SortedField &other);
        bool operator==(const SortedField &other) const;
        bool operator!=(const SortedField &other) const;
        void setField(const QString &field);
        void setOrder(Qt::SortOrder order);
        QString field() const;
        Qt::SortOrder order() const;

        private:
            class Private;
            Private * const d;
    };

    //! Open the dataset
    virtual bool open() = 0;

    //! Close the dataset
    virtual bool close() = 0;

    //! Move to the next record
    virtual bool moveNext() = 0;

    //! Move to the previous record
    virtual bool movePrevious() = 0;

    //! Move to the first record
    virtual bool moveFirst() = 0;

    //! Move to the last record
    virtual bool moveLast() = 0;

    //! Return the current position in the dataset
    virtual qint64 at() const = 0;

    //! Return the total number of records
    virtual qint64 recordCount() const = 0;

    //! Return the index number of the field given by nane field
    virtual int fieldNumber(const QString &field) const = 0;

    //! Return the list of field names
    virtual QStringList fieldNames() const = 0;

    //! Return the list of field keys. Returns fieldNames() by default
    virtual QStringList fieldKeys() const;

    //! Return the value of the field at the given position for the current record
    virtual QVariant value(int pos) const = 0;

    //! Return the value of the field fir the given name for the current record
    virtual QVariant value(const QString &field) const = 0;

    //! Return the name of this source
    virtual QString sourceName() const;

    //! @return the class name of this source
    virtual QString sourceClass() const;

    //! Sets the sorting for the data
    //! Should be called before open() so that the data source can be edited accordingly
    //! Default impl does nothing
    virtual void setSorting(const QList<SortedField> &sorting);

    //! Adds a condition to the data source
    virtual void addCondition(const QString &field, const QVariant &value, const QString& relation = QLatin1String("="));

    //! Return a list of data source names available for this data source
    //! Works after the source is opened
    virtual QStringList dataSourceNames() const = 0;

    //! Return data source caption for specified @a dataSourceName
    //! It is possibly translated. As such it is suitable for use in GUIs.
    //! Default implementation just returns @a dataSourceName.
    virtual QString dataSourceCaption(const QString &dataSourceName) const;

    //! Creates a new instance with data source. Default implementation returns @c nullptr.
    //! @a source is implementation-specific identifier.
    //! Owner of the returned pointer is the caller.
    Q_REQUIRED_RESULT virtual KReportDataSource* create(const QString &source) const;

private:
    Q_DISABLE_COPY(KReportDataSource)
    class Private;
    Private * const d;
};

#endif
