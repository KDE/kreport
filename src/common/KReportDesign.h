/* This file is part of the KDE project
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

#ifndef KREPORTDESIGN_H
#define KREPORTDESIGN_H

#include "kreport_export.h"
#include "config-kreport.h"
#include "KReportSection.h"

#include <QCoreApplication>

class QPageSize;
class QMarginsF;
class QPageLayout;
class KReportElement;

//! The KReportDesignReadStatus represents status of reading a report design in .kreport format.
/*! It is used by KReportDesign::setContent(). */
class KREPORT_EXPORT KReportDesignReadingStatus
{
public:
    //! Creates an empty status object.
    /*! For empty status objects isError() returns false. */
    KReportDesignReadingStatus();

    ~KReportDesignReadingStatus();
    KReportDesignReadingStatus(const KReportDesignReadingStatus &other);
    KReportDesignReadingStatus& operator=(const KReportDesignReadingStatus &other);
    
    //! @return true if the status is error.
    //! Equivalent of errorLineNumber() >= 0 && errorColumnNumber() >= 0.
    bool isError() const;
    
    //! Error message suitable for displaying to the user, translated.
    QString errorMessage() const;
    
    //! Detailed error message, partially translated.    
    QString errorDetails() const;
    
    //! Line number (counting from 0) in which the error occured. -1 if there is no error.
    int errorLineNumber() const;  
    
    //! Column number (counting from 0) in which the error occured. -1 if there is no error.
    int errorColumnNumber() const;
    
    void setErrorMessage(const QString& msg);
    void setErrorDetails(const QString& details);
    void setErrorLineNumber(int line);
    void setErrorColumnNumber(int column);
    
private:
    class Private;
    Private * const d;    
};

//! Sends information about the reading status @a status to debug output @a dbg.
KREPORT_EXPORT QDebug operator<<(QDebug dbg, const KReportDesignReadingStatus& status);

//! The KReportDesign class represents a report design in .kreport format
class KREPORT_EXPORT KReportDesign
{
    Q_DECLARE_TR_FUNCTIONS(KReportDesign)
public:
    KReportDesign();

    ~KReportDesign();

    //! Reads the XML document in .kreport format from the string @a text
    //! @return true if the content was successfully parsed
    //! On failure false is returned and if @a status is provided, it is updated accordingly.
    bool setContent(const QString &text, KReportDesignReadingStatus *status = 0);

    //! Converts the report document back to its textual representation.
    QString toString(int indent = 1) const;

    //! @return title for this design
    QString title() const;

    //! Sets title for this design to @a title
    void setTitle(const QString &title);

    //! @return page layout for this design
    QPageLayout pageLayout() const;

    //! Sets the page layout to @a pageLayout
    //! @note Calling this method does not alter page layouts of existing KReportDesign objects.
    void setPageLayout(const QPageLayout &pageLayout);

    //! @return true if this design has section defined of type @a type
    bool hasSection(KReportSection::Type type) const;

    //! @return section of type @a type
    KReportSection section(KReportSection::Type type) const;

    //! Add section @a section. Previous section of the same type is removed from this design.
    void addSection(const KReportSection &section);

    //! Creates and returns report element of type @a typeName
    //! On success @a errorMessage is cleared, on failure it is set to a nonempty value.
    KReportElement createElement(const QString &typeName, QString *errorMessage);

    //! @return default page layout that is used for creating new report designs
    /*! Attributes that are specified in the design format:
        - margins: by default equal to equivalent of 1cm in points (QPageLayout::Point).
        - mode: by default QPageLayout::StandardMode
        - orientation: by default QPageLayout::Portrait
        - pageSize: by default equal to default page size of the default printer
                     (QPrinterInfo::defaultPrinter().defaultPageSize()).
                     If there is no default printer, A4 size is used.
                     Passing invalid page size restores defaults explained in
                     documentation of QPageLayout defaultPageLayout().
                     @todo For KDE Plasma use information from the Locale by using
                           readConfigNumEntry("PageSize", QPrinter::A4, m_pageSize, QPrinter::PageSize)
                           from KLocalePrivate::initFormat() (klocale_kde.cpp)

       Other attributes are ignored by the design format.
       In particular units for margins and pageSize are always QPageLayout::Point.
     */
    static QPageLayout defaultPageLayout();

    //! Sets default page layout to @a pageLayout
    //! This information is used when a new report design is created.
    static void setDefaultPageLayout(const QPageLayout &pageLayout);

#ifdef KREPORT_SCRIPTING
    //! @return text of the script program
    QString script() const;
#endif

private:
    Q_DISABLE_COPY(KReportDesign)
    class Private;
    Private * const d;
};

#endif // KREPORTDESIGN_H
