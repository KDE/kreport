/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
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

#ifndef KREPORTSCHEMADATA_H
#define KREPORTSCHEMADATA_H

#include "config-kreport.h"
#include "kreport_export.h"
#include "KReportSectionData.h"
#include "KReportPageOptions.h"

class KReportDetailSectionData;

#ifdef KREPORT_SCRIPTING
namespace Scripting
{
class Report;
}
#endif

/**
*/
class KREPORT_EXPORT KReportDocument : public QObject
{
    Q_OBJECT

public:
    explicit KReportDocument(const QDomElement & elemSource, QObject *parent = 0);
    explicit KReportDocument(QObject *parent = 0);
    ~KReportDocument();

    bool isValid() const {
        return m_valid;
    }

    /**
    \return a list of all objects in the report
    */
    QList<KReportItemBase*> objects() const;

    /**
    \return a report object given its name
    */
    KReportItemBase* object(const QString&) const;

    /**
    \return all the sections, including groups and detail
    */
    QList<KReportSectionData*> sections() const;

    /**
    \return a sectiondata given a section enum
    */
    KReportSectionData* section(KReportSectionData::Section) const;

    /**
    \return a sectiondata given its name
    */
    KReportSectionData* section(const QString&) const;

    QString query() const {
        return m_query;
    }
#ifdef KREPORT_SCRIPTING
    QString script() const {
        return m_script;
    };
    QString interpreter() const {
        return m_interpreter;
    }
#endif

    bool externalData() const {
        return m_externalData;
    }

    KReportDetailSectionData* detail() const {
        return m_detailSection;
    }

    void setName(const QString&n) {
        m_name = n;
    }
    QString name() const {
        return m_name;
    }

    KReportPageOptions pageOptions() const;

protected:
    QString m_title;
    QString m_name;
    QString m_query;
#ifdef KREPORT_SCRIPTING
    QString m_script;
    QString m_interpreter;
#endif
    bool m_externalData;

    KReportPageOptions page;

    KReportSectionData * m_pageHeaderFirst;
    KReportSectionData * m_pageHeaderOdd;
    KReportSectionData * m_pageHeaderEven;
    KReportSectionData * m_pageHeaderLast;
    KReportSectionData * m_pageHeaderAny;

    KReportSectionData * m_reportHeader;
    KReportSectionData * m_reportFooter;

    KReportSectionData * m_pageFooterFirst;
    KReportSectionData * m_pageFooterOdd;
    KReportSectionData * m_pageFooterEven;
    KReportSectionData * m_pageFooterLast;
    KReportSectionData * m_pageFooterAny;

    KReportDetailSectionData* m_detailSection;

private:
    bool m_valid;
    void init();

    friend class KReportPreRendererPrivate;
    friend class KReportPreRenderer;
#ifdef KREPORT_SCRIPTING
    friend class KReportScriptHandler;
    friend class Scripting::Report;
#endif
};

#endif
