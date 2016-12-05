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

#include "KReportScriptDraw.h"
#include "KReportRenderObjects.h"
#include "KReportPosition.h"
#include "KReportSize.h"

#include <QFont>
#include <QFontMetrics>

KReportScriptDraw::KReportScriptDraw(QObject *parent)
        : QObject(parent)
{
    m_curPage = 0;
}


KReportScriptDraw::~KReportScriptDraw()
{
}

void KReportScriptDraw::setPage(OROPage *p)
{
    m_curPage = p;
}

void KReportScriptDraw::setOffset(QPointF off)
{
    m_curOffset = off;
}

void KReportScriptDraw::rectangle(qreal x, qreal y, qreal w, qreal h, const QString& lc, const QString& fc, qreal lw, int a)
{
    if (m_curPage) {
        ORORect *r = new ORORect();
        KReportPosition p;
        KReportSize s;

        p.setPointPos(QPointF(x, y));
        s.setPointSize(QSizeF(w, h));
        r->setRect(QRectF(p.toScene() + m_curOffset, s.toScene()));

        QPen pen(QColor(lc), lw);
        QColor c(fc);
        c.setAlpha(a);
        QBrush bru(c);

        r->setBrush(bru);
        r->setPen(pen);
        m_curPage->insertPrimitive(r);
    }
}

void KReportScriptDraw::ellipse(qreal x, qreal y, qreal w, qreal h, const QString& lc, const QString& fc, qreal lw, int a)
{
    if (m_curPage) {
        OROEllipse *e = new OROEllipse();
        KReportPosition p;
        KReportSize s;

        p.setPointPos(QPointF(x, y));
        s.setPointSize(QSizeF(w, h));
        e->setRect(QRectF(p.toScene() + m_curOffset, s.toScene()));

        QPen pen(QColor(lc), lw);
        QColor c(fc);
        c.setAlpha(a);
        QBrush bru(c);

        e->setBrush(bru);
        e->setPen(pen);
        m_curPage->insertPrimitive(e);
    }
}

void KReportScriptDraw::line(qreal x1, qreal y1, qreal x2, qreal y2, const QString& lc)
{
    if (m_curPage) {
        OROLine *ln = new OROLine();
        KReportPosition s;
        KReportPosition e;

        s.setPointPos(QPointF(x1, y1));
        e.setPointPos(QPointF(x2, y2));

        ln->setStartPoint(s.toScene() + m_curOffset);
        ln->setEndPoint(e.toScene() + m_curOffset);

        KReportLineStyle ls;
        ls.setColor(QColor(lc));
        ls.setWidth(1);
        ls.setPenStyle(Qt::SolidLine);

        ln->setLineStyle(ls);
        m_curPage->insertPrimitive(ln);
    }
}

void KReportScriptDraw::text(qreal x, qreal y, const QString &txt, const QString &fnt, int pt, const QString &fc, const QString&bc, const QString &lc, qreal lw, int o)
{
    if (m_curPage) {
        QFont f(fnt, pt);
        QRectF r = QFontMetrics(f).boundingRect(txt);

        KRTextStyleData ts;
        ts.font = f;
        ts.backgroundColor = QColor(bc);
        ts.foregroundColor = QColor(fc);
        ts.backgroundOpacity = o;

        KReportLineStyle ls;
        ls.setColor(QColor(lc));
        ls.setWidth(lw);
        if (lw <= 0)
            ls.setPenStyle(Qt::NoPen);
        else
            ls.setPenStyle(Qt::SolidLine);


        OROTextBox *tb = new OROTextBox();
        tb->setPosition(QPointF(x, y) + m_curOffset);
        tb->setSize(r.size());
        tb->setTextStyle(ts);
        tb->setLineStyle(ls);

        tb->setText(txt);

        m_curPage->insertPrimitive(tb);

    }
}

