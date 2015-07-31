/* This file is part of the KDE project
 * Copyright (C) 2012 Dag Andersen <danders@get2net.dk>
 * Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>
 *
 * QFUZZYCOMPARE() from marble/tests/TestUtils.h:
 * Copyright (C) 2013 Dennis Nienhüser <earthwings@gentoo.org>
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

#include "FormatTest.h"
#include "KoReportPreRenderer.h"
#include "KoReportDesigner.h"
#include "KReportLabelElement.h"
#include "krreportdata.h"
#include "reportpageoptions.h"
#include "reportsectiondetail.h"
#include "reportsection.h"
#include "krpos.h"
#include "krsize.h"
#include "KoReportDesignerItemLine.h"
#include "KoReportItemLine.h"
#include "KoReportDesignerItemRectBase.h"
#include "KReportUnit.h"
#include <KReportDpi>
#include <KReportDesign>

#include <QPointF>
#include <QSizeF>
#include <QDomDocument>
#include <QScreen>
#include <QDir>
#include <QTest>
#include <QPageLayout>

QTEST_MAIN(FormatTest)

namespace QTest
{
bool qCompare(qreal val1, qreal val2, qreal epsilon, const char *actual, const char *expected,
              const char *file, int line)
{
    return (qAbs( val1 - val2 ) < epsilon)
        ? compare_helper(true, "COMPARE()", toString(val1), toString(val2), actual,
                         expected, file, line)
        : compare_helper(false, "Compared qreals are not the same", toString(val1),
                         toString( val2 ), actual, expected, file, line);
}
}

#define QFUZZYCOMPARE(actual, expected, epsilon) \
do {\
    if (!QTest::qCompare(actual, expected, epsilon, #actual, #expected, __FILE__, __LINE__))\
        return;\
} while (0)

void FormatTest::testPageOptions()
{
    return;
    QString s;
    s += "<!DOCTYPE kexireport>\n";
    s += "<kexireport>\n";
    s += "<report:content xmlns:report=\"http://kexi-project.org/report/2.0\" xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" >\n";
    s += "<report:title>Report</report:title>\n";
    s += "<report:grid report:grid-divisions=\"4\" report:grid-snap=\"1\" report:page-unit=\"cm\" report:grid-visible=\"1\" />\n";
    s += "<report:page-style report:print-orientation=\"portrait\" fo:margin-bottom=\"1.5cm\" fo:margin-top=\"2.0cm\" fo:margin-left=\"3.0cm\" fo:margin-right=\"4.0cm\" report:page-size=\"A5\" >predefined</report:page-style>\n";
    // needs detail section, or else designer crash
    s += "<report:body>\n";
    s += "<report:detail>\n";
    s += "<report:section svg:height=\"5.0cm\" fo:background-color=\"#ffffff\" report:section-type=\"detail\"/>\n";
    s += "</report:detail>\n";
    s += "</report:body>\n";
    s += "</kexireport>\n";

    QDomDocument doc;
    doc.setContent( s );
    KoReportDesigner designer(0, doc.documentElement());
    QCOMPARE(designer.propertySet()->property("page-size").value().toString(), QLatin1String("A5"));
    QCOMPARE(designer.propertySet()->property("margin-bottom").value().toDouble(), KReportUnit::parseValue("1.5cm"));
    QCOMPARE(designer.propertySet()->property("margin-top").value().toDouble(), KReportUnit::parseValue("2.0cm"));
    QCOMPARE(designer.propertySet()->property("margin-left").value().toDouble(), KReportUnit::parseValue("3.0cm"));
    QCOMPARE(designer.propertySet()->property("margin-right").value().toDouble(), KReportUnit::parseValue("4.0cm"));

    KoReportPreRenderer renderer( designer.document() );
    renderer.generate();
    ReportPageOptions opt = renderer.reportData()->pageOptions();


    QCOMPARE(opt.getPageSize(), QString("A5"));
    QScreen *srn = QApplication::screens().at(0);
    const qreal dpiY = srn->logicalDotsPerInchY();
    qDebug() << opt.getMarginBottom() << INCH_TO_POINT(opt.getMarginBottom()) << KReportDpi::dpiY() << dpiY << KReportUnit::parseValue("1.5cm");
    QFUZZYCOMPARE(INCH_TO_POINT(opt.getMarginBottom()) / KReportDpi::dpiY(), KReportUnit::parseValue("1.5cm"), 0.2);
    QFUZZYCOMPARE(INCH_TO_POINT(opt.getMarginTop()) / KReportDpi::dpiY(), KReportUnit::parseValue("2.0cm"), 0.2);
    QFUZZYCOMPARE(INCH_TO_POINT(opt.getMarginLeft()) / KReportDpi::dpiX(), KReportUnit::parseValue("3.0cm"), 0.2);
    QFUZZYCOMPARE(INCH_TO_POINT(opt.getMarginRight()) / KReportDpi::dpiX(), KReportUnit::parseValue("4.0cm"), 0.3);
}

void FormatTest::testLineItem()
{
    QString s;
    s += "<report:content xmlns:report=\"http://kexi-project.org/report/2.0\"";
    s += " xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\"";
    s += " xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" >";
    s += "<report:title>Report</report:title>";
    s += "<report:grid report:grid-divisions=\"4\" report:grid-snap=\"1\" report:page-unit=\"cm\" report:grid-visible=\"1\" />";
    s += "<report:page-style report:print-orientation=\"portrait\"";
    s += " fo:margin-bottom=\"1.5cm\" fo:margin-top=\"2.0cm\"";
    s += " fo:margin-left=\"3.0cm\" fo:margin-right=\"4.0cm\"";
    s += " report:page-size=\"A5\" >predefined</report:page-style>";

    s += "<report:body>";
    s += "<report:detail>";
    s += "<report:section svg:height=\"5.0cm\" fo:background-color=\"#ffffff\" report:section-type=\"detail\">";
    s += "<report:line report:name=\"line1\" ";
    s += "svg:y1=\"0.5cm\" svg:x1=\"1.5cm\" svg:y2=\"2.5cm\" svg:x2=\"4.5cm\" report:z-index=\"1.5\">";
    s += "<report:line-style report:line-style=\"solid\" report:line-weight=\"2\" report:line-color=\"#000000\"/>";
    s += "</report:line>";
    s += "</report:section>";
    s += "</report:detail>";
    s += "</report:body>";

    QDomDocument doc;
    doc.setContent( s );
    KoReportDesigner designer(0, doc.documentElement());
    ReportSectionDetail *ds = designer.detailSection();
    ReportSection *sec = ds->detailSection();
    KoReportItemLine *l = dynamic_cast<KoReportItemLine*>(sec->items().first());

    QVERIFY(l != 0);
    QCOMPARE(l->Z, 1.5);
    KRPos start = l->startPosition();
    KRPos end = l->endPosition();
    QCOMPARE(start.toPoint(), QPointF(KReportUnit::parseValue("1.5cm"), KReportUnit::parseValue("0.5cm")));
    QCOMPARE(end.toPoint(), QPointF(KReportUnit::parseValue("4.5cm"), KReportUnit::parseValue("2.5cm")));

    KoReportPreRenderer renderer( designer.document() );
    renderer.generate();
    l = dynamic_cast<KoReportItemLine*>(renderer.reportData()->object("line1"));

    QVERIFY(l != 0);
    QCOMPARE(l->Z, 1.5);
    start = l->startPosition();
    end = l->endPosition();
    QCOMPARE(start.toPoint(), QPointF(KReportUnit::parseValue("1.5cm"), KReportUnit::parseValue("0.5cm")));
    QCOMPARE(end.toPoint(), QPointF(KReportUnit::parseValue("4.5cm"), KReportUnit::parseValue("2.5cm")));
}

void FormatTest::testRectItem()
{
    const QString dir(QFile::decodeName(FILES_DATA_DIR));
    const QString fname(QLatin1String(QTest::currentTestFunction()) + QLatin1String(".kreport"));
    QFile file(dir + QDir::separator() + fname);
    bool ok = file.open(QFile::ReadOnly | QFile::Text);
    if (!ok) {
        QFAIL(qPrintable(QString::fromLatin1("Could not open file %1: ").arg(file.fileName()) + file.errorString()));
    }

    QString content = file.readAll();
    QVERIFY2(file.error() == QFileDevice::NoError,
        qPrintable(QString::fromLatin1("Error reading file %1: ").arg(file.fileName()) + file.errorString()));

    KReportDesign design;
    KReportDesignReadingStatus status;
    if (!design.setContent(content, &status)) {
        QString message;
        QDebug(&message) << status;
        QFAIL(qPrintable(QLatin1String("Failed to load content. ") + message));
    }
    QCOMPARE(design.title(), QLatin1String("RectItem Test Report"));

    qDebug()<<design.pageLayout();
    const QPageLayout pageLayout = design.pageLayout();
    QVERIFY(pageLayout.isValid());
    QCOMPARE(pageLayout.pageSize().id(), QPageSize::A5);
    QCOMPARE(pageLayout.pageSize().sizePoints(), QPageSize(QPageSize::A5).sizePoints());
    QCOMPARE(pageLayout.orientation(), QPageLayout::Portrait);
    QCOMPARE(pageLayout.margins(QPageLayout::Millimeter), QMarginsF(30.0, 20.0, 40.0, 15.0));

    QVERIFY(design.hasSection(KReportSection::Detail));
    KReportSection detailSection = design.section(KReportSection::Detail);
    QCOMPARE(detailSection.type(), KReportSection::Detail);
    QCOMPARE(detailSection.height(), CM_TO_POINT(5.0));
    QCOMPARE(detailSection.backgroundColor(), QColor("#eeeeee"));

    QList<KReportElement> elements = detailSection.elements();
    QCOMPARE(elements.count(), 1);
    KReportElement element = elements.first();
    QCOMPARE(element.name(), QLatin1String("label1"));

    //TODO: move to an elements API test
    KReportElement e;
    e.setName("foo");

    KReportElement e2 = e;
    qDebug() << e2.name();
    e.setName("");
    qDebug() << e2.name();
    KReportLabelElement l1, l2;
    l1.setText("text");
    l2 = l1.clone();
    l1.setText("");
    qDebug() << "cloned:" << l2.text();
    e = l2;
    l2.setText("text");
    KReportLabelElement l3(e);
    qDebug() << l3.text() << KReportLabelElement(e).text() << KReportLabelElement(e2).text();
    KReportElement ee = e.clone();
    qDebug() << "KReportLabelElement(e).text():" << KReportLabelElement(e).text();

    qDebug() << element.name();
    detailSection.elements().first().setName("new_label_name");

    detailSection.addElement(e);
    qDebug() << KReportLabelElement(detailSection.elements().at(1)).text();
    qDebug() << detailSection.elements().at(0).name() << element.name();
    // end of TODO

    //TODO: move this renderer test to a separate place
    KoReportDesigner designer(0);//, doc.documentElement());
    ReportSectionDetail *ds = designer.detailSection();
    ReportSection *sec = ds->detailSection();
    QVERIFY(sec->items().count() == 1);
    KoReportDesignerItemRectBase *rect = dynamic_cast<KoReportDesignerItemRectBase*>(sec->items().first());

    QVERIFY(rect != 0);
    QRectF expected( QPointF(KReportUnit::parseValue("1.5cm"), KReportUnit::parseValue("0.5cm")), QSizeF(KReportUnit::parseValue("4.5cm"), KReportUnit::parseValue("0.75cm")));
    QCOMPARE(rect->pointRect(), expected);

    KoReportPreRenderer renderer( designer.document() );
    renderer.generate();
    KoReportItemBase *item = dynamic_cast<KoReportItemBase*>(renderer.reportData()->object("label1"));

    QVERIFY(item != 0);
    KRPos pos = item->position();
    KRSize size = item->size();
    QCOMPARE(pos.toPoint().x(), KReportUnit::parseValue("1.5cm"));
    QCOMPARE(pos.toPoint().y(), KReportUnit::parseValue("0.5cm"));
    QCOMPARE(size.toPoint(), QSizeF(KReportUnit::parseValue("4.5cm"), KReportUnit::parseValue("0.75cm")));
    QCOMPARE(size.toPoint(), QSizeF(KReportUnit::parseValue("4.5cm"), KReportUnit::parseValue("0.75cm")));
}
