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

#include "KReportScriptImage.h"
#include "KReportItemImage.h"

#include <KProperty>

namespace Scripting
{

Image::Image(KReportItemImage *i)
{
    m_image = i;
}


Image::~Image()
{
}

QPointF Image::position() const
{
    return m_image->m_pos.toPoint();
}
void Image::setPosition(const QPointF& p)
{
    m_image->m_pos.setPointPos(p);
}

QSizeF Image::size() const
{
    return m_image->m_size.toPoint();
}
void Image::setSize(const QSizeF& s)
{
    m_image->m_size.setPointSize(s);
}

QString Image::resizeMode() const
{
    return m_image->m_resizeMode->value().toString();
}

void Image::setResizeMode(const QString &rm)
{
    if (rm == QLatin1String("Stretch")) {
        m_image->m_resizeMode->setValue(QLatin1String("Stretch"));
    } else {
        m_image->m_resizeMode->setValue(QLatin1String("Clip"));
    }
}

void Image::setInlineImage(const QByteArray &ba)
{
    m_image->setInlineImageData(ba);
}

void Image::loadFromFile(const QVariant &pth)
{
    QString str = pth.toString();
    m_image->setInlineImageData(QByteArray(), str);
}
}
