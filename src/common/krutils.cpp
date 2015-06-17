/* This file is part of the KDE project
   Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "krutils.h"

#include "krpos.h"
#include "krsize.h"
#include "KoReportItemBase.h"

#include <KProperty>

#include <QColor>
#include <QFont>
#include <QDomDocument>
#include <QDomElement>
#include <QVariant>
#include <QString>

#include <float.h>

int KRUtils::readPercent(const QDomElement& el, const char* name, int defaultPercentValue, bool *ok)
{
    QString percent(el.attribute(QLatin1String(name)));
    if (percent.isEmpty()) {
        if (ok)
            *ok = true;
        return defaultPercentValue;
    }
    if (!percent.endsWith(QLatin1Char('%'))) {
        if (ok)
            *ok = false;
        return 0;
    }
    percent.chop(1);
    if (ok)
        *ok = true;
    return percent.toInt(ok);
}

//! @return string representation of @a value, cuts of zeros; precision is set to 2
static QString roundValueToString(qreal value)
{
    QString s(QString::number(value, 'g', 2));
    if (s.endsWith(QLatin1String(".00")))
        return QString::number(qRound(value));
    return s;
}

//! Used by readFontAttributes()
static QFont::Capitalization readFontCapitalization(const QByteArray& fontVariant, const QByteArray& textTransform)
{
    if (fontVariant == "small-caps")
        return QFont::SmallCaps;
    if (textTransform == "uppercase")
        return QFont::AllUppercase;
    if (textTransform == "lowercase")
        return QFont::AllLowercase;
    if (textTransform == "capitalize")
        return QFont::Capitalize;
    // default, "normal"
    return QFont::MixedCase;
}

bool KRUtils::readFontAttributes(const QDomElement& el, QFont& font)
{
    bool ok;
    const QFont::Capitalization cap = readFontCapitalization(
        el.attribute(QLatin1String("fo:font-variant")).toLatin1(),
        el.attribute(QLatin1String("fo:text-transform")).toLatin1());
    font.setCapitalization(cap);

    // weight
    const QByteArray fontWeight(el.attribute(QLatin1String("fo:font-weight"), QLatin1String("normal")).toLatin1());
    int weight = -1;
    if (fontWeight == "bold") {
        weight = QFont::Bold;
    }
    else if (fontWeight == "normal") {
        weight = QFont::Normal;
    }
    else if (!fontWeight.isEmpty()) {
        // Remember : Qt and CSS/XSL doesn't have the same scale. It's 100-900 instead of Qt's 0-100
        // See http://www.w3.org/TR/2001/REC-xsl-20011015/slice7.html#font-weight
        // and http://www.w3.org/TR/CSS2/fonts.html#font-boldness
        int boldness = fontWeight.toInt(&ok);
        if (ok)
            weight = (boldness - 100) / 8; // 0..100
        else
            return false;
    }
    if (weight >= 0)
        font.setWeight(weight);

    // italic
    const QByteArray fontStyle(el.attribute(QLatin1String("fo:font-style")).toLatin1());
    font.setItalic(fontStyle == "italic");

    // pitch
    const QByteArray fontPitch(el.attribute(QLatin1String("style:font-pitch")).toLatin1());
    font.setFixedPitch(fontPitch == "fixed");

    font.setFamily(el.attribute(QLatin1String("fo:font-family")));

    // kerning
    font.setKerning(QVariant(el.attribute(QLatin1String("style:letter-kerning"))).toBool());

    // underline
    const QByteArray underlineType(el.attribute(QLatin1String("style:text-underline-type")).toLatin1());
    font.setUnderline(!underlineType.isEmpty() && underlineType != "none");

    // stricken-out
    const QByteArray strikeOutType(el.attribute(QLatin1String("style:text-line-through-type")).toLatin1());
    font.setStrikeOut(!strikeOutType.isEmpty() && strikeOutType != "none");

//! @todo support fo:font-size-rel?
//! @todo support fo:font-size in px
    const QByteArray pointSize(el.attribute(QLatin1String("fo:font-size")).toLatin1());
    const int pointSizeInt = pointSize.toUInt(&ok);
    if (ok)
        font.setPointSize(pointSizeInt);
    else
        return false;

    // letter spacing
    // §7.16.2 of [XSL] http://www.w3.org/TR/xsl11/#letter-spacing
    QByteArray letterSpacing(el.attribute(QLatin1String("fo:letter-spacing")).toLatin1());
    if (letterSpacing.endsWith('%')) {
        letterSpacing.chop(1);
        const qreal letterSpacingReal = letterSpacing.toDouble(&ok);
        if (ok)
            font.setLetterSpacing(QFont::PercentageSpacing, letterSpacingReal);
        else
            return false;
    }
    else if (!letterSpacing.isEmpty()) {
        const qreal letterSpacingReal = letterSpacing.toDouble(&ok);
        if (ok)
            font.setLetterSpacing(QFont::AbsoluteSpacing, letterSpacingReal);
        else
            return false;
    }
    return true;
}

void KRUtils::writeFontAttributes(QDomElement& el, const QFont &font)
{
    switch (font.capitalization()) {
    case QFont::SmallCaps:
        el.setAttribute(QLatin1String("fo:font-variant"), QLatin1String("small-caps"));
        break;
    case QFont::MixedCase:
        // default: "normal", do not save
        break;
    case QFont::AllUppercase:
        el.setAttribute(QLatin1String("fo:text-transform"), QLatin1String("uppercase"));
        break;
    case QFont::AllLowercase:
        el.setAttribute(QLatin1String("fo:text-transform"), QLatin1String("lowercase"));
        break;
    case QFont::Capitalize:
        el.setAttribute(QLatin1String("fo:text-transform"), QLatin1String("capitalize"));
        break;
    }

    // Remember : Qt and CSS/XSL doesn't have the same scale. It's 100-900 instead of Qt's 0-100
    // See http://www.w3.org/TR/2001/REC-xsl-20011015/slice7.html#font-weight
    // and http://www.w3.org/TR/CSS2/fonts.html#font-boldness
    if (font.weight() == QFont::Light) {
        el.setAttribute(QLatin1String("fo:font-weight"), 200);
    }
    else if (font.weight() == QFont::Normal) {
        // Default
        //el.setAttribute("fo:font-weight", "normal"); // 400
    }
    else if (font.weight() == QFont::DemiBold) {
        el.setAttribute(QLatin1String("fo:font-weight"), 600);
    }
    else if (font.weight() == QFont::Bold) {
        el.setAttribute(QLatin1String("fo:font-weight"), QLatin1String("bold")); // 700
    }
    else if (font.weight() == QFont::Black) {
        el.setAttribute(QLatin1String("fo:font-weight"), 900);
    }
    else {
        el.setAttribute(QLatin1String("fo:font-weight"), qBound(10, font.weight(), 90) * 10);
    }
    // italic, default is "normal"
    if (font.italic()) {
        el.setAttribute(QLatin1String("fo:font-style"), QLatin1String("italic"));
    }
    // pitch, default is "variable"
    if (font.fixedPitch()) {
        el.setAttribute(QLatin1String("style:font-pitch"), QLatin1String("fixed"));
    }
    if (!font.family().isEmpty()) {
        el.setAttribute(QLatin1String("fo:font-family"), font.family());
    }
    // kerning, default is "true"
    el.setAttribute(QLatin1String("style:letter-kerning"), QLatin1String(font.kerning() ? "true" : "false"));
    // underline, default is "none"
    if (font.underline()) {
        el.setAttribute(QLatin1String("style:text-underline-type"), QLatin1String("single"));
    }
    // stricken-out, default is "none"
    if (font.strikeOut()) {
        el.setAttribute(QLatin1String("style:text-line-through-type"), QLatin1String("single"));
    }
    el.setAttribute(QLatin1String("fo:font-size"), font.pointSize());

    // letter spacing, default is "normal"
    // §7.16.2 of [XSL] http://www.w3.org/TR/xsl11/#letter-spacing
    if (font.letterSpacingType() == QFont::PercentageSpacing) {
        // A value of 100 will keep the spacing unchanged; a value of 200 will enlarge
        // the spacing after a character by the width of the character itself.
        if (font.letterSpacing() != 100.0) {
            el.setAttribute(QLatin1String("fo:letter-spacing"), roundValueToString(font.letterSpacing()) + QLatin1Char('%'));
        }
    }
    else {
        // QFont::AbsoluteSpacing
        // A positive value increases the letter spacing by the corresponding pixels; a negative value decreases the spacing.
        el.setAttribute(QLatin1String("fo:letter-spacing"), roundValueToString(font.letterSpacing()));
    }
}


void KRUtils::buildXMLRect(QDomElement & entity, KRPos *pos, KRSize *siz)
{
    KRUtils::setAttribute(entity, pos->toPoint() );
    KRUtils::setAttribute(entity, siz->toPoint() );
}

void KRUtils::buildXMLTextStyle(QDomDocument & doc, QDomElement & entity, KRTextStyleData ts)
{
    QDomElement element = doc.createElement(QLatin1String("report:text-style"));

    element.setAttribute(QLatin1String("fo:background-color"), ts.backgroundColor.name());
    element.setAttribute(QLatin1String("fo:foreground-color"), ts.foregroundColor.name());
    element.setAttribute(QLatin1String("fo:background-opacity"), QString::number(ts.backgroundOpacity) + QLatin1Char('%'));
    KRUtils::writeFontAttributes(element, ts.font);

    entity.appendChild(element);
}

void KRUtils::buildXMLLineStyle(QDomDocument & doc, QDomElement & entity, KRLineStyleData ls)
{
    QDomElement element = doc.createElement(QLatin1String("report:line-style"));

    element.setAttribute(QLatin1String("report:line-color"), ls.lineColor.name());
    element.setAttribute(QLatin1String("report:line-weight"), ls.weight);

    QString l;
    switch (ls.style) {
        case Qt::NoPen:
            l = QLatin1String("nopen");
            break;
        case Qt::SolidLine:
            l = QLatin1String("solid");
            break;
        case Qt::DashLine:
            l = QLatin1String("dash");
            break;
        case Qt::DotLine:
            l = QLatin1String("dot");
            break;
        case Qt::DashDotLine:
            l = QLatin1String("dashdot");
            break;
        case Qt::DashDotDotLine:
            l = QLatin1String("dashdotdot");
            break;
        default:
            l = QLatin1String("solid");
    }
    element.setAttribute(QLatin1String("report:line-style"), l);

    entity.appendChild(element);
}

void KRUtils::addPropertyAsAttribute(QDomElement* e, KProperty* p)
{
    const QString name = QLatin1String("report:") + QLatin1String(p->name().toLower());

    switch (p->value().type()) {
        case QVariant::Int :
            e->setAttribute(name, p->value().toInt());
            break;
        case QVariant::Double:
            e->setAttribute(name, p->value().toDouble());
            break;
        case QVariant::Bool:
            e->setAttribute(name, p->value().toInt());
            break;
        default:
            e->setAttribute(name, p->value().toString());
            break;
    }
}

void KRUtils::setAttribute(QDomElement &e, const QString &attribute, double value)
{
    QString s;
    s.setNum(value, 'f', DBL_DIG);
    e.setAttribute(attribute, s + QLatin1String("pt"));
}

void KRUtils::setAttribute(QDomElement &e, const QPointF &value)
{
    KRUtils::setAttribute(e, QLatin1String("svg:x"), value.x());
    KRUtils::setAttribute(e, QLatin1String("svg:y"), value.y());
}

void KRUtils::setAttribute(QDomElement &e, const QSizeF &value)
{
    KRUtils::setAttribute(e, QLatin1String("svg:width"), value.width());
    KRUtils::setAttribute(e, QLatin1String("svg:height"), value.height());
}

bool KRUtils::parseReportTextStyleData(const QDomElement & elemSource, KRTextStyleData & ts)
{
    if (elemSource.tagName() != QLatin1String("report:text-style"))
        return false;
    ts.backgroundColor = QColor(elemSource.attribute(QLatin1String("fo:background-color"), QLatin1String("#ffffff")));
    ts.foregroundColor = QColor(elemSource.attribute(QLatin1String("fo:foreground-color"), QLatin1String("#000000")));

    bool ok;
    ts.backgroundOpacity = KRUtils::readPercent(elemSource, "fo:background-opacity", 100, &ok);
    if (!ok) {
        return false;
    }
    if (!KRUtils::readFontAttributes(elemSource, ts.font)) {
        return false;
    }
    return true;
}

bool KRUtils::parseReportLineStyleData(const QDomElement & elemSource, KRLineStyleData & ls)
{
    if (elemSource.tagName() == QLatin1String("report:line-style")) {
        ls.lineColor = QColor(elemSource.attribute(QLatin1String("report:line-color"), QLatin1String("#ffffff")));
        ls.weight = elemSource.attribute(QLatin1String("report:line-weight"), QLatin1String("0")).toInt();

        QString l = elemSource.attribute(QLatin1String("report:line-style"), QLatin1String("nopen"));
        if (l == QLatin1String("nopen")) {
            ls.style = Qt::NoPen;
        } else if (l == QLatin1String("solid")) {
            ls.style = Qt::SolidLine;
        } else if (l == QLatin1String("dash")) {
            ls.style = Qt::DashLine;
        } else if (l == QLatin1String("dot")) {
            ls.style = Qt::DotLine;
        } else if (l == QLatin1String("dashdot")) {
            ls.style = Qt::DashDotLine;
        } else if (l == QLatin1String("dashdotdot")) {
            ls.style = Qt::DashDotDotLine;
        }
        return true;
    }
    return false;
}

bool KRUtils::parseReportRect(const QDomElement & elemSource, KRPos *pos, KRSize *siz)
{
    QStringList sl;
    QDomNamedNodeMap map = elemSource.attributes();
    for (int i=0; i < map.count(); ++i ) {
        sl << map.item(i).nodeName();
    }
    QPointF _pos;
    QSizeF _siz;

    _pos.setX(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:x"), QLatin1String("1cm"))));
    _pos.setY(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:y"), QLatin1String("1cm"))));
    _siz.setWidth(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:width"), QLatin1String("1cm"))));
    _siz.setHeight(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:height"), QLatin1String("1cm"))));

    pos->setPointPos(_pos);
    siz->setPointSize(_siz);
    return true;
}
