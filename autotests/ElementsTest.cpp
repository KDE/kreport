/* This file is part of the KDE project
 * Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>
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

#include "ElementsTest.h"
#include "KReportTestUtils.h"
#include <KReportElement>
#include <KReportLabelElement>
#include <KReportSection>
#include <KReportUnit>

#include <QTest>

QTEST_MAIN(ElementsTest)

Q_DECLARE_METATYPE(KReportElement)
Q_DECLARE_METATYPE(KReportLabelElement)
Q_DECLARE_METATYPE(KReportSection)

void ElementsTest::testElements()
{
    KReportElement e;
    QCOMPARE(e, e);
    QCOMPARE(e.name(), "");
    QCOMPARE(e.rect(), QRectF());
    QCOMPARE(e.z(), 0.0);
    QCOMPARE(e.backgroundColor(), QColor());
    QCOMPARE(e.foregroundColor(), QColor());
    QCOMPARE(e.backgroundOpacity(), 0.0);

    e.setZ(10.3);
    QCOMPARE(e.z(), 10.3);
    e.setName("element1");
    QCOMPARE(e.name(), "element1");
    e.setRect(QRect(1, 2, 3, 4));
    QCOMPARE(e.rect(), QRectF(1, 2, 3, 4));
    e.setBackgroundColor(Qt::red);
    QCOMPARE(e.backgroundColor(), QColor(Qt::red));
    e.setForegroundColor(Qt::white);
    QCOMPARE(e.foregroundColor(), QColor(Qt::white));
    QCOMPARE(e, e);

    KReportElement e2 = e;
    e.setName("");
    QCOMPARE(e, e2);
    QCOMPARE(e.name(), e2.name());
    QCOMPARE(e.rect(), e2.rect());

    e2.setName("element2");
    KReportLabelElement lbl1(e2);
    QCOMPARE(e2.name(), lbl1.name());
    e2.setName("");
    QCOMPARE(lbl1.name(), "element2");

    lbl1.setText("label1");
    QCOMPARE(lbl1.text(), "label1");
    e = lbl1;

    KReportLabelElement lbl2(e); // e points to lbl1 so shares lbl1 in fact
    QCOMPARE(lbl1, lbl2);
    QCOMPARE(KReportLabelElement(e), lbl2);
    lbl2 = e;
    QCOMPARE(KReportLabelElement(e), lbl2);

    lbl1 = KReportLabelElement();
    QCOMPARE(lbl1.alignment(), Qt::AlignLeft | Qt::AlignVCenter); // defaults
    lbl1.setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    QCOMPARE(lbl1.alignment(), Qt::AlignHCenter|Qt::AlignBottom);

    lbl1.setBackgroundOpacity(20.0);
    QCOMPARE(lbl1.backgroundOpacity(), 1.0); // 1.0 is max
    lbl1.setBackgroundOpacity(-100.10);
    QCOMPARE(lbl1.backgroundOpacity(), 0.0); // 0.0 is min

    lbl1.setFont(QApplication::font());
    QFont f = lbl1.font();
    QCOMPARE(f, QApplication::font());

    QCOMPARE(lbl1.borderStyle().width(), 0.0); // default
    QCOMPARE(lbl1.borderStyle().penStyle(), Qt::NoPen); // default
    QCOMPARE(lbl1.borderStyle().color(), QColor()); // default
    KReportLineStyle lineStyle;
    lineStyle.setWidth(2.0);
    lineStyle.setPenStyle(Qt::DashLine);
    lineStyle.setColor(QColor("brown"));
    lbl1.setBorderStyle(lineStyle);
    QCOMPARE(lbl1.borderStyle(), lineStyle);
    lineStyle.setColor(Qt::yellow);
    QCOMPARE(lbl1.borderStyle().color(), QColor(Qt::yellow)); // shared
}

void ElementsTest::testElementCloning()
{
    KReportLabelElement lbl1("text1"), lbl2;
    lbl1.setName("label1");
    lbl2 = lbl1.clone();
    QVERIFY(lbl1 != lbl2); // a deep copy, not equal
    lbl1.setText("");
    QCOMPARE(lbl1.name(), lbl2.name());
    QCOMPARE(lbl1.text(), "");
    QCOMPARE(lbl2.text(), "text1"); // this is another copy, not affected by lbl1.setText("")

    lbl1.setName("label1");
    lbl1.setText("text1");
    KReportElement e1 = lbl1;
    KReportElement e2 = e1.clone();
    QVERIFY2(e1 != e2, "Cloned element not equal to the original");
    QCOMPARE(e1.name(), "label1");
    QCOMPARE(e2.name(), "label1");
    QVERIFY2(KReportLabelElement(e2) != lbl1, "Reference to original not equal to a clone");
    QVERIFY2(KReportLabelElement(e2) != KReportLabelElement(e1), "Reference to original not equal to reference to a clone");
    lbl1.setName("");
    lbl1.setText("");
    QCOMPARE(e1.name(), ""); // shared with lbl1
    QCOMPARE(e2.name(), "label1"); // shared with lbl1's clone
    QCOMPARE(KReportLabelElement(e1).text(), "");
    QCOMPARE(KReportLabelElement(e2).text(), "text1");
    KReportLabelElement lbl3 = e2;

    // e2 points to a lbl1's clone with name=label1, text=text1, so:
    KReportLabelElement(e2).setText("text2"); // This affect e2 and lbl2 because
                                              // KReportLabelElement(e2) upcasts it to the KReportLabelElement type
    QCOMPARE(KReportLabelElement(e2).text(), "text2");
    QCOMPARE(lbl3.text(), "text2");
    QCOMPARE(lbl1.text(), ""); // lbl1 unaffected
}

void ElementsTest::testSections()
{
    // types
    KReportSection sec1;
    QCOMPARE(sec1.type(), KReportSection::InvalidType);
    QCOMPARE(KReportSection(sec1).type(), KReportSection::InvalidType);
    sec1.setType(KReportSection::PageFooterFirst);
    KReportSection sec2(sec1);
    QCOMPARE(sec1.type(), sec2.type());
    QCOMPARE(sec1, sec2);
    sec1.setType(KReportSection::PageFooterEven);
    QCOMPARE(sec1.type(), sec2.type()); // shared
    QCOMPARE(sec1, sec2); // shared

    // height
    KReportSection::setDefaultHeight(-1); // sanity: reset
    QCOMPARE(sec1.height(), KReportSection::defaultHeight()); // initially: default height
    KReportSection::setDefaultHeight(CM_TO_POINT(10.0)); // sanity: reset
    QCOMPARE(KReportSection::defaultHeight(), CM_TO_POINT(10.0));
    QCOMPARE(sec1.height(), KReportSection::defaultHeight()); // still default height
    KReportSection::setDefaultHeight(-1); // reset
    QCOMPARE(sec1.height(), KReportSection::defaultHeight()); // still default height
    sec1.setHeight(CM_TO_POINT(7.0));
    QCOMPARE(sec1.height(), CM_TO_POINT(7.0)); // custom height

    // background color
    KReportSection::setDefaultBackgroundColor(QColor()); // sanity: reset
    QCOMPARE(sec1.backgroundColor(), KReportSection::defaultBackgroundColor()); // initially: default color
    KReportSection::setDefaultBackgroundColor(Qt::yellow); // sanity: reset
    QCOMPARE(KReportSection::defaultBackgroundColor(), QColor(Qt::yellow));
    QCOMPARE(sec1.backgroundColor(), KReportSection::defaultBackgroundColor()); // still default color
    KReportSection::setDefaultBackgroundColor(QColor()); // reset
    QCOMPARE(sec1.backgroundColor(), KReportSection::defaultBackgroundColor()); // still default color
    sec1.setBackgroundColor(Qt::lightGray);
    QCOMPARE(sec1.backgroundColor(), QColor(Qt::lightGray)); // custom color

    // elements
    QVERIFY2(sec1.elements().isEmpty(), "Initially section has no elements inside");
    QVERIFY2(sec2.elements().isEmpty(), "Initially section has no elements inside");
    KReportLabelElement lbl1("text1");
    QVERIFY(sec1.addElement(lbl1));
    QCOMPARE(sec1.elements().count(), 1);
    QCOMPARE(sec2.elements().count(), 1);
    QCOMPARE(sec1.elements().first(), sec1.elements().last());
    QCOMPARE(KReportLabelElement(sec1.elements().first()), lbl1);
    QCOMPARE(KReportLabelElement(sec1.elements().at(0)), lbl1);

    QVERIFY2(!sec1.addElement(lbl1), "Adding the same element twice isn't possible");
    QCOMPARE(sec1.elements().count(), 1);
    KReportLabelElement lbl2("text2");
    QVERIFY2(!sec1.removeElement(lbl2), "Removing not added element isn't possible");
    QCOMPARE(sec1.elements().count(), 1);
    QVERIFY(sec1.removeElement(lbl1));
    QCOMPARE(sec1.elements().count(), 0);
    QCOMPARE(sec2.elements().count(), 0);

    lbl2 = lbl1.clone();
    QVERIFY(sec1.addElement(lbl1));
    QVERIFY(sec1.addElement(lbl2));
    QCOMPARE(sec1.elements().count(), 2);
    QCOMPARE(sec2.elements().count(), 2);

    // insert
    sec1 = KReportSection();
    QVERIFY(sec1.insertElement(0, lbl1));
    QVERIFY2(!sec1.insertElement(0, lbl1), "Cannot insert the same element twice");
    QVERIFY2(!sec1.insertElement(2, lbl2), "Cannot insert element at position 2");
    QVERIFY2(!sec1.insertElement(-1, lbl2), "Cannot insert element at position -1");
    QVERIFY(sec1.insertElement(0, lbl2));

    // indexOf
    QCOMPARE(sec1.elements().indexOf(lbl1), 1);
    QCOMPARE(sec1.elements().indexOf(lbl2), 0);
    QCOMPARE(KReportLabelElement(sec1.elements().last()), lbl1);

    // removeAt
    QVERIFY2(!sec1.removeElementAt(2), "Cannot remove element at position 2");
    QVERIFY2(!sec1.removeElementAt(-1), "Cannot remove element at position -1");
    QVERIFY(sec1.removeElementAt(1));
    QVERIFY(sec1.removeElementAt(0));

    // cloning sections
    sec1 = KReportSection();
    QVERIFY(sec1.addElement(lbl1));
    QVERIFY(sec1.addElement(lbl2));
    sec2 = sec1.clone();
    QCOMPARE(sec1.elements().count(), sec2.elements().count());
    QVERIFY2(sec1.elements() != sec2.elements(), "Cloned sections have different sets of elements");
    QVERIFY2(sec1 != sec2, "Cloned section not equal to the original");
    QVERIFY2(sec1.elements().first() != sec2.elements().first(), "Elements of cloned sections have different sets of elements");
}
