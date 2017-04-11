/* This file is part of the KDE project
   Copyright (C) 2010-2015 Jarosław Staniek <staniek@kde.org>

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

#include "KReportUtils.h"
#include "KReportUnit.h"
#include "KReportItemBase.h"
#include "KReportLineStyle.h"

#include <KProperty>

#include <QDomDocument>
#include <QDomElement>

#include <float.h>

QString KReportUtils::attr(const QDomElement &el, const char *attrName,
                           const QString &defaultValue)
{
    const QString val = el.attribute(QLatin1String(attrName));
    return val.isEmpty() ? defaultValue : val;
}

QByteArray KReportUtils::attr(const QDomElement &el, const char *attrName,
                           const QByteArray &defaultValue)
{
    const QByteArray val = el.attribute(QLatin1String(attrName)).toLatin1();
    return val.isEmpty() ? defaultValue : val;
}

bool KReportUtils::attr(const QDomElement &el, const char *attrName, bool defaultValue)
{
    const QString val = el.attribute(QLatin1String(attrName));
    return val.isEmpty() ? defaultValue : QVariant(val).toBool();
}

int KReportUtils::attr(const QDomElement &el, const char *attrName, int defaultValue)
{
    const QString val = el.attribute(QLatin1String(attrName));
    if (val.isEmpty()) {
        return defaultValue;
    }
    bool ok;
    const int result = QVariant(val).toInt(&ok);
    return ok ? result : defaultValue;
}

qreal KReportUtils::attr(const QDomElement &el, const char *attrName, qreal defaultValue)
{
    const QString val = el.attribute(QLatin1String(attrName));
    return KReportUnit::parseValue(val, defaultValue);
}

QColor KReportUtils::attr(const QDomElement &el, const char *attrName, const QColor &defaultValue)
{
    const QString val = el.attribute(QLatin1String(attrName));
    if (val.isEmpty()) {
        return defaultValue;
    }
    return QColor(val);
}

qreal KReportUtils::attrPercent(const QDomElement& el, const char* attrName, qreal defaultValue)
{
    QString str(el.attribute(QLatin1String(attrName)));
    if (str.isEmpty() || !str.endsWith(QLatin1Char('%'))) {
        return defaultValue;
    }
    str.chop(1);
    bool ok;
    const qreal result = QVariant(str).toReal(&ok) / 100.0;
    if (!ok) {
        return defaultValue;
    }
    return result;
}

Qt::PenStyle KReportUtils::penStyle(const QString& str, Qt::PenStyle defaultValue)
{
    const QByteArray s(str.toLatin1());
    if (s == "nopen" || s == "none") {
        return Qt::NoPen;
    } else if (s == "solid") {
        return Qt::SolidLine;
    } else if (s == "dash" || s == "wave" /*we have nothing better for now*/) {
        return Qt::DashLine;
    } else if (s == "dot" || s == "dotted") {
        return Qt::DotLine;
    } else if (s == "dashdot" || s == "dot-dash") {
        return Qt::DashDotLine;
    } else if (s == "dashdotdot" || s == "dot-dot-dash") {
        return Qt::DashDotDotLine;
    } else {
        return defaultValue;
    }
}

Qt::Alignment KReportUtils::verticalAlignment(const QString &str, Qt::Alignment defaultValue)
{
    const QByteArray s(str.toLatin1());
    if (s == "center") {
        return Qt::AlignVCenter;
    } else if (s == "top") {
        return Qt::AlignTop;
    } else if (s == "bottom") {
        return Qt::AlignBottom;
    } else {
        return defaultValue;
    }
}

Qt::Alignment KReportUtils::horizontalAlignment(const QString &str, Qt::Alignment defaultValue)
{
    const QByteArray s(str.toLatin1());
    if (s == "center") {
        return Qt::AlignHCenter;
    } else if (s == "right") {
        return Qt::AlignRight;
    } else if (s == "left") {
        return Qt::AlignLeft;
    } else {
        return defaultValue;
    }
}

QString KReportUtils::verticalToString(Qt::Alignment alignment)
{
    if (alignment.testFlag(Qt::AlignVCenter)) {
        return QLatin1String("center");
    } else if (alignment.testFlag(Qt::AlignTop)) {
        return QLatin1String("top");
    } else if (alignment.testFlag(Qt::AlignBottom)) {
        return QLatin1String("bottom");
    }
    return QString();
}

