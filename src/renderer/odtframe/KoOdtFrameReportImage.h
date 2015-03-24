/*
   Calligra Report Engine
   Copyright (C) 2011, 2012 by Dag Andersen (danders@get2net.dk)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KOODTFRAMEREPORTIMAGE_H
#define KOODTFRAMEREPORTIMAGE_H

#include "KoOdtFrameReportPrimitive.h"

#include <QString>

class KoXmlWriter;
class OROImage;
class OROPrimitive;

class KoOdtFrameReportImage : public KoOdtFrameReportPrimitive
{
public:
    explicit KoOdtFrameReportImage(OROImage *primitive);
    virtual ~KoOdtFrameReportImage();

    virtual void createBody(KoXmlWriter *bodyWriter) const;
    virtual bool saveData(KoStore *store, KoXmlWriter *manifestWriter) const;

    OROImage *image() const;

    void setImageName(const QString &name);
    QString imageName() const { return m_name; }

private:
    QString m_name;
};

#endif // KOODTFRAMEREPORTIMAGE_H
