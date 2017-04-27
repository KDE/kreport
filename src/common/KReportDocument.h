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

#ifndef KREPORTDOCUMENT_H
#define KREPORTDOCUMENT_H

#include "config-kreport.h"
#include "kreport_export.h"
#include "KReportSectionData.h"

#include <QPageLayout>

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
    explicit KReportDocument(const QDomElement &elemSource, QObject *parent = nullptr);
    explicit KReportDocument(QObject *parent = nullptr);
    ~KReportDocument() override;

    bool isValid() const;

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

    QString query() const;
#ifdef KREPORT_SCRIPTING
    QString script() const;

    QString interpreter() const;
#endif

    bool externalData() const;

    KReportDetailSectionData* detail() const {
        return m_detailSection;
    }

    void setName(const QString&n);
    QString name() const;

    QString title() const;

    QPageLayout pageLayout() const;
    
    QString pageSize();
    void setPageSize(const QString &size);
    
protected:


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
    void init();
    
    friend class KReportPreRendererPrivate;
    friend class KReportPreRenderer;
#ifdef KREPORT_SCRIPTING
    friend class KReportScriptHandler;
    friend class Scripting::Report;
#endif
    
    //! TODO add support for labels
    QString labelType() const;
    void setLabelType(const QString &label);
    
    class Private;
    Private * const d;
};

#endif
