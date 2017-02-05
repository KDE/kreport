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

#include "KReportDataSource.h"
#include <QVariant>

#define KReportDataSortedFieldPrivateArgs(o) std::tie(o.field, o.order)

class KReportDataSource::SortedField::Private 
{
    
public:
    Private() {}
    Private(const Private& other) {
            KReportDataSortedFieldPrivateArgs((*this)) = KReportDataSortedFieldPrivateArgs(other);
    }
    QString field;
    Qt::SortOrder order = Qt::AscendingOrder;
};

class KReportDataSource::Private
{
public:
    bool dummy = true;
};

//==========KReportData::SortedField==========

KReportDataSource::SortedField::SortedField()
    : d(new Private)
{
}

KReportDataSource::SortedField::SortedField(const KReportDataSource::SortedField& other) : d(new Private(*other.d))
{
}


KReportDataSource::SortedField::~SortedField()
{
    delete d;
}

KReportDataSource::SortedField & KReportDataSource::SortedField::operator=(const KReportDataSource::SortedField& other)
{
    if (this != &other) {
        setField(other.field());
        setOrder(other.order());
    }
    return *this;
}

bool KReportDataSource::SortedField::operator==(const KReportDataSource::SortedField& other) const
{
    return KReportDataSortedFieldPrivateArgs((*d)) == KReportDataSortedFieldPrivateArgs((*other.d));
}

bool KReportDataSource::SortedField::operator!=(const KReportDataSource::SortedField& other) const
{
    return KReportDataSortedFieldPrivateArgs((*d)) != KReportDataSortedFieldPrivateArgs((*other.d));
}

QString KReportDataSource::SortedField::field() const
{
    return d->field;
}

Qt::SortOrder KReportDataSource::SortedField::order() const
{
    return d->order;
}

void KReportDataSource::SortedField::setField(const QString& field)
{
    d->field = field;
}

void KReportDataSource::SortedField::setOrder(Qt::SortOrder order)
{
    d->order = order;
}


//==========KReportData==========

KReportDataSource::KReportDataSource() : d(new Private())
{
}

KReportDataSource::~KReportDataSource()
{
    delete d;
}

QStringList KReportDataSource::fieldKeys() const
{
    return fieldNames();
}

QString KReportDataSource::sourceName() const
{
    return QString();
}

QString KReportDataSource::sourceClass() const
{
    return QString();
}

void KReportDataSource::setSorting(const QList<SortedField> &sorting)
{
    Q_UNUSED(sorting);
}

void KReportDataSource::addCondition(const QString &field, const QVariant &value, const QString& relation)
{
    Q_UNUSED(field);
    Q_UNUSED(value);
    Q_UNUSED(relation);
}

#ifdef KREPORT_SCRIPTING
QStringList KReportDataSource::scriptList() const
{
    return QStringList();
}

QString KReportDataSource::scriptCode(const QString &script) const
{
    Q_UNUSED(script);
    return QString();
}
#endif

QStringList KReportDataSource::dataSourceNames() const
{
    return QStringList();
}

QString KReportDataSource::dataSourceCaption(const QString &dataSourceName) const
{
    return dataSourceName;
}

KReportDataSource* KReportDataSource::create(const QString &source) const
{
    Q_UNUSED(source);
    return 0;
}
