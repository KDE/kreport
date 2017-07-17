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

#include "KReportDesign.h"
#include "KReportDesign_p.h"
#include "KReportElement.h"
#include "KReportUnit.h"
#include "KReportUtils.h"
#include "KReportPluginManager.h"
#include "KReportPluginInterface.h"

#include <QDomDocument>
#include <QDomElement>
#include <QPrinter>
#include <QPrinterInfo>

class Q_DECL_HIDDEN KReportDesignReadingStatus::Private
{
public:
    QString errorMessage;
    QString errorDetails;
    int errorLineNumber = -1;
    int errorColumnNumber = -1;
};

KReportDesignReadingStatus::KReportDesignReadingStatus() : d(new Private)
{
}

KReportDesignReadingStatus::~KReportDesignReadingStatus()
{
    delete d;
}

KReportDesignReadingStatus::KReportDesignReadingStatus(const KReportDesignReadingStatus& other) : d(new Private)
{
    *this = other;
}

KReportDesignReadingStatus& KReportDesignReadingStatus::operator=(const KReportDesignReadingStatus &other)
{
    if (this != &other) {
            setErrorMessage(other.errorMessage());
            setErrorDetails(other.errorDetails());
            setErrorLineNumber(other.errorLineNumber());
            setErrorColumnNumber(other.errorColumnNumber());
    }
    
    return *this;
}


bool KReportDesignReadingStatus::isError() const
{
    return d->errorLineNumber >= 0 && d->errorColumnNumber >= 0;
}

int KReportDesignReadingStatus::errorColumnNumber() const
{
    return d->errorColumnNumber;
}

QString KReportDesignReadingStatus::errorDetails() const
{
    return d->errorDetails;
}

QString KReportDesignReadingStatus::errorMessage() const
{
    return d->errorMessage;
}


int KReportDesignReadingStatus::errorLineNumber() const
{
    return d->errorLineNumber;
}

void KReportDesignReadingStatus::setErrorColumnNumber(int column)
{
    d->errorColumnNumber = column;
}

void KReportDesignReadingStatus::setErrorDetails(const QString& details)
{
    d->errorDetails = details;
}

void KReportDesignReadingStatus::setErrorLineNumber(int line)
{
    d->errorLineNumber = line;
}

void KReportDesignReadingStatus::setErrorMessage(const QString& msg)
{
    d->errorMessage = msg;
}

QDebug operator<<(QDebug dbg, const KReportDesignReadingStatus& status)
{
    if (status.isError()) {
        dbg.nospace() << qPrintable(
            QString::fromLatin1("KReportDesignReadingStatus: errorMessage=\"%1\" "
                                "errorDetails=\"%2\" line=%3 column=%4")
                .arg(status.errorMessage()).arg(status.errorDetails())
                .arg(status.errorLineNumber()).arg(status.errorColumnNumber()));
    } else {
        dbg.nospace() << "KReportDesignReadingStatus: OK";
    }
    return dbg.space();
}

//-----------------------------------

KReportDesign::KReportDesign()
    : d(new Private(this))
{
}

KReportDesign::~KReportDesign()
{
    delete d;
}

bool KReportDesign::setContent(const QString &text, KReportDesignReadingStatus *status)
{
    QDomDocument doc;
    QString errorDetails;
    int errorLine;
    int errorColumn;
    
    if (!doc.setContent(text, &errorDetails, &errorLine, &errorColumn))
    {
        if (status) {
            status->setErrorMessage(tr("Could not parse XML document."));
            status->setErrorDetails(errorDetails);
            status->setErrorLineNumber(errorLine);
            status->setErrorColumnNumber(errorColumn);
        }
        return false;
    }
    bool ret = d->processDocument(doc, status);
    if (!ret && status) {
        status->setErrorMessage(tr("Error in XML document."));
    }
    return ret;
}

QString KReportDesign::toString(int indent) const
{
    Q_UNUSED(indent);
    //! @todo
    return QString();
}

QPageLayout KReportDesign::pageLayout() const
{
    return d->pageLayout;
}

QString KReportDesign::title() const
{
    return d->title;
}

void KReportDesign::setTitle(const QString &title)
{
    d->title = title;
}

void KReportDesign::setPageLayout(const QPageLayout &pageLayout)
{
    d->pageLayout = pageLayout;
    d->pageLayout.setUnits(QPageLayout::Point);
}

KReportElement KReportDesign::createElement(const QString &typeName, QString *errorMessage)
{
    QDomElement el;
    KReportDesignReadingStatus status;
    KReportPluginInterface* plugin = d->findPlugin(typeName, el, &status);
    if (!plugin) {
        if (errorMessage) {
            *errorMessage = status.errorMessage();
        }
        return KReportElement();
    }
    return plugin->createElement();
}

bool KReportDesign::hasSection(KReportSection::Type type) const
{
    const int index = static_cast<int>(type) - 1;
    if (0 <= index && index < d->sections.length()) {
        return d->sections[index];
    }
    return false;
}

KReportSection KReportDesign::section(KReportSection::Type type) const
{
    const int index = static_cast<int>(type) - 1;
    if (0 <= index && index < d->sections.length()) {
        KReportSection *section = d->sections[index];
        if (section) {
            return *section;
        }
    }
    return KReportSection();
}

void KReportDesign::addSection(const KReportSection &section)
{
    const int index = static_cast<int>(section.type()) - 1;
    if (0 <= index && index < d->sections.length()) {
        if (d->sections[index]) {
            *d->sections[index] = section;
        } else {
            d->sections[index] = new KReportSection(section);
        }
    }
}

// static
QPageLayout KReportDesign::defaultPageLayout()
{
    QPageLayout layout = KReportDesignGlobal::self()->defaultPageLayout;
    if (!layout.pageSize().isValid()) {
        if (!QPrinterInfo::defaultPrinter().isNull()) {
            layout.setPageSize(QPrinterInfo::defaultPrinter().defaultPageSize());
        }
        else {
            layout.setPageSize(QPageSize(DEFAULT_PAGE_SIZE));
        }
    }
    return layout;
}

// static
void KReportDesign::setDefaultPageLayout(const QPageLayout &pageLayout)
{
    KReportDesignGlobal::self()->defaultPageLayout = pageLayout;
    KReportDesignGlobal::self()->defaultPageLayout.setUnits(QPageLayout::Point);
}

#ifdef KREPORT_SCRIPTING
QString KReportDesign::script() const
{
    return d->script;
}
#endif
