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

#include "KoReportHTMLCSSRenderer.h"
#include "common/renderobjects.h"

#include <QTemporaryFile>

#include <QDir>
#include <QPainter>
#include <QString>
#include <QDebug>

KoReportHTMLCSSRenderer::KoReportHTMLCSSRenderer()
{

}

KoReportHTMLCSSRenderer::~KoReportHTMLCSSRenderer()
{

}

bool KoReportHTMLCSSRenderer::render(const KoReportRendererContext& context, ORODocument *document, int page)
{
    Q_UNUSED(page);
    QTemporaryFile tempHtmlFile; // auto removed by default on destruction
    if (!tempHtmlFile.open()) {
        qWarning() << "Couldn't create temporary file to write into";
        return false;
    }

    QTextStream out(&tempHtmlFile);

    QString dirSuffix = QLatin1String(".files");
    QDir tempDir;
    QFileInfo fi(tempHtmlFile);

    QString tempFileName = fi.absoluteFilePath();
    m_tempDirName = tempFileName + dirSuffix;
    m_actualDirName = context.destinationUrl.fileName() + dirSuffix;

    if (!tempDir.mkpath(m_tempDirName))
        return false;

    out << renderCSS(document);

    out.flush();
    tempHtmlFile.close();

    bool status = true; //TODO KIO
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

//! @todo use QTextStream for efficiency
QString KoReportHTMLCSSRenderer::renderCSS(ORODocument *document)
{
    QString html;
    QString body;
    QString style;
    QStringList styles;
    int styleindex;
    bool renderedPageHead = false;
    bool renderedPageFoot = false;

    QDir d(m_tempDirName);
    // Render Each Section
    for (long s = 0; s < document->sections(); s++) {
        OROSection *section = document->section(s);

        if (section->type() == KRSectionData::GroupHeader ||
                section->type() == KRSectionData::GroupFooter ||
                section->type() == KRSectionData::Detail ||
                section->type() == KRSectionData::ReportHeader ||
                section->type() == KRSectionData::ReportFooter ||
                (section->type() == KRSectionData::PageHeaderAny && !renderedPageHead) ||
                (section->type() == KRSectionData::PageFooterAny && !renderedPageFoot && s > document->sections() - 2)) { //render the page foot right at the end, it will either be the last or second last section if there is a report footer
            if (section->type() == KRSectionData::PageHeaderAny)
                renderedPageHead = true;

            if (section->type() == KRSectionData::PageFooterAny)
                renderedPageFoot = true;

            style = QLatin1String("position: relative; top: 0pt; left: 0pt; background-color: ") + section->backgroundColor().name() + QLatin1String("; height: ") + QString::number(section->height()) + QLatin1String("pt;");

            if (!styles.contains(style)) {
                styles << style;
            }
            styleindex = styles.indexOf(style);

            body += QLatin1String("<div class=\"style") + QString::number(styleindex) + QLatin1String("\">\n");
            //Render the objects in each section
            for (int i = 0; i < section->primitives(); i++) {
                OROPrimitive * prim = section->primitive(i);
                //qDebug() << "Got object type" << prim->type();
                if (prim->type() == OROTextBox::TextBox) {
                    OROTextBox * tb = (OROTextBox*) prim;

                    QColor bg = tb->textStyle().backgroundColor;
                    style = QLatin1String("position: absolute; ") +
                            QLatin1String("background-color: ") + QString::fromLatin1("rgba(%1,%2,%3,%4)")
                                                            .arg(bg.red())
                                                            .arg(bg.green())
                                                            .arg(bg.blue())
                                                            .arg(0.01 * tb->textStyle().backgroundOpacity) +QLatin1String( "; ") +
                            QLatin1String("top: ") + QString::number(tb->position().y()) + QLatin1String("pt; ") +
                            QLatin1String("left: ") + QString::number(tb->position().x()) + QLatin1String("pt; ") +
                            QLatin1String("font-size: ") + QString::number(tb->textStyle().font.pointSize()) + QLatin1String("pt; ") +
                            QLatin1String("color: ") + tb->textStyle().foregroundColor.name() + QLatin1String("; ") +
                            QLatin1String("width: ") + QString::number(tb->size().width()) + QLatin1String("px;") +
                            QLatin1String("height: ") + QString::number(tb->size().height()) + QLatin1String("px;") ;
                    //TODO opaque text + translucent background
                    //it looks a pain to implement
                    //http://developer.mozilla.org/en/docs/Useful_CSS_tips:Color_and_Background
                    //style += "filter:alpha(opacity=" + QString::number((tb->textStyle().bgOpacity / 255) * 100) + ");"; //ie opacity
                    //style += "opacity: " + QString::number(tb->textStyle().bgOpacity / 255.0) + ";";

                    if (!styles.contains(style)) {
                        styles << style;
                    }
                    styleindex = styles.indexOf(style);

                    body += QLatin1String("<div class=\"style") + QString::number(styleindex) + QLatin1String("\">") +
                            tb->text() +
                            QLatin1String("</div>\n");
                } else if (prim->type() == OROImage::Image) {
                    //qDebug() << "Saving an image";
                    OROImage * im = (OROImage*) prim;
                    style = QLatin1String("position: absolute; ") +
                            QLatin1String("top: ") + QString::number(im->position().y()) + QLatin1String("pt; ") +
                            QLatin1String("left: ") + QString::number(im->position().x()) + QLatin1String("pt; ");
                    if (!styles.contains(style)) {
                        styles << style;
                    }
                    styleindex = styles.indexOf(style);

                    body += QLatin1String("<div class=\"style") + QString::number(styleindex) + QLatin1String("\">") +
                            QLatin1String("<img width=\"") + QString::number(im->size().width()) + QLatin1String("px") + QLatin1String("\" height=\"") + QString::number(im->size().height()) + QLatin1String("px") + QLatin1String("\" src=\"./") + m_actualDirName + QLatin1String("/object") + QString::number(s) + QString::number(i) + QLatin1String(".png\"></img>") +
                            QLatin1String("</div>\n");


                    im->image().save(m_tempDirName + QLatin1String("/object") + QString::number(s) + QString::number(i) + QLatin1String(".png"));
                } else if (prim->type() == OROPicture::Picture) {
                    //qDebug() << "Saving a picture";
                    OROPicture * im = (OROPicture*) prim;
                    style = QLatin1String("position: absolute; ") +
                            QLatin1String("top: ") + QString::number(im->position().y()) + QLatin1String("pt; ") +
                            QLatin1String("left: ") + QString::number(im->position().x()) + QLatin1String("pt; ");
                    if (!styles.contains(style)) {
                        styles << style;
                    }
                    styleindex = styles.indexOf(style);

                    body += QLatin1String("<div class=\"style") + QString::number(styleindex) + QLatin1String("\">") +
                            QLatin1String("<img width=\"") + QString::number(im->size().width()) + QLatin1String("px") + QLatin1String("\" height=\"") + QString::number(im->size().height()) + QLatin1String("px") + QLatin1String("\" src=\"./") + m_actualDirName + QLatin1String("/object") + QString::number(s) + QString::number(i) + QLatin1String(".png\"></img>") +
                            QLatin1String("</div>\n");

                    QImage image(im->size().toSize(), QImage::Format_RGB32);
                    QPainter painter(&image);
                    im->picture()->play(&painter);
                    image.save(m_tempDirName + QLatin1String("/object") + QString::number(s) + QString::number(i) + QLatin1String(".png"));
                } else {
                    qWarning() << "unrecognized primitive type" << prim->type();
                }
            }
            body += QLatin1String("</div>\n");
        }
    }

    //! @todo add option for creating separate css file
    html = QLatin1String("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
        "<html>\n"
        "<head>\n"
        "<style type=\"text/css\">\n");

    for (int i = 0; i < styles.count(); ++i) {
        html += QLatin1String(".style") + QString::number(i) + QLatin1String("{") + styles[i] + QLatin1String("}\n");
    }

    html += QLatin1String("</style>\n") +
        QLatin1String("<title>") + document->title() + QLatin1String("</title>\n") +
        QLatin1String("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">\n") +
        QLatin1String("<meta name=\"generator\" content=\"Kexi\">\n") +
        QLatin1String("</head>\n") +
        QLatin1String("<body>\n") +
        body +
        QLatin1String("\n</body>\n") +
        QLatin1String("</html>\n");

    return html;
}
