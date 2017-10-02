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

#include "KReportDesign_p.h"
#include "KReportElement.h"
#include "KReportUtils.h"
#include "KReportPluginManager.h"
#include "KReportPluginInterface.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QSizeF>

const bool DEFAULT_SHOW_GRID = true;
const bool DEFAULT_SNAP_TO_GRID = true;
const int DEFAULT_GRID_DIVISIONS = 4;
const KReportUnit::Type DEFAULT_UNIT_TYPE = KReportUnit::Type::Centimeter;
const KReportUnit DEFAULT_UNIT(DEFAULT_UNIT_TYPE);
const int DEFAULT_PAGE_MARGIN = CM_TO_POINT(1.0);
const QPageSize::PageSizeId DEFAULT_PAGE_SIZE = QPageSize::A4;
const QPageLayout::Orientation DEFAULT_PAGE_ORIENTATION = QPageLayout::Landscape;

KReportDesign::Private::Private(KReportDesign *design)
 : q(design)
 , showGrid(DEFAULT_SHOW_GRID)
 , snapToGrid(DEFAULT_SNAP_TO_GRID)
 , gridDivisions(DEFAULT_GRID_DIVISIONS)
 , pageUnit(DEFAULT_UNIT)
 , sections(static_cast<int>(KReportSection::Type::Detail))
{
    memset(static_cast<void*>(sections.data()), 0, sizeof(void*) * sections.length());
    pageLayout.setUnits(QPageLayout::Point); // initializate because of https://bugreports.qt.io/browse/QTBUG-47551
}

KReportDesign::Private::~Private()
{
    qDeleteAll(sections);
}

KReportDesignGlobal::KReportDesignGlobal()
    : defaultSectionHeight(CM_TO_POINT(2.0))
    , defaultSectionBackgroundColor(Qt::white)
{
    defaultPageLayout.setUnits(QPageLayout::Point);
    defaultPageLayout.setMargins(QMarginsF(DEFAULT_PAGE_MARGIN,
                                           DEFAULT_PAGE_MARGIN,
                                           DEFAULT_PAGE_MARGIN,
                                           DEFAULT_PAGE_MARGIN));
    defaultPageLayout.setMode(QPageLayout::StandardMode);
    defaultPageLayout.setOrientation(DEFAULT_PAGE_ORIENTATION);
}

KReportSection::Type KReportDesignGlobal::sectionType(const QString& typeName)
{
    initSectionTypes();
    return sectionTypesForName.value(typeName); // returns Invalid type for invalid name
}

inline uint qHash(KReportSection::Type sectionType, uint seed = 0)
{
    return qHash(static_cast<uint>(sectionType), seed);
}

QString KReportDesignGlobal::sectionTypeName(KReportSection::Type sectionType)
{
    initSectionTypes();
    return sectionTypeNames.value(sectionType);
}

void KReportDesignGlobal::initSectionTypes()
{
    if (!sectionTypesForName.isEmpty()) {
        return;
    }
    for (const SectionTypeInfo *info = sectionTypes; info->name; ++info) {
        sectionTypesForName.insert(QString::fromLatin1(info->name), info->type);
        sectionTypeNames.insert(info->type, QString::fromLatin1(info->name));
    }
}

const KReportDesignGlobal::SectionTypeInfo KReportDesignGlobal::sectionTypes[] = {
    { KReportSection::Type::Invalid, "" },
    { KReportSection::Type::PageHeaderAny, "header-page-any" },
    { KReportSection::Type::PageHeaderEven, "header-page-even" },
    { KReportSection::Type::PageHeaderOdd, "header-page-odd" },
    { KReportSection::Type::PageHeaderFirst, "header-page-first" },
    { KReportSection::Type::PageHeaderLast, "header-page-last" },
    { KReportSection::Type::PageFooterAny, "footer-page-any" },
    { KReportSection::Type::PageFooterEven, "footer-page-even" },
    { KReportSection::Type::PageFooterOdd, "footer-page-odd" },
    { KReportSection::Type::PageFooterFirst, "footer-page-first" },
    { KReportSection::Type::PageFooterLast, "footer-page-last" },
    { KReportSection::Type::ReportHeader, "header-report" },
    { KReportSection::Type::ReportFooter, "footer-report" },
    { KReportSection::Type::GroupHeader, "group-header" },
    { KReportSection::Type::GroupFooter, "group-footer" },
    { KReportSection::Type::Detail, "detail" },
    { KReportSection::Type::Invalid, nullptr }
};

