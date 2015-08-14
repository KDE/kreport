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
#include "KoReportPluginManager.h"
#include "KoReportPluginInterface.h"

#include <QDomDocument>
#include <QDomElement>
#include <QPrinter>
#include <QPrinterInfo>

KReportDesignReadingStatus::KReportDesignReadingStatus()
    : lineNumber(-1), columnNumber(-1)
{
}

bool KReportDesignReadingStatus::isError() const
{
    return lineNumber >= 0;
}

QDebug operator<<(QDebug dbg, const KReportDesignReadingStatus& status)
{
    if (status.isError()) {
        dbg.nospace() << qPrintable(
            QString::fromLatin1("KReportDesignReadingStatus: errorMessage=\"%1\" "
                                "errorDetails=\"%2\" line=%3 column=%4")
                .arg(status.errorMessage).arg(status.errorDetails)
                .arg(status.lineNumber).arg(status.columnNumber));
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
    if (!doc.setContent(text, status ? &status->errorDetails : 0, status ? &status->lineNumber : 0,
                        status ? &status->columnNumber : 0))
    {
        if (status) {
            status->errorMessage = KReportDesign::tr("Could not parse XML document.");
        }
        return false;
    }
    return d->processDocument(doc, status);
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
    KoReportPluginInterface* plugin = d->findPlugin(typeName, el, &status);
    if (!plugin) {
        if (errorMessage) {
            *errorMessage = status.errorMessage;
        }
        return KReportElement();
    }
    return plugin->createElement();
}

bool KReportDesign::hasSection(KReportSection::Type type) const
{
    const int index = type - 1;
    if (0 <= index && index < d->sections.length()) {
        return d->sections[index];
    }
    return false;
}

KReportSection KReportDesign::section(KReportSection::Type type) const
{
    const int index = type - 1;
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
    const int index = section.type() - 1;
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
