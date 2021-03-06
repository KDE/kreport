/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
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

#include "KReportScreenRenderer_p.h"
#include "KReportRenderObjects.h"
#include "KReportUnit.h"
#include "kreport_debug.h"

namespace KReportPrivate {

ScreenRenderer::ScreenRenderer()
{

}

ScreenRenderer::~ScreenRenderer()
{
}

bool ScreenRenderer::render(const KReportRendererContext& context, ORODocument *document, int page)
{
    if (!document)
        return false;

    if (!context.painter())
        return false;

    OROPage *p = document->page(page);

    if (!p) {
        return false;
    }

    // Render Page Objects
    for (int i = 0; i < p->primitiveCount(); i++) {
        OROPrimitive *prim = p->primitive(i);

        if (OROTextBox *tb = dynamic_cast<OROTextBox*>(prim)) {
            QPointF ps = tb->position();
            QSizeF sz = tb->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            context.painter()->save();
            //Background

            context.painter()->setBackgroundMode(Qt::TransparentMode);

            QColor bg = tb->textStyle().backgroundColor;
            bg.setAlphaF(0.01 * tb->textStyle().backgroundOpacity);

            context.painter()->fillRect(rc, bg);

            //Text
            context.painter()->setFont(tb->textStyle().font);
            context.painter()->setPen(tb->textStyle().foregroundColor);
            context.painter()->drawText(rc.adjusted(2, 2, 0, 0), tb->flags(), tb->text());

            //outer line
            context.painter()->setPen(QPen(tb->lineStyle().color(), tb->lineStyle().weight(), tb->lineStyle().penStyle()));
            context.painter()->drawRect(rc);

            //Reset back to defaults for next element
            context.painter()->restore();
        }
        else if (OROLine *ln = dynamic_cast<OROLine*>(prim)) {
            QPointF s = ln->startPoint();
            QPointF e = ln->endPoint();
            //QPen pen ( _painter->pen() );
            QPen pen(ln->lineStyle().color(), ln->lineStyle().weight(), ln->lineStyle().penStyle());

            context.painter()->save();
            context.painter()->setRenderHint(QPainter::Antialiasing, true);
            context.painter()->setPen(pen);
            context.painter()->drawLine(QLineF(s.x(), s.y(), e.x(), e.y()));
            context.painter()->setRenderHint(QPainter::Antialiasing, false);
            context.painter()->restore();
        }
        else if (ORORect *re = dynamic_cast<ORORect*>(prim)) {
            QPointF ps = re->position();
            QSizeF sz = re->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            context.painter()->save();
            context.painter()->setPen(re->pen());
            context.painter()->setBrush(re->brush());
            context.painter()->drawRect(rc);
            context.painter()->restore();
        }
        else if (OROEllipse *re = dynamic_cast<OROEllipse*>(prim)) {
            QPointF ps = re->position();
            QSizeF sz = re->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            context.painter()->save();
            context.painter()->setPen(re->pen());
            context.painter()->setBrush(re->brush());
            context.painter()->drawEllipse(rc);
            context.painter()->restore();
        }
        else if ( OROImage *im = dynamic_cast<OROImage*>(prim)) {
            QPointF ps = im->position();
            QSizeF sz = im->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            QImage img = im->image();
            if (im->isScaled())
                img = img.scaled(rc.size().toSize(), (Qt::AspectRatioMode) im->aspectRatioMode(),
                                 (Qt::TransformationMode) im->transformationMode());

            QRectF sr = QRectF(QPointF(0.0, 0.0), rc.size().boundedTo(img.size()));
            context.painter()->drawImage(rc.topLeft(), img, sr);
        }
        else if (OROPicture *im = dynamic_cast<OROPicture*>(prim)) {
            QPointF ps = im->position();
            QSizeF sz = im->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());
            context.painter()->save();
            context.painter()->drawPicture(rc.topLeft(), *(im->picture()));
            context.painter()->restore();
        }
        else if (OROCheckBox *chk = dynamic_cast<OROCheckBox*>(prim)) {
            QPointF ps = chk->position();
            QSizeF sz = chk->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            context.painter()->save();

            context.painter()->setBackgroundMode(Qt::OpaqueMode);
            context.painter()->setRenderHint(QPainter::Antialiasing);

            context.painter()->setPen(chk->foregroundColor());

            if (chk->lineStyle().penStyle() == Qt::NoPen || chk->lineStyle().weight() <= 0) {
                context.painter()->setPen(QPen(Qt::lightGray));
            } else {
                context.painter()->setPen(QPen(chk->lineStyle().color(), chk->lineStyle().weight(), chk->lineStyle().penStyle()));
            }

            qreal ox = sz.width() / 5;
            qreal oy = sz.height() / 5;

            //Checkbox Style
            if (chk->checkType() == OROCheckBox::Type::Cross) {
                context.painter()->drawRoundedRect(rc, sz.width() / 10 , sz.height() / 10);

                if (chk->value()) {
                    QPen lp;
                    lp.setColor(chk->foregroundColor());
                    lp.setWidth(ox > oy ? oy : ox);
                    context.painter()->setPen(lp);
                    context.painter()->drawLine(QPointF(ox, oy) + ps, QPointF(sz.width() - ox, sz.height() - oy) + ps);
                    context.painter()->drawLine(QPointF(ox, sz.height() - oy) + ps, QPoint(sz.width() - ox, oy) + ps);
                }
            }
            else if (chk->checkType() == OROCheckBox::Type::Dot) {
                //Radio Style
                context.painter()->drawEllipse(rc);

                if (chk->value()) {
                    QBrush lb(chk->foregroundColor());
                    context.painter()->setBrush(lb);
                    context.painter()->setPen(Qt::NoPen);
                    context.painter()->drawEllipse(rc.center(), sz.width() / 2 - ox, sz.height() / 2 - oy);
                }
            }
            else {
                //Tickbox Style
                context.painter()->drawRoundedRect(rc, sz.width() / 10 , sz.height() / 10);

                if (chk->value()) {
                    QPen lp;
                    lp.setColor(chk->foregroundColor());
                    lp.setWidth(ox > oy ? oy : ox);
                    context.painter()->setPen(lp);
                    context.painter()->drawLine(
                        QPointF(ox, sz.height() / 2) + ps,
                        QPointF(sz.width() / 2, sz.height() - oy) + ps);
                    context.painter()->drawLine(
                        QPointF(sz.width() / 2, sz.height() - oy) + ps,
                        QPointF(sz.width() - ox, oy) + ps);
                }
            }

            context.painter()->restore();
        }
        else {
            kreportWarning() << "unrecognized primitive type";
        }
    }

    return true;
}

}