Q_GLOBAL_STATIC(KReportDesignGlobal, s_global)

//static
KReportDesignGlobal* KReportDesignGlobal::self()
{
    return s_global;
}

static void setStatus(KReportDesignReadingStatus *status, const QString& details,
                      const QDomNode &node)
{
    if (status) {
        status->setErrorDetails(details);
        status->setErrorLineNumber(node.lineNumber() == -1 ? 0 /* mark error */ : node.lineNumber());
        status->setErrorColumnNumber(node.columnNumber() == -1 ? 0 /* mark error */ : node.columnNumber());
    }
}

static bool checkElement(const QDomNode &node, KReportDesignReadingStatus *status)
{
    if (node.isElement()) {
        return true;
    }
    setStatus(status, QString::fromLatin1("Element expected inside of <%1>")
              .arg(node.parentNode().toElement().tagName()), node);
    return false;
}

static void setNoAttributeStatus(const QDomElement &el, const char *attrName, KReportDesignReadingStatus *status)
{
    setStatus(status, QString::fromLatin1("Attribute \"%1\" expected inside of <%1>")
              .arg(QLatin1String(attrName)).arg(el.tagName()), el);
}

#if 0 // TODO unused for now
static bool checkAttribute(const QDomElement &el, const char *attrName, KReportDesignReadingStatus *status)
{
    if (el.hasAttribute(QLatin1String(attrName))) {
        return true;
    }
    setNoAttributeStatus(el, attrName, status);
    return false;
}
#endif

KReportSection KReportDesign::Private::processSectionElement(const QDomElement &el,
                                                             KReportDesignReadingStatus *status)
{
    const QString sectionTypeName = KReportUtils::attr(el, "report:section-type", QString());
    KReportSection::Type sectionType = s_global->sectionType(sectionTypeName);
    if (sectionType == KReportSection::Type::Invalid) {
        setStatus(status,
            QString::fromLatin1("Invalid value of report:section-type=\"%1\" in element <%2>")
                                .arg(sectionTypeName).arg(el.tagName()), el);
        return KReportSection();
    }
    KReportSection section;
    section.setType(sectionType);
    qreal height = KReportUtils::attr(el, "svg:height", -1.0);
    if (height >= 0.0) {
        section.setHeight(height);
    }
    section.setBackgroundColor(QColor(KReportUtils::attr(el, "fo:background-color", QString())));
    for (QDomNode node = el.firstChild(); !node.isNull(); node = node.nextSibling()) {
        if (!checkElement(node, status)) {
            return KReportSection();
        }
        KReportElement element = processSectionElementChild(node.toElement(), status);
        if (!element.rect().isValid() || (status && status->isError())) {
            return KReportSection();
        }
        (void)section.addElement(element);
    }
    return section;
}

KReportPluginInterface* KReportDesign::Private::findPlugin(const QString &typeName,
                                                            const QDomElement &el,
                                                            KReportDesignReadingStatus *status)
{
    KReportPluginInterface* plugin = KReportPluginManager::self()->plugin(typeName);
    if (!plugin) {
        setStatus(status, QString::fromLatin1("No such plugin \"%1\"").arg(typeName), el);
        return nullptr;
    }
    return plugin;
}

KReportElement KReportDesign::Private::processSectionElementChild(const QDomElement &el,
                                                                  KReportDesignReadingStatus *status)
{
    const QByteArray name = el.tagName().toLatin1();
    const char* elNamespace = "report:";

    if (!name.startsWith(elNamespace)) {
        unexpectedElement(el, status);
        return KReportElement();
    }
    const QByteArray reportElementName = name.mid(qstrlen(elNamespace));
    //qDebug() << "Found Report Element:" << reportElementName;
    KReportPluginInterface *plugin = findPlugin(QLatin1String(reportElementName), el, status);
    if (!plugin) {
        return KReportElement();
    }
    KReportElement element = plugin->createElement();
    if (!plugin->loadElement(&element, el, status)) {
        return KReportElement();
    }
    element.setName(KReportUtils::attr(el, "report:name", QString()));
    if (element.name().isEmpty()) {
        setNoAttributeStatus(el, "report:name", status);
        return KReportElement();
    }
    return element;
}

bool KReportDesign::Private::processGroupElement(const QDomElement &el,
                                                 KReportDesignReadingStatus *status)
{
    Q_UNUSED(el);
    Q_UNUSED(status);
    //! @todo
    return true;
}

