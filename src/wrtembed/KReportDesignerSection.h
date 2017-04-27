/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2014 Jarosław Staniek <staniek@kde.org>
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

#ifndef __REPORTSECTION_H__
#define __REPORTSECTION_H__

#include <QWidget>

#include "kreport_export.h"

// forward declarations
class QDomNode;
class QDomDocument;
class QDomElement;
class QGraphicsItem;
typedef QList<QGraphicsItem*> QGraphicsItemList;

class KPropertySet;
class KProperty;

class KReportDesigner;
class KReportZoomHandler;

//
// Class ReportSection
//
//     This class is the base to all Report Section's visual representation.
// It contains the basic data and interface that all the sections need to work.
//
class KREPORT_EXPORT KReportDesignerSection : public QWidget
{
    Q_OBJECT
public:
    ~KReportDesignerSection() override;

    void setTitle(const QString & s);
    void buildXML(QDomDocument *doc, QDomElement *section);
    void initFromXML(const QDomNode & section);
    QSize sizeHint() const override;

    /**
     * @brief Return the items in the section
     * Only return top-level items ... ie, items with no parent item
     * because child items are not full report-items, they are implementation
     * details of a report item and do not need to be counted individually
     *
     * @return QGraphicsItemList
     */
    QGraphicsItemList items() const;

    void setSectionCursor(const QCursor&);
    void unsetSectionCursor();

protected Q_SLOTS:
    void slotResizeBarDragged(int delta);

protected:
    explicit KReportDesignerSection(KReportDesigner * rptdes,
                                    const KReportZoomHandler &zoomHandler);

private Q_SLOTS:
    void slotPageOptionsChanged(KPropertySet &);
    void slotSceneClicked();
    void slotPropertyChanged(KPropertySet &, KProperty &);

private:
    Q_DISABLE_COPY(KReportDesignerSection)
    class Private;
    Private * const d;
    friend class KReportDesigner;
    friend class KReportDesignerSectionTitle;
};

#endif