QString KReportUtils::horizontalToString(Qt::Alignment alignment)
{
    if (alignment.testFlag(Qt::AlignHCenter)) {
        return QLatin1String("center");
    } else if (alignment.testFlag(Qt::AlignLeft)) {
        return QLatin1String("left");
    } else if (alignment.testFlag(Qt::AlignRight)) {
        return QLatin1String("right");
    }
    return QString();
}

QRectF KReportUtils::readRectAttributes(const QDomElement &el, const QRectF &defaultValue)
{
    QRectF val;
    val.setX(attr(el, "svg:x", defaultValue.x()));
    val.setY(attr(el, "svg:y", defaultValue.y()));
    val.setWidth(attr(el, "svg:width", defaultValue.width()));
    val.setHeight(attr(el, "svg:height", defaultValue.height()));
    return val;
}

int KReportUtils::readPercent(const QDomElement& el, const char* name, int defaultPercentValue, bool *ok)
{
    Q_ASSERT(name);
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

void KReportUtils::readFontAttributes(const QDomElement& el, QFont *font)
{
    Q_ASSERT(font);
    const QFont::Capitalization cap = readFontCapitalization(
        attr(el, "fo:font-variant", QByteArray()), attr(el, "fo:text-transform", QByteArray()));
    font->setCapitalization(cap);

    // weight
    const QByteArray fontWeight(attr(el, "fo:font-weight", QByteArray()));
    int weight = -1;
    if (fontWeight == "bold") {
        weight = QFont::Bold;
    }
    if (fontWeight == "normal") {
        weight = QFont::Normal;
    }
    else if (!fontWeight.isEmpty()) {
        // Remember : Qt and CSS/XSL doesn't have the same scale. It's 100-900 instead of Qt's 0-100
        // See http://www.w3.org/TR/2001/REC-xsl-20011015/slice7.html#font-weight
        // and http://www.w3.org/TR/CSS2/fonts.html#font-boldness
        bool ok;
        qreal boldness = fontWeight.toUInt(&ok);
        if (ok) {
            boldness = qMin(boldness, 900.0);
            boldness = qMax(boldness, 100.0);
            weight = (boldness - 100.0) * 0.12375 /*== 99/800*/; // 0..99
        }
    }
    if (weight >= 0) {
        font->setWeight(weight);
    }

    font->setItalic(attr(el, "fo:font-style", QByteArray()) == "italic");
    font->setFixedPitch(attr(el, "style:font-pitch", QByteArray()) == "fixed");
    font->setFamily(attr(el, "fo:font-family", font->family()));
    font->setKerning(attr(el, "style:letter-kerning", font->kerning()));

    // underline
    const QByteArray underlineType(attr(el, "style:text-underline-type", QByteArray()));
    font->setUnderline(!underlineType.isEmpty() && underlineType != "none"); // double or single (we don't recognize them)

    // stricken-out
    const QByteArray strikeOutType(attr(el, "style:text-line-through-type", QByteArray()));
    font->setStrikeOut(!strikeOutType.isEmpty() && strikeOutType != "none"); // double or single (we don't recognize them)

//! @todo support fo:font-size-rel?
//! @todo support fo:font-size in px
    font->setPointSizeF(KReportUtils::attr(el, "fo:font-size", font->pointSizeF()));

    // letter spacing
    // §7.16.2 of [XSL] http://www.w3.org/TR/xsl11/#letter-spacing
    font->setLetterSpacing(QFont::PercentageSpacing,
                          100.0 * KReportUtils::attrPercent(el, "fo:letter-spacing", font->letterSpacing()));
}

void KReportUtils::writeFontAttributes(QDomElement *el, const QFont &font)
{
    Q_ASSERT(el);
    switch (font.capitalization()) {
    case QFont::SmallCaps:
        el->setAttribute(QLatin1String("fo:font-variant"), QLatin1String("small-caps"));
        break;
    case QFont::MixedCase:
        // default: "normal", do not save
        break;
    case QFont::AllUppercase:
        el->setAttribute(QLatin1String("fo:text-transform"), QLatin1String("uppercase"));
        break;
    case QFont::AllLowercase:
        el->setAttribute(QLatin1String("fo:text-transform"), QLatin1String("lowercase"));
        break;
    case QFont::Capitalize:
        el->setAttribute(QLatin1String("fo:text-transform"), QLatin1String("capitalize"));
        break;
    }

    // Remember : Qt and CSS/XSL doesn't have the same scale. It's 100-900 instead of Qt's 0-100
    // See http://www.w3.org/TR/2001/REC-xsl-20011015/slice7.html#font-weight
    // and http://www.w3.org/TR/CSS2/fonts.html#font-boldness
    if (font.weight() == QFont::Light) {
        el->setAttribute(QLatin1String("fo:font-weight"), 200);
    }
    else if (font.weight() == QFont::Normal) {
        // Default
        //el->setAttribute("fo:font-weight", "normal"); // 400
    }
    else if (font.weight() == QFont::DemiBold) {
        el->setAttribute(QLatin1String("fo:font-weight"), 600);
    }
    else if (font.weight() == QFont::Bold) {
        el->setAttribute(QLatin1String("fo:font-weight"), QLatin1String("bold")); // 700
    }
    else if (font.weight() == QFont::Black) {
        el->setAttribute(QLatin1String("fo:font-weight"), 900);
    }
    else {
        el->setAttribute(QLatin1String("fo:font-weight"), qBound(10, font.weight(), 90) * 10);
    }
    // italic, default is "normal"
    if (font.italic()) {
        el->setAttribute(QLatin1String("fo:font-style"), QLatin1String("italic"));
    }
    // pitch, default is "variable"
    if (font.fixedPitch()) {
        el->setAttribute(QLatin1String("style:font-pitch"), QLatin1String("fixed"));
    }
    if (!font.family().isEmpty()) {
        el->setAttribute(QLatin1String("fo:font-family"), font.family());
    }
    // kerning, default is "true"
    el->setAttribute(QLatin1String("style:letter-kerning"), QLatin1String(font.kerning() ? "true" : "false"));
    // underline, default is "none"
    if (font.underline()) {
        el->setAttribute(QLatin1String("style:text-underline-type"), QLatin1String("single"));
    }
    // stricken-out, default is "none"
    if (font.strikeOut()) {
        el->setAttribute(QLatin1String("style:text-line-through-type"), QLatin1String("single"));
    }
    el->setAttribute(QLatin1String("fo:font-size"), font.pointSize());

    // letter spacing, default is "normal"
    // §7.16.2 of [XSL] http://www.w3.org/TR/xsl11/#letter-spacing
    if (font.letterSpacingType() == QFont::PercentageSpacing) {
        // A value of 100 will keep the spacing unchanged; a value of 200 will enlarge
        // the spacing after a character by the width of the character itself.
        if (font.letterSpacing() != 100.0) {
            el->setAttribute(QLatin1String("fo:letter-spacing"), roundValueToString(font.letterSpacing()) + QLatin1Char('%'));
        }
    }
    else {
        // QFont::AbsoluteSpacing
        // A positive value increases the letter spacing by the corresponding pixels; a negative value decreases the spacing.
        el->setAttribute(QLatin1String("fo:letter-spacing"), roundValueToString(font.letterSpacing()));
    }
}


void KReportUtils::buildXMLRect(QDomElement *entity, const QPointF &pos, const QSizeF &size)
{
    Q_ASSERT(entity);

    KReportUtils::setAttribute(entity, pos);
    KReportUtils::setAttribute(entity, size );
}

void KReportUtils::buildXMLTextStyle(QDomDocument *doc, QDomElement *entity, const KRTextStyleData &ts)
{
    Q_ASSERT(doc);
    Q_ASSERT(entity);
    QDomElement element = doc->createElement(QLatin1String("report:text-style"));

    element.setAttribute(QLatin1String("fo:background-color"), ts.backgroundColor.name());
    element.setAttribute(QLatin1String("fo:foreground-color"), ts.foregroundColor.name());
    element.setAttribute(QLatin1String("fo:background-opacity"), QString::number(ts.backgroundOpacity) + QLatin1Char('%'));
    KReportUtils::writeFontAttributes(&element, ts.font);

    entity->appendChild(element);
}

void KReportUtils::buildXMLLineStyle(QDomDocument *doc, QDomElement *entity, const KReportLineStyle &ls)
{
    Q_ASSERT(doc);
    Q_ASSERT(entity);
    QDomElement element = doc->createElement(QLatin1String("report:line-style"));

    element.setAttribute(QLatin1String("report:line-color"), ls.color().name());
    element.setAttribute(QLatin1String("report:line-weight"), ls.weight());

    QString l;
    switch (ls.penStyle()) {
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

    entity->appendChild(element);
}

void KReportUtils::addPropertyAsAttribute(QDomElement* e, KProperty* p)
{
    Q_ASSERT(e);
    Q_ASSERT(p);
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

void KReportUtils::setAttribute(QDomElement *e, const QString &attribute, double value)
{
    Q_ASSERT(e);
    QString s;
    s.setNum(value, 'f', DBL_DIG);
    e->setAttribute(attribute, s + QLatin1String("pt"));
}

void KReportUtils::setAttribute(QDomElement *e, const QPointF &value)
{
    Q_ASSERT(e);
    KReportUtils::setAttribute(e, QLatin1String("svg:x"), value.x());
    KReportUtils::setAttribute(e, QLatin1String("svg:y"), value.y());
}

void KReportUtils::setAttribute(QDomElement *e, const QSizeF &value)
{
    Q_ASSERT(e);
    KReportUtils::setAttribute(e, QLatin1String("svg:width"), value.width());
    KReportUtils::setAttribute(e, QLatin1String("svg:height"), value.height());
}

bool KReportUtils::parseReportTextStyleData(const QDomElement & elemSource, KRTextStyleData *ts)
{
    Q_ASSERT(ts);
    if (elemSource.tagName() != QLatin1String("report:text-style"))
        return false;
    ts->backgroundColor = QColor(elemSource.attribute(QLatin1String("fo:background-color"), QLatin1String("#ffffff")));
    ts->foregroundColor = QColor(elemSource.attribute(QLatin1String("fo:foreground-color"), QLatin1String("#000000")));

    bool ok;
    ts->backgroundOpacity = KReportUtils::readPercent(elemSource, "fo:background-opacity", 100, &ok);
    if (!ok) {
        return false;
    }
    KReportUtils::readFontAttributes(elemSource, &ts->font);
    return true;
}

bool KReportUtils::parseReportLineStyleData(const QDomElement & elemSource, KReportLineStyle *ls)
{
    Q_ASSERT(ls);
    if (elemSource.tagName() == QLatin1String("report:line-style")) {
        ls->setColor(QColor(elemSource.attribute(QLatin1String("report:line-color"), QLatin1String("#ffffff"))));
        ls->setWeight(elemSource.attribute(QLatin1String("report:line-weight"), QLatin1String("0.0")).toDouble());

        QString l = elemSource.attribute(QLatin1String("report:line-style"), QLatin1String("nopen"));
        if (l == QLatin1String("nopen")) {
            ls->setPenStyle(Qt::NoPen);
        } else if (l == QLatin1String("solid")) {
            ls->setPenStyle(Qt::SolidLine);
        } else if (l == QLatin1String("dash")) {
            ls->setPenStyle(Qt::DashLine);
        } else if (l == QLatin1String("dot")) {
            ls->setPenStyle(Qt::DotLine);
        } else if (l == QLatin1String("dashdot")) {
            ls->setPenStyle(Qt::DashDotLine);
        } else if (l == QLatin1String("dashdotdot")) {
            ls->setPenStyle(Qt::DashDotDotLine);
        }
        return true;
    }
    return false;
}

#if 0
bool KReportUtils::parseReportRect(const QDomElement & elemSource, KReportPosition *pos, KReportSize *size)
{
    Q_ASSERT(pos);
    Q_ASSERT(size);
//    QStringList sl;
//    QDomNamedNodeMap map = elemSource.attributes();
//    for (int i=0; i < map.count(); ++i ) {
//        sl << map.item(i).nodeName();
//    }
    QPointF _pos;
    QSizeF _siz;

    _pos.setX(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:x"), QLatin1String("1cm"))));
    _pos.setY(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:y"), QLatin1String("1cm"))));
    _siz.setWidth(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:width"), QLatin1String("1cm"))));
    _siz.setHeight(KReportUnit::parseValue(elemSource.attribute(QLatin1String("svg:height"), QLatin1String("1cm"))));

    pos->setPointPos(_pos);
    size->setPointSize(_siz);
    return true;
}
#endif

class PageIds : private QHash<QString, QPageSize::PageSizeId>
{
public:
    PageIds() {}
    QPageSize::PageSizeId id(const QString &key) {
        if (isEmpty()) {
            for (int i = 0; i < QPageSize::LastPageSize; ++i) {
                QString key(QPageSize::key(static_cast<QPageSize::PageSizeId>(i)));
                if (key.isEmpty()) {
                    break;
                }
                insert(key, static_cast<QPageSize::PageSizeId>(i));
            }
        }
        return value(key);
    }
};

Q_GLOBAL_STATIC(PageIds, s_pageIds)

QPageSize::PageSizeId KReportUtils::pageSizeId(const QString &key)
{
    return s_pageIds->id(key);
}

QPageSize KReportUtils::pageSize(const QString &key)
{
    return QPageSize(s_pageIds->id(key));
}
