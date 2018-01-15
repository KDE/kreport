/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010-2018 Jarosław Staniek <staniek@kde.org>
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


#ifndef KREPORTSECTIONDATA_H
#define KREPORTSECTIONDATA_H

#include "KReportUnit.h"

#include <KPropertySet>

#include <QColor>

class KReportItemBase;
class KReportDocument;
class QDomElement;

namespace Scripting
{
class Section;
}

/**
 * KReportSectionData is used to store the information about a specific report section
 *
 * A section has a name, type, unit and optionally extra data.
 */
class KREPORT_EXPORT KReportSectionData : public QObject
{
    Q_OBJECT
public:
    enum class Type {
        None,
        PageHeaderFirst,
        PageHeaderOdd,
        PageHeaderEven,
        PageHeaderLast,
        PageHeaderAny,
        ReportHeader,
        ReportFooter,
        PageFooterFirst,
        PageFooterOdd,
        PageFooterEven,
        PageFooterLast,
        PageFooterAny,
        GroupHeader,
        GroupFooter,
        Detail
    };

    explicit KReportSectionData(QObject* parent = nullptr);

    explicit KReportSectionData(const QDomElement &elemSource, QObject *parent = nullptr);

    ~KReportSectionData() override;

    KReportUnit unit() const;

    void setUnit(const KReportUnit &u);

    /**
     * Returns property set for this section
     *
     * @since 3.1
     */
    KPropertySet* propertySet();

    /**
     * @overload
     */
    const KPropertySet* propertySet() const;

    bool isValid() const;

    qreal height() const;

    void setHeight(qreal ptHeight);

    QList<KReportItemBase*> objects() const;

    QString name() const;

    QColor backgroundColor() const;

    Type type() const;

    static KReportSectionData::Type sectionTypeFromString(const QString& s);
    static QString sectionTypeString(KReportSectionData::Type type);

private:
    void setBackgroundColor(const QColor &color);
    void setHeight(qreal ptHeight, KProperty::ValueOptions options);
    KReportItemBase* object(int index);

    Q_DISABLE_COPY(KReportSectionData)
    class Private;
    Private * const d;

    friend class Scripting::Section;
    friend class KReportDesignerSection;
};

#endif
