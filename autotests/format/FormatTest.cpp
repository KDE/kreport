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
#include "KReportTestUtils.h"
#include "KReportPreRenderer.h"
#include "KReportDesigner.h"
#include "KReportLabelElement.h"
#include "KReportDocument.h"
#include "KReportDesignerSectionDetail.h"
#include "KReportSection.h"
#include "KReportDesignerItemLine.h"
#include "KReportItemLine.h"
#include "KReportDesignerItemRectBase.h"
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

static bool openDesignFile(KReportDesign *design, QString *errorMessage)
{
    const QString dir(QFile::decodeName(FILES_DATA_DIR));
    const QString fname(QLatin1String(QTest::currentTestFunction()) + QLatin1String(".kreport"));
    QFile file(dir + QDir::separator() + fname);
    bool ok = file.open(QFile::ReadOnly | QFile::Text);
    if (!ok) {
        *errorMessage = QString::fromLatin1("Could not open file %1: ").arg(file.fileName()) + file.errorString();
        return false;
    }

    QString content = file.readAll();
    if (file.error() != QFileDevice::NoError) {
        *errorMessage = QString::fromLatin1("Error reading file %1: ").arg(file.fileName()) + file.errorString();
        return false;
    }

    KReportDesignReadingStatus status;
    if (!design->setContent(content, &status)) {
        QString message;
        QDebug(&message) << status;
        *errorMessage = QLatin1String("Failed to load content. ") + message;
        return false;
    }
    errorMessage->clear();
    return true;
}

void FormatTest::testPageLayout()
{
    KReportDesign design;
    QString errorMessage;
    if (!openDesignFile(&design, &errorMessage)) {
        QFAIL(qPrintable(errorMessage));
    }

    const QPageLayout pageLayout = design.pageLayout();
    QVERIFY(pageLayout.isValid());
    QCOMPARE(pageLayout.pageSize().id(), QPageSize::A5);
    QCOMPARE(pageLayout.pageSize().sizePoints(), QPageSize(QPageSize::A5).sizePoints());
    QCOMPARE(pageLayout.orientation(), QPageLayout::Portrait);
    QCOMPARE(pageLayout.margins(QPageLayout::Millimeter), QMarginsF(30.0, 20.0, 40.0, 15.0));

    //! @todo move this renderer test to a separate place
#if 0
    KReportDesigner designer;
    QCOMPARE(designer.propertySet()->property("page-size").value().toString(), QLatin1String("A5"));
    QCOMPARE(designer.propertySet()->property("margin-bottom").value().toDouble(), KReportUnit::parseValue("1.5cm"));
    QCOMPARE(designer.propertySet()->property("margin-top").value().toDouble(), KReportUnit::parseValue("2.0cm"));
    QCOMPARE(designer.propertySet()->property("margin-left").value().toDouble(), KReportUnit::parseValue("3.0cm"));
    QCOMPARE(designer.propertySet()->property("margin-right").value().toDouble(), KReportUnit::parseValue("4.0cm"));

    KReportPreRenderer renderer( designer.document() );
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
#endif
}

void FormatTest::testLineElement()
{
#if 0 // todo
    KReportDesign design;
    QString errorMessage;
    if (!openDesignFile(&design, &errorMessage)) {
        QFAIL(qPrintable(errorMessage));
    }
#endif
    //! @todo move this renderer test to a separate place
#if 0
    KReportDesigner designer;
    ReportSectionDetail *ds = designer.detailSection();
    ReportSection *sec = ds->detailSection();
    KReportItemLine *l = dynamic_cast<KReportItemLine*>(sec->items().first());

    QVERIFY(l != 0);
    QCOMPARE(l->Z, 1.5);
    KReportPosition start = l->startPosition();
    KReportPosition end = l->endPosition();
    QCOMPARE(start.toPoint(), QPointF(KReportUnit::parseValue("1.5cm"), KReportUnit::parseValue("0.5cm")));
    QCOMPARE(end.toPoint(), QPointF(KReportUnit::parseValue("4.5cm"), KReportUnit::parseValue("2.5cm")));

    KReportPreRenderer renderer( designer.document() );
    renderer.generate();
    l = dynamic_cast<KReportItemLine*>(renderer.reportData()->object("line1"));

    QVERIFY(l != 0);
    QCOMPARE(l->Z, 1.5);
    start = l->startPosition();
    end = l->endPosition();
    QCOMPARE(start.toPoint(), QPointF(KReportUnit::parseValue("1.5cm"), KReportUnit::parseValue("0.5cm")));
    QCOMPARE(end.toPoint(), QPointF(KReportUnit::parseValue("4.5cm"), KReportUnit::parseValue("2.5cm")));
#endif
}

