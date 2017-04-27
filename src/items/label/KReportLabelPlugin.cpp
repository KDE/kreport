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

#include "KReportLabelPlugin.h"
#include "KReportItemLabel.h"
#include "KReportDesignerItemLabel.h"
#include "KReportDesigner.h"
#include "KReportPluginMetaData.h"
#include "KReportLabelElement.h"
#include "KReportLineStyle.h"
#include "KReportUtils.h"

#ifdef KREPORT_SCRIPTING
#include "KReportScriptLabel.h"
#endif

#include <QDomElement>

KREPORT_PLUGIN_FACTORY(KReportLabelPlugin, "label.json")

KReportLabelPlugin::KReportLabelPlugin(QObject *parent, const QVariantList &args)
    : KReportPluginInterface(parent, args)
{
}

KReportLabelPlugin::~KReportLabelPlugin()
{

}

QObject* KReportLabelPlugin::createRendererInstance(const QDomNode &elem)
{
    return new KReportItemLabel(elem);
}

QObject* KReportLabelPlugin::createDesignerInstance(KReportDesigner* designer, QGraphicsScene* scene, const QPointF &pos)
{
    return new KReportDesignerItemLabel(designer, scene, pos);
}

QObject* KReportLabelPlugin::createDesignerInstance(const QDomNode & element, KReportDesigner *designer, QGraphicsScene * scene)
{
    return new KReportDesignerItemLabel(element, designer, scene);
}

KReportElement KReportLabelPlugin::createElement()
{
    return KReportLabelElement();
}

bool KReportLabelPlugin::loadElement(KReportElement *el, const QDomElement &dom, KReportDesignReadingStatus *status)
{
    if (!KReportPluginInterface::loadElement(el, dom, status)) {
        return false;
    }
    KReportLabelElement label(*el);
    label.setText(KReportUtils::attr(dom, "report:caption", QString()));
    QString s = KReportUtils::attr(dom, "report:horizontal-align", QString());
    Qt::Alignment alignment = KReportUtils::horizontalAlignment(s, label.alignment() & Qt::AlignHorizontal_Mask);
    s = KReportUtils::attr(dom, "report:vertical-align", QString());
    alignment |= KReportUtils::verticalAlignment(s, label.alignment() & Qt::AlignVertical_Mask);
    label.setAlignment(alignment);

    const QDomElement textStyleDom = dom.firstChildElement(QLatin1String("report:text-style"));
    QFont font(label.font());
    KReportUtils::readFontAttributes(textStyleDom, &font);
    label.setFont(font);

    const QDomElement lineStyleDom = dom.firstChildElement(QLatin1String("report:line-style"));
    KReportLineStyle borderStyle(label.borderStyle());
    s = KReportUtils::attr(lineStyleDom, "report:line-style", QString());
    borderStyle.setPenStyle(KReportUtils::penStyle(s, borderStyle.penStyle()));
    borderStyle.setColor(KReportUtils::attr(lineStyleDom, "report:line-color", borderStyle.color()));
    // border-line-width could be better name but it's too late...
    borderStyle.setWeight(KReportUtils::attr(lineStyleDom, "report:line-weight", borderStyle.weight()));
    label.setBorderStyle(borderStyle);
    return true;
}

#ifdef KREPORT_SCRIPTING
QObject* KReportLabelPlugin::createScriptInstance(KReportItemBase *item)
{
    KReportItemLabel *label = dynamic_cast<KReportItemLabel*>(item);
    if (label) {
        return new Scripting::Label(label);
    }
    return nullptr;
}
#endif

#include "KReportLabelPlugin.moc"
