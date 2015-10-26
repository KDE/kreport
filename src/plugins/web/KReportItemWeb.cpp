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

#include "KoReportItemWeb.h"
#include <KReportRenderObjects.h>

#include <KProperty>
#include <KPropertySet>

#include <QGraphicsRectItem>
#include <QUrl>
#include <QWebPage>
#include "kreportplugin_debug.h"

KReportItemWeb::KReportItemWeb(): m_rendering(false)
{
    createProperties();
    init();
}

KReportItemWeb::KReportItemWeb(QDomNode *element)
{
    createProperties();
    init();
    QDomNodeList nl = element->childNodes();
    QString n;
    QDomNode node;
    QDomElement e = element->toElement();

    m_controlSource->setValue(element->toElement().attribute("report:item-data-source"));
    m_name->setValue(element->toElement().attribute("report:name"));
    Z = element->toElement().attribute("report:z-index").toDouble();
    parseReportRect(element->toElement(), &m_pos, &m_size);
    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();
    }
}

void KReportItemWeb::init()
{
    m_webPage = new QWebPage();
    connect(m_webPage, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
}

void KReportItemWeb::createProperties()
{
    m_set = new KPropertySet(0, "web");

    m_controlSource = new KProperty("item-data-source", QStringList(),
                                    QStringList(), QString(), tr("Data Source"));
    m_set->addProperty(m_controlSource);
    addDefaultProperties();
}

KReportItemWeb::~KReportItemWeb()
{
    delete m_set;
}
QString KReportItemWeb::typeName() const
{
    return "web";
}

void KReportItemWeb::loadFinished(bool)
{
    kreportpluginDebug() << m_rendering;
    if (m_rendering) {
        OROPicture * pic = new OROPicture();
        m_webPage->setViewportSize(m_size.toScene().toSize());
        m_webPage->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
        m_webPage->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);

        QPainter p(pic->picture());

        m_webPage->mainFrame()->render(&p);

        QPointF pos = m_pos.toScene();
        QSizeF size = m_size.toScene();

        pos += m_targetOffset;

        pic->setPosition(pos);
        pic->setSize(size);
        if (m_targetPage) m_targetPage->addPrimitive(pic, false, true);

        OROPicture *p2 = dynamic_cast<OROPicture*>(pic->clone());
        p2->setPosition(m_pos.toPoint());
        if (m_targetSection) m_targetSection->addPrimitive(p2);

        m_rendering = false;
        emit(finishedRendering());
    }
}

int KReportItemWeb::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                      const QVariant &data, KReportScriptHandler *script)
{
    Q_UNUSED(script);

    m_rendering = true;

    kreportpluginDebug() << data;

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
