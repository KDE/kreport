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

#ifndef KREPORTDESIGNERSECTIONDETAILGROUP_H
#define KREPORTDESIGNERSECTIONDETAILGROUP_H

#include <QObject>

#include "kreport_export.h"

class QDomElement;
class QDomDocument;
class QString;
class QWidget;

class KReportDesignerSection;
class KReportDesignerSectionDetail;

/*!
 * @brief A section group allows a header and footer to be used for a particular report field
*/
class KREPORT_EXPORT KReportDesignerSectionDetailGroup : public QObject
{
    Q_OBJECT
public:
    KReportDesignerSectionDetailGroup(const QString &column, KReportDesignerSectionDetail *rsd,
                                      QWidget *parent = nullptr);
    ~KReportDesignerSectionDetailGroup() override;

    enum class PageBreak {
        None,
        AfterGroupFooter,
        BeforeGroupHeader
    };

    void setColumn(const QString &);
    QString column() const;

    void setGroupHeaderVisible(bool yes = true);
    bool groupHeaderVisible() const;

    void setGroupFooterVisible(bool yes = true);
    bool groupFooterVisible() const;

    void setPageBreak(PageBreak);
    PageBreak  pageBreak() const;

    void setSort(Qt::SortOrder);
    Qt::SortOrder sort();

    KReportDesignerSection * groupHeader() const;
    KReportDesignerSection * groupFooter() const;

    void buildXML(QDomDocument *doc, QDomElement *section) const;
    void initFromXML( const QDomElement &element );

private:
    Q_DISABLE_COPY(KReportDesignerSectionDetailGroup)
    class Private;
    Private * const d;
};


#endif
