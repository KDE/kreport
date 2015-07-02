/* This file is part of the KDE project
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

#include "KoReportItemImage.h"
#include "common/renderobjects.h"
#include "kreportplugin_debug.h"

#include <KPropertySet>

#include <kcodecs.h>

#include <QBuffer>
#include <QDomNodeList>


KoReportItemImage::KoReportItemImage()
{
    createProperties();
}

KoReportItemImage::KoReportItemImage(QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;

    m_name->setValue(element.toElement().attribute(QLatin1String("report:name")));
    m_controlSource->setValue(element.toElement().attribute(QLatin1String("report:item-data-source")));
    m_resizeMode->setValue(element.toElement().attribute(QLatin1String("report:resize-mode"), QLatin1String("stretch")));
    Z = element.toElement().attribute(QLatin1String("report:z-index")).toDouble();

    parseReportRect(element.toElement(), &m_pos, &m_size);

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == QLatin1String("report:inline-image-data")) {

            setInlineImageData(node.firstChild().nodeValue().toLatin1());
        } else {
            kreportpluginWarning() << "while parsing image element encountered unknow element: " << n;
        }
    }

}

KoReportItemImage::~KoReportItemImage()
{
    delete m_set;
}

bool KoReportItemImage::isInline() const
{
    return !(inlineImageData().isEmpty());
}

QByteArray KoReportItemImage::inlineImageData() const
{
    QPixmap pixmap = m_staticImage->value().value<QPixmap>();
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::ReadWrite);
    pixmap.save(&buffer, "PNG");   // writes pixmap into ba in PNG format,
    //! @todo should I remember the format used, or save as PNG as its lossless?

    QByteArray imageEncoded;
    KCodecs::base64Encode(buffer.buffer(), imageEncoded, true);
    return imageEncoded;
}

void KoReportItemImage::setInlineImageData(QByteArray dat, const QString &fn)
{
    if (!fn.isEmpty()) {
        QPixmap pix(fn);
        if (!pix.isNull())
            m_staticImage->setValue(pix);
        else {
            QPixmap blank(1, 1);
            blank.fill();
            m_staticImage->setValue(blank);
        }
    } else {
        const QByteArray binaryStream(KCodecs::base64Decode(dat));
        const QPixmap pix(QPixmap::fromImage(QImage::fromData(binaryStream), Qt::ColorOnly));
        m_staticImage->setValue(pix);
    }

}

QString KoReportItemImage::mode() const
{
    return m_resizeMode->value().toString();
}

void KoReportItemImage::setMode(const QString &m)
{
    if (mode() != m) {
        m_resizeMode->setValue(m);
    }
}

void KoReportItemImage::createProperties()
{
    m_set = new KPropertySet(0, QLatin1String("Image"));

    m_controlSource = new KProperty("item-data-source", QStringList(), QStringList(), QString(), tr("Data Source"));

    QStringList keys, strings;
    keys << QLatin1String("clip") << QLatin1String("stretch");
    strings << tr("Clip") << tr("Stretch");
    m_resizeMode = new KProperty("resize-mode", keys, strings, QLatin1String("clip"), tr("Resize Mode"));

    m_staticImage = new KProperty("static-image", QPixmap(), tr("Value"), tr("Value used if not bound to a field"));

    addDefaultProperties();
    m_set->addProperty(m_controlSource);
    m_set->addProperty(m_resizeMode);
    m_set->addProperty(m_staticImage);
}


void KoReportItemImage::setColumn(const QString &c)
{
    m_controlSource->setValue(c);
}

QString KoReportItemImage::itemDataSource() const
{
    return m_controlSource->value().toString();
}

QString KoReportItemImage::typeName() const
{
    return QLatin1String("image");
}

int KoReportItemImage::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                        const QVariant &data, KRScriptHandler *script)
{
    Q_UNUSED(script)

    QByteArray uudata;
    QByteArray imgdata;
    if (!isInline()) {
        imgdata = data.toByteArray();
    } else {
        uudata = inlineImageData();
        imgdata = KCodecs::base64Decode(uudata);
    }

    QImage img;
    img.loadFromData(imgdata);
    OROImage * id = new OROImage();
    id->setImage(img);
    if (mode().toLower() == QLatin1String("stretch")) {
        id->setScaled(true);
        id->setAspectRatioMode(Qt::KeepAspectRatio);
        id->setTransformationMode(Qt::SmoothTransformation);
    }

    id->setPosition(m_pos.toScene() + offset);
    id->setSize(m_size.toScene());
    if (page) {
        page->addPrimitive(id);
    }

    if (section) {
        OROImage *i2 = dynamic_cast<OROImage*>(id->clone());
        i2->setPosition(m_pos.toPoint());
        section->addPrimitive(i2);
    }

    if (!page) {
        delete id;
    }

    return 0; //Item doesn't stretch the section height
}