//! The report:detail element contains a single report:section child of type 'detail'
//! and 0 or more report:group children.
bool KReportDesign::Private::processDetailElement(const QDomElement &el,
                                                  KReportDesignReadingStatus *status)
{
    QDomElement sectionEl;
    for (QDomNode node = el.firstChild(); !node.isNull(); node = node.nextSibling()) {
        if (!checkElement(node, status)) {
            return false;
        }
        QDomElement childEl = node.toElement();
        const QByteArray name = childEl.tagName().toLatin1();
        if (name == "report:section") {
            if (!sectionEl.isNull()) {
                return false;
            }
            KReportSection section = processSectionElement(childEl, status);
            if (status && status->isError()) {
                return false;
            }
            if (section.type() != KReportSection::Type::Detail) {
                setStatus(status,
                    QString::fromLatin1("Only section of type \"detail\" allowed in <report:detail>"), el);
                return false;
            }
            q->addSection(section);
        }
        else if (name == "report:group") {
            if (!processGroupElement(childEl, status)) {
                return false;
            }
        }
        else {
            unexpectedElement(childEl, status);
            return false;
        }
    }
    // finally make sure we have one report:section
    (void)requiredChildElement(el, "report:section", status);
    if (status && status->isError()) {
        return false;
    }
    return true;
}

/*! <pre>
       <report:body>
        *<report:section>..</report:section> (up to 12 sections)
         <report:detail>
           <report:group> // any number of groups
             *<report:section>..</report:section> (group-header, group-footer)
           </report:group>
         </report:detail>
       <report:body>
    <pre>
*/
bool KReportDesign::Private::processBodyElementChild(const QDomElement &el,
                                                     KReportDesignReadingStatus *status)
{
    const QByteArray name = el.tagName().toLatin1();
    //kreportDebug() << name;
    if (name == "report:section") {
        KReportSection section = processSectionElement(el, status);
        if (status && status->isError()) {
            return false;
        }
        if (q->hasSection(section.type())) {
            setStatus(status, QString::fromLatin1("Could not add two sections of type \"%1\" "
                                                  "to the same report design")
                                .arg(s_global->sectionTypeName(section.type())), el);
            return false;
        }
        if (section.type() == KReportSection::Type::Detail) {
            setStatus(status,
                QString::fromLatin1("Section of type \"detail\" not allowed in <report:body>"), el);
            return false;
        }
        q->addSection(section);
#if 0 //TODO
        if (section(KReportSectionData::sectionTypeFromString(sectiontype)) == 0) {
            insertSection(KReportSectionData::sectionTypeFromString(sectiontype));
            section(KReportSectionData::sectionTypeFromString(sectiontype))->initFromXML(sec);
        }
#endif
    } else if (name == "report:detail") {
        if (!processDetailElement(el, status)) {
            return false;
        }
#if 0 //TODO
        ReportSectionDetail * rsd = new ReportSectionDetail(this);
        rsd->initFromXML(&sec);
        setDetail(rsd);
#endif
    }
    return true;
}

