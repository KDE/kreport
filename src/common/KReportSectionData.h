/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>
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

#include <QColor>

#include <KPropertySet>

class KReportItemBase;
class KReportDocument;
class QDomElement;

namespace Scripting
{
class Section;
}

//
// KReportSectionData is used to store the information about a specific
// section.
// A section has a name and optionally extra data. `name'
// reportheader, reportfooter, pageheader, pagefooter, groupheader, groupfooter or detail.
// In the case of pghead and pgfoot extra would contain the page
// designation (firstpage, odd, even or lastpage).
class KReportSectionData : public QObject
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
    KReportSectionData(const QDomElement &, KReportDocument* report);
    ~KReportSectionData() override;
    KPropertySet* propertySet() const {
        return m_set;
    }

    bool isValid() const {
        return m_valid;
    }

    qreal height() const {
        return m_height->value().toDouble();
    }

    QList<KReportItemBase*> objects() const {
        return m_objects;
    }

    QString name() const;

    QColor backgroundColor() const {
        return m_backgroundColor->value().value<QColor>();
    }

    Type type() const {
        return m_type;
    }

    static KReportSectionData::Type sectionTypeFromString(const QString& s);
    static QString sectionTypeString(KReportSectionData::Type type);
protected:
    KPropertySet *m_set;
    KProperty *m_height;
    KProperty *m_backgroundColor;

private:
    void createProperties(const QDomElement & elemSource);

    QList<KReportItemBase*> m_objects;

    Type m_type;

    static bool zLessThan(KReportItemBase* s1, KReportItemBase* s2);
    static bool xLessThan(KReportItemBase* s1, KReportItemBase* s2);

    bool m_valid;

    friend class Scripting::Section;
    friend class KReportDesignerSection;
};

#endif
