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

#ifndef KREPORTDESIGNERSECTIONDETAIL_H
#define KREPORTDESIGNERSECTIONDETAIL_H

#include <QWidget>

#include "kreport_export.h"

class QDomNode;
class QDomElement;
class QDomDocument;

class KReportDesignerSection;
class KReportDesigner;
class KReportDesignerSectionDetailGroup;

/**
*/
class KREPORT_EXPORT KReportDesignerSectionDetail : public QWidget
{
    Q_OBJECT
public:
    explicit KReportDesignerSectionDetail(KReportDesigner * rptdes);
    virtual ~KReportDesignerSectionDetail();

    enum PageBreak {
        BreakNone = 0,
        BreakAtEnd = 1
    };

    void setPageBreak(int);
    int pageBreak() const;

    KReportDesignerSection * detailSection() const;

    void buildXML(QDomDocument *doc, QDomElement *section);
    void initFromXML(QDomNode *node);

    KReportDesigner * reportDesigner() const;

    int groupSectionCount() const;
    KReportDesignerSectionDetailGroup * groupSection(int i) const;
    void insertGroupSection(int idx, KReportDesignerSectionDetailGroup * rsd);
    int indexOfGroupSection(const QString & column) const;
    void removeGroupSection(int idx, bool del = false);
    virtual QSize sizeHint() const;

    void setSectionCursor(const QCursor&);
    void unsetSectionCursor();

private:
    Q_DISABLE_COPY(KReportDesignerSectionDetail)
    class Private;
    Private * const d;
};

#endif