/* NOTE: don't translate these extremely detailed messages. */
//! @todo Load page options
bool KReportDesign::Private::processContentElementChild(const QDomElement &el,
                                                        KReportDesignReadingStatus *status)
{
    const QByteArray name = el.tagName().toLatin1();
    QPageLayout defaultPageLayout = KReportDesign::defaultPageLayout();
    //kreportDebug() << name;
    if (name == "report:title") {
        title = el.text();
#ifdef KREPORT_SCRIPTING
    } else if (name == "report:script") {
        script = el.firstChildElement().text();
        originalInterpreter = KReportUtils::attr(el, "report:script-interpreter", QString());
#endif
    } else if (name == "report:grid") {
        showGrid = KReportUtils::attr(el, "report:grid-visible", DEFAULT_SHOW_GRID);
        snapToGrid = KReportUtils::attr(el, "report:grid-snap", DEFAULT_SNAP_TO_GRID);
        gridDivisions = KReportUtils::attr(el, "report:grid-divisions", DEFAULT_GRID_DIVISIONS);
        const QString pageUnitString = KReportUtils::attr(el, "report:page-unit", QString());
        pageUnit = KReportUnit(KReportUnit::symbolToType(pageUnitString));
        if (!pageUnit.isValid()) {
            pageUnit = DEFAULT_UNIT;
            if (!pageUnitString.isEmpty()) {
                qWarning() << "Invalid page unit" << pageUnitString << "specified in" << name
                           << "element, defaulting to" << pageUnit.symbol();
            }
        }
    }
    else if (name == "report:page-style") { // see https://git.reviewboard.kde.org/r/115314
        const QByteArray pagetype = el.text().toLatin1();
        if (pagetype == "predefined") {
            pageLayout.setPageSize(
                        KReportUtils::pageSize(KReportUtils::attr(el, "report:page-size",
                                               QPageSize(DEFAULT_PAGE_SIZE).key())));
        } else if (pagetype.isEmpty() || pagetype == "custom") {
            QSizeF size(KReportUtils::attr(el, "fo:page-width", -1.0),
                        KReportUtils::attr(el, "fo:page-height", -1.0));
            if (size.isValid()) {
                pageLayout.setPageSize(QPageSize(size, QPageSize::Point));
            } else {
                pageLayout.setPageSize(defaultPageLayout.pageSize());
            }
        } else if (pagetype == "label") {
            //! @todo?
            pageLayout.setPageSize(defaultPageLayout.pageSize());
        }
        QMarginsF margins(KReportUtils::attr(el, "fo:margin-left", defaultPageLayout.margins().left()),
                 KReportUtils::attr(el, "fo:margin-top", defaultPageLayout.margins().top()),
                 KReportUtils::attr(el, "fo:margin-right", defaultPageLayout.margins().right()),
                 KReportUtils::attr(el, "fo:margin-bottom", defaultPageLayout.margins().bottom()));
        bool b = pageLayout.setMargins(margins);
        if (!b) {
            qWarning() << "Failed to set page margins to" << margins;
        }
        const QString s = KReportUtils::attr(el, "report:print-orientation", QString());
        if (s == QLatin1String("portrait")) {
            pageLayout.setOrientation(QPageLayout::Portrait);
        } else if (s == QLatin1String("landscape")) {
            pageLayout.setOrientation(QPageLayout::Landscape);
        }
        else {
            pageLayout.setOrientation(defaultPageLayout.orientation());
        }
    } else if (name == "report:body") {
        for (QDomNode node = el.firstChild(); !node.isNull(); node = node.nextSibling()) {
            if (!checkElement(node, status)) {
                return false;
            }
            if (!processBodyElementChild(node.toElement(), status)) {
                return false;
            }
        }
    }
    return true;
}

void KReportDesign::Private::unexpectedElement(const QDomElement &element,
                                               KReportDesignReadingStatus *status) const
{
    setStatus(status, QString::fromLatin1("Unexpected child element <%1> found in <%2>")
          .arg(element.tagName()).arg(element.parentNode().toElement().tagName()), element);
}

QDomElement KReportDesign::Private::requiredChildElement(const QDomElement &parent,
                                                         const char* childElementName,
                                                         KReportDesignReadingStatus *status) const
{
    const QDomElement result = parent.firstChildElement(QLatin1String(childElementName));
    if (result.isNull()) {
        setStatus(status, QString::fromLatin1("Child element <%1> not found in <%2>")
              .arg(QLatin1String(childElementName)).arg(parent.tagName()), parent);
    }
    return result;
}

/* NOTE: don't translate these extremely detailed messages. */
bool KReportDesign::Private::processDocument(const QDomDocument &doc,
                                             KReportDesignReadingStatus *status)
{
    const QDomElement rootEl = doc.documentElement();
    const QLatin1String rootElName("kexireport"); // legacy name kept for compatibility
    if (doc.doctype().name() != rootElName) {
        setStatus(status, QString::fromLatin1("Document type should be \"%1\"").arg(rootElName), rootEl);
        return false;
    }
    if (rootEl.tagName() != rootElName) {
        setStatus(status, QString::fromLatin1("Root element should be <%1>").arg(rootElName), rootEl);
        return false;
    }
    const QDomElement contentEl = requiredChildElement(rootEl, "report:content", status);
    if (status && status->isError()) {
        return false;
    }
    //! @todo check namespaces as in:
    //! <report:content xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"
    //!     xmlns:report="http://kexi-project.org/report/2.0"
    //!     xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0">

//    deleteDetail();

    for (QDomNode node = contentEl.firstChild(); !node.isNull(); node = node.nextSibling()) {
        if (!checkElement(node, status)) {
            return false;
        }
        if (!processContentElementChild(node.toElement(), status)) {
            return false;
        }
    }

    if (status) {
        *status = KReportDesignReadingStatus();
    }
    return true;
}