void FormatTest::testLabelElement()
{
    KReportDesign design;
    QString errorMessage;
    if (!openDesignFile(&design, &errorMessage)) {
        QFAIL(qPrintable(errorMessage));
    }

    QCOMPARE(design.title(), QLatin1String("Label Element Test Report"));

    QVERIFY(design.hasSection(KReportSection::Detail));
    KReportSection detailSection = design.section(KReportSection::Detail);
    QCOMPARE(detailSection.type(), KReportSection::Detail);
    QCOMPARE(detailSection.height(), CM_TO_POINT(5.0));
    QCOMPARE(detailSection.backgroundColor(), QColor("#eeeeee"));

    QList<KReportElement> elements = detailSection.elements();
    QCOMPARE(elements.count(), 1);
    KReportElement element = elements.first();
    QCOMPARE(element.name(), QLatin1String("label1"));
    KReportLabelElement label1(elements.first());
    QCOMPARE(label1.text(), "Label");
    QCOMPARE(label1.z(), 2.5);
    const QRectF rect(CM_TO_POINT(1.5), CM_TO_POINT(0.5), CM_TO_POINT(4.5), CM_TO_POINT(0.75));
    QCOMPARE(label1.rect(), rect);
    QCOMPARE(label1.backgroundColor(), QColor("#dddddd"));
    QCOMPARE(label1.foregroundColor(), QColor("#101010"));
    QCOMPARE(label1.backgroundOpacity(), 0.9);
    QCOMPARE(label1.alignment(), Qt::AlignRight | Qt::AlignBottom);
    QCOMPARE(label1.font().capitalization(), QFont::AllLowercase);
    QCOMPARE(label1.font().bold(), true);
    QCOMPARE(label1.font().weight(), 99);
    QCOMPARE(label1.font().italic(), true);
    QCOMPARE(label1.font().fixedPitch(), true);
    QCOMPARE(label1.font().family(), "Ubuntu");
    QCOMPARE(label1.font().kerning(), true);
    QCOMPARE(label1.font().underline(), true);
    QCOMPARE(label1.font().strikeOut(), true);
    QCOMPARE(label1.font().pointSizeF(), 9.0);
    QCOMPARE(label1.font().letterSpacing(), 110.0);
    QCOMPARE(label1.font().letterSpacingType(), QFont::PercentageSpacing);
    QCOMPARE(label1.borderStyle().width(), 1.0);
    QCOMPARE(label1.borderStyle().color(), QColor("#400000"));
    QCOMPARE(label1.borderStyle().penStyle(), Qt::DashLine);

    //! @todo move this renderer test to a separate place
#if 0
    KReportDesigner designer;//, doc.documentElement());
    ReportSectionDetail *ds = designer.detailSection();
    ReportSection *sec = ds->detailSection();
    QVERIFY(sec->items().count() == 1);
    KReportDesignerItemRectBase *rect = dynamic_cast<KReportDesignerItemRectBase*>(sec->items().first());

    QVERIFY(rect != 0);
    QRectF expected( QPointF(KReportUnit::parseValue("1.5cm"), KReportUnit::parseValue("0.5cm")), QSizeF(KReportUnit::parseValue("4.5cm"), KReportUnit::parseValue("0.75cm")));
    QCOMPARE(rect->pointRect(), expected);

    KReportPreRenderer renderer( designer.document() );
    renderer.generate();
    KReportItemBase *item = dynamic_cast<KReportItemBase*>(renderer.reportData()->object("label1"));

    QVERIFY(item != 0);
    KReportPosition pos = item->position();
    KReportSize size = item->size();
    QCOMPARE(pos.toPoint().x(), KReportUnit::parseValue("1.5cm"));
    QCOMPARE(pos.toPoint().y(), KReportUnit::parseValue("0.5cm"));
    QCOMPARE(size.toPoint(), QSizeF(KReportUnit::parseValue("4.5cm"), KReportUnit::parseValue("0.75cm")));
    QCOMPARE(size.toPoint(), QSizeF(KReportUnit::parseValue("4.5cm"), KReportUnit::parseValue("0.75cm")));
#endif
}
