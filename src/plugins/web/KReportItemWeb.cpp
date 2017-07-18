/* This file is part of the KDE project
   Copyright Shreya Pandit <shreya@shreyapandit.com>
   Copyright 2011 Adam Pigg <adam@piggz.co.uk>

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

#include "KReportItemWeb.h"
#include <KReportRenderObjects.h>

#include <KPropertyListData>
#include <KPropertySet>

#include <QGraphicsRectItem>
#include <QUrl>
#include <QWebPage>
#include <QWebFrame>
#include <QPainter>

#include "kreportplugin_debug.h"

KReportItemWeb::KReportItemWeb() : m_rendering(false), m_targetPage(nullptr)
{
    createProperties();
    m_webPage = new QWebPage();
    connect(m_webPage, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
}

KReportItemWeb::KReportItemWeb(const QDomNode &element)
    : KReportItemWeb()
{
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;
    QDomElement e = element.toElement();

    m_controlSource->setValue(element.toElement().attribute(QLatin1String("report:item-data-source")));
    nameProperty()->setValue(element.toElement().attribute(QLatin1String("report:name")));
    setZ(element.toElement().attribute(QLatin1String("report:z-index")).toDouble());
    parseReportRect(element.toElement());
    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();
    }
}

void KReportItemWeb::createProperties()
{
    m_controlSource
        = new KProperty("item-data-source", new KPropertyListData, QVariant(), tr("Data Source"));
    propertySet()->addProperty(m_controlSource);
}

KReportItemWeb::~KReportItemWeb()
{
}

QString KReportItemWeb::typeName() const
{
    return QLatin1String("web");
}

void KReportItemWeb::loadFinished(bool)
{
    //kreportpluginDebug() << m_rendering;
    if (m_rendering) {
        OROPicture * pic = new OROPicture();
        m_webPage->setViewportSize(sceneSize(size()).toSize());
        m_webPage->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
        m_webPage->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);

        QPainter p(pic->picture());

        m_webPage->mainFrame()->render(&p);

        QPointF pos = scenePosition(position());
        QSizeF siz = sceneSize(size());

        pos += m_targetOffset;

        pic->setPosition(pos);
        pic->setSize(siz);
        if (m_targetPage) m_targetPage->insertPrimitive(pic);

        OROPicture *p2 = dynamic_cast<OROPicture*>(pic->clone());
        if (p2) {
            p2->setPosition(scenePosition(position()));
            if (m_targetSection) {
                m_targetSection->addPrimitive(p2);
            }
        }

        m_rendering = false;
        emit(finishedRendering());
    }
}

int KReportItemWeb::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                      const QVariant &data, KReportScriptHandler *script)
{
    Q_UNUSED(script);

    m_rendering = true;

    //kreportpluginDebug() << data;

    m_targetPage = page;
    m_targetSection = section;
    m_targetOffset = offset;

    QUrl url = QUrl::fromUserInput(data.toString());
    if (url.isValid()) {
        m_webPage->mainFrame()->load(url);
    } else {
        m_webPage->mainFrame()->setHtml(data.toString());
    }

    return 0; //Item doesn't stretch the section height
}

QString KReportItemWeb::itemDataSource() const
{
    return m_controlSource->value().toString();
}
