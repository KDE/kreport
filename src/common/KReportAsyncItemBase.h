/*  This file is part of the KDE project
    Copyright (C) 2011  Adam Pigg <adam@piggz.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef KREPORTASYNCITEMBASE_H
#define KREPORTASYNCITEMBASE_H

#include "KReportItemBase.h"

class KREPORT_EXPORT KReportAsyncItemBase : public KReportItemBase
{
    Q_OBJECT
public:
    virtual int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset, const QVariant &data, KReportScriptHandler *script) = 0;

Q_SIGNALS:
    void finishedRendering();
};

#endif // KREPORTASYNCITEMBASE_H
