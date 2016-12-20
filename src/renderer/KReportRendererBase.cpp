/* This file is part of the KDE project
   Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)

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

#include "KReportRendererBase.h"
#include "KReportScreenRenderer_p.h"
#include "KReportPrintRenderer_p.h"
#include "KReportKSpreadRenderer.h"
#include "KReportHTMLTableRenderer_p.h"
#include "KReportHTMLCSSRenderer_p.h"
//! @todo port #include "KReportODTRenderer.h"
//! @todo port #include "KOdtFrameReportRenderer.h"

class Q_DECL_HIDDEN KReportRendererContext::Private 
{
public:
    Private();
    ~Private();
    QUrl destinationUrl;
    QPainter *painter = nullptr;
    QPrinter *printer = nullptr;    
};

KReportRendererContext::Private::Private()
{
}

KReportRendererContext::Private::~Private()
{
}


KReportRendererContext::KReportRendererContext()
 : d(new Private())
{
}

KReportRendererContext::~KReportRendererContext()
{
    delete d;
}

QPainter * KReportRendererContext::painter() const
{
    return d->painter;
}

QPrinter * KReportRendererContext::printer() const
{
    return d->printer;
}


QPainter * KReportRendererContext::painter()
{
    return d->painter;
}

QPrinter * KReportRendererContext::printer()
{
    return d->printer;
}

void KReportRendererContext::setUrl(const QUrl& url)
{
    d->destinationUrl = url;
}

void KReportRendererContext::setPainter(QPainter* painter)
{
    d->painter = painter;
}

void KReportRendererContext::setPrinter(QPrinter* printer)
{
    d->printer = printer;
}

QUrl KReportRendererContext::url() const
{
    return d->destinationUrl;
}


KReportRendererBase::KReportRendererBase()
{
}

KReportRendererBase::~KReportRendererBase()
{
}

class KReportRendererFactory::Private 
{
public:
    bool dummy = true;
};

KReportRendererFactory::KReportRendererFactory() : d(new Private())
{
}

KReportRendererFactory::~KReportRendererFactory()
{
    delete d;
}

KReportRendererBase* KReportRendererFactory::createInstance(const QString& key)
{
    const QString lowerKey = key.toLower();
    if (lowerKey == QLatin1String("screen")) {
        return new KReportPrivate::ScreenRenderer();
    }
    if (lowerKey == QLatin1String("print")) {
        return new KReportPrivate::PrintRenderer();
    }
//! @todo port
#if 0
    if (lowerKey == QLatin1String("ods")) {
        return new KReportKSpreadRenderer();
    }
#endif
    if (lowerKey == QLatin1String("htmltable")) {
        return new KReportPrivate::HTMLTableRenderer();
    }
    if (lowerKey == QLatin1String("htmlcss")) {
        return new KReportPrivate::HTMLCSSRenderer();
    }
//! @todo port
#if 0
    if (lowerKey == QLatin1String("odttable") || lowerKey == QLatin1String("odt")) {
        return new KReportODTRenderer();
    }
    if (lowerKey == QLatin1String("odtframes")) {
        return new KReportOdtFrameReportRenderer();
    }
#endif
    return 0;
}
