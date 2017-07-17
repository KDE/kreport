/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC <info@openmfg.com>
 * Copyright (C) 2007-2010 by Adam Pigg <adam@piggz.co.uk>
 * Copyright (C) 2011-2015 Jarosław Staniek <staniek@kde.org>
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

#ifndef KREPORTDESIGN_P_H
#define KREPORTDESIGN_P_H

#include "KReportDesign.h"
#include "KReportUnit.h"
#include "KReportUtils_p.h"

#include <QPageSize>
#include <QPageLayout>
#include <QVarLengthArray>

class QDomDocument;
class QDomElement;
class KReportPluginInterface;

extern const bool DEFAULT_SHOW_GRID;
extern const bool DEFAULT_SNAP_TO_GRID;
extern const int DEFAULT_GRID_DIVISIONS;
extern const KReportUnit::Type DEFAULT_UNIT_TYPE;
extern const KReportUnit DEFAULT_UNIT;
extern const int DEFAULT_PAGE_MARGIN;
extern const QPageSize::PageSizeId DEFAULT_PAGE_SIZE;
extern const QPageLayout::Orientation DEFAULT_PAGE_ORIENTATION;

class Q_DECL_HIDDEN KReportDesign::Private
{
public:
    explicit Private(KReportDesign *design);

    ~Private();

    QDomElement requiredChildElement(const QDomElement &parent,
                                     const char* childElementName,
                                     KReportDesignReadingStatus *status) const;

    void unexpectedElement(const QDomElement &element,
                           KReportDesignReadingStatus *status) const;

    //! Processes document @a doc and sets status @a status
    bool processDocument(const QDomDocument &doc, KReportDesignReadingStatus *status);

    //! Processes @a el, a child of /report:content element and sets status @a status
    bool processContentElementChild(const QDomElement &el, KReportDesignReadingStatus *status);

    //! Processes @a el, a child of /report:content/report:body element and sets status @a status
    bool processBodyElementChild(const QDomElement &el, KReportDesignReadingStatus *status);

    //! Processes @a el, a /report:content/report:body/report:section element and sets status @a status
    KReportSection processSectionElement(const QDomElement &el, KReportDesignReadingStatus *status);

    //! Processes @a el,
    //! a child of /report:content/report:body/report:section element
    //! or a child of /report:content/report:body/report:detail/report:section element
    //! and sets status @a status.
    //! It is probably the lowest level in hierarchy and @a el refers to a single report element.
    KReportElement processSectionElementChild(const QDomElement &el, KReportDesignReadingStatus *status);

    //! Processes @a el, a child of /report:content/report:body/report:detail element and sets status @a status
    bool processDetailElement(const QDomElement &el, KReportDesignReadingStatus *status);

    //! Processes @a el, a /report:content/report:body/report:detail/report:group element and sets status @a status
    bool processGroupElement(const QDomElement &el, KReportDesignReadingStatus *status);

    KReportPluginInterface* findPlugin(const QString &typeName, const QDomElement &el,
                                        KReportDesignReadingStatus *status);

    KReportDesign * const q;

    // Visual settings only
    bool showGrid;
    bool snapToGrid;
    int gridDivisions;
    KReportUnit pageUnit;
    // END OF: Visual settings only
    QString title;
    KReportPrivate::PageLayout pageLayout;
    QVarLengthArray<KReportSection*, static_cast<int>(KReportSection::Type::Detail)> sections;
#ifdef KREPORT_SCRIPTING
    QString script;
    QString originalInterpreter; //!< used for backward-compatibility to save the original
#endif
};

class KReportDesignGlobal
{
public:
    KReportDesignGlobal();

    static KReportDesignGlobal* self();

    struct SectionTypeInfo {
        KReportSection::Type type;
        const char *name;
    };

    KReportSection::Type sectionType(const QString& typeName);

    QString sectionTypeName(KReportSection::Type sectionType);

    KReportPrivate::PageLayout defaultPageLayout;
    qreal defaultSectionHeight;
    QColor defaultSectionBackgroundColor;

private:
    void initSectionTypes();

    static const SectionTypeInfo sectionTypes[];
    QHash<QString, KReportSection::Type> sectionTypesForName;
    QHash<KReportSection::Type, QString> sectionTypeNames;
};

#endif
