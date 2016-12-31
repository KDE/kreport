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

#include "KReportHTMLTableRenderer_p.h"
#include "KReportRenderObjects.h"

#include <QTemporaryFile>

#include "kreport_debug.h"
#include <QDir>
#include <QPainter>
#include <QString>

namespace KReportPrivate {

HTMLTableRenderer::HTMLTableRenderer()
{
}

HTMLTableRenderer::~HTMLTableRenderer()
{

}

bool HTMLTableRenderer::render(const KReportRendererContext& context, ORODocument *document, int page)
{
    Q_UNUSED(page);
    QTemporaryFile tempHtmlFile; // auto removed by default on destruction
    if (!tempHtmlFile.open()) {
        kreportWarning() << "Couldn't create temporary file to write into";
        return false;
    }

    QTextStream out(&tempHtmlFile);

    QString dirSuffix = QLatin1String(".files");
    QDir tempDir;
    QFileInfo fi(tempHtmlFile);

    QString tempFileName = fi.absoluteFilePath();
    m_tempDirName = tempFileName + dirSuffix;
    m_actualDirName = context.url().fileName() + dirSuffix;

    if (!tempDir.mkpath(m_tempDirName))
        return false;

    out << renderTable(document);

    out.flush();
    tempHtmlFile.close();

    bool status = true; //! @todo port KIO;
//    if (KIO::NetAccess::upload(tempFileName, context.destinationUrl, 0) && KIO::NetAccess::dircopy(QUrl(m_tempDirName),  QUrl(context.destinationUrl.url() + dirSuffix), 0)) {
//        status = true;
//    }

    // cleanup the temporary directory
    tempDir.setPath(m_tempDirName);
    QStringList fileList = tempDir.entryList();
    foreach(const QString& fileName, fileList) {
        tempDir.remove(fileName);
    }
    tempDir.rmdir(m_tempDirName);

    return status;
}

QString HTMLTableRenderer::renderTable(ORODocument *document)
{
    QString html;
    QString body;
    QString tr;

    bool renderedPageHeader = false;
    bool renderedPageFooter = false;

    QDir d(m_tempDirName);

    // Render Each Section
    body = QLatin1String("<table>\n");
    for (int s = 0; s < document->sectionCount(); s++) {
        OROSection *section = document->section(s);
        section->sortPrimitives(Qt::Horizontal);

        if (section->type() == KReportSectionData::GroupHeader ||
                section->type() == KReportSectionData::GroupFooter ||
                section->type() == KReportSectionData::Detail ||
                section->type() == KReportSectionData::ReportHeader ||
                section->type() == KReportSectionData::ReportFooter ||
                (section->type() == KReportSectionData::PageHeaderAny && !renderedPageHeader) ||
                (section->type() == KReportSectionData::PageFooterAny && !renderedPageFooter && s > document->sectionCount() - 2)) { //render the page foot right at the end, it will either be the last or second last section if there is a report footer
            if (section->type() == KReportSectionData::PageHeaderAny)
                renderedPageHeader = true;

            if (section->type() == KReportSectionData::PageFooterAny)
                renderedPageFooter = true;

            tr = QLatin1String("<tr style=\"background-color: ") + section->backgroundColor().name() + QLatin1String("\">\n");
            //Render the objects in each section
            for (int i = 0; i < section->primitiveCount(); i++) {
                OROPrimitive * prim = section->primitive(i);

                if (OROTextBox *tb = dynamic_cast<OROTextBox*>(prim)) {
                    tr += QLatin1String("<td>") +
                          tb->text() +
                          QLatin1String("</td>\n");
                } else if (OROImage *im  = dynamic_cast<OROImage*>(prim)) {
                    tr += QLatin1String("<td>"
                          "<img src=\"./") + m_actualDirName + QLatin1String("/object") + QString::number(s) + QString::number(i) + QLatin1String(".png\"></img>"
                          "</td>\n");
                    im->image().save(m_tempDirName + QLatin1String("/object") + QString::number(s) + QString::number(i) + QLatin1String(".png"));
                } else if (OROPicture *im = dynamic_cast<OROPicture*>(prim)) {
                    tr += QLatin1String("<td>"
                          "<img src=\"./") + m_actualDirName + QLatin1String("/object") + QString::number(s) + QString::number(i) + QLatin1String(".png\"></img>"
                          "</td>\n");
                    QImage image(im->size().toSize(), QImage::Format_RGB32);
                    QPainter painter(&image);
                    im->picture()->play(&painter);
                    image.save(m_tempDirName + QLatin1String("/object") + QString::number(s) + QString::number(i) + QLatin1String(".png"));
                } else {
                    kreportWarning() << "unhandled primitive type";
                }
            }
            tr += QLatin1String("</tr>\n");

            if (tr.contains(QLatin1String("<td>"))) {
                body += tr;
            }
        }
    }
    body += QLatin1String("</table>\n");
    html = QLatin1String("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
        "<html>\n"
        "<head>\n"
        "<title>")
      + document->title()
      + QLatin1String("</title>\n"
        "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">\n"
        "<meta name=\"generator\" content=\"Kexi\">\n"
        "</head>\n"
        "<body>\n")
      + body
      + QLatin1String("\n</body>\n"
        "</html>\n");

    return html;
}

}

