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

#ifndef KRSCRIPTLABEL_H
#define KRSCRIPTLABEL_H

#include <QObject>

#include "KReportItemLabel.h"

/**
 @brief Label item script interface

 The user facing interface for scripting report label items
 */
namespace Scripting
{
class Label : public QObject
{
    Q_OBJECT
public:
    explicit Label(KReportItemLabel *);

    ~Label() override;

public Q_SLOTS:
    //!Return the label caption
    QString caption() const;
    //!Sets the label caption
    void setCaption(const QString&);

    //!Returns the horizontal alignment of the label, -1 = left, 0 = center, 1 = right
    int horizontalAlignment() const;
    //!Sets the horizontal alignment.  Valid values are -1, 0, 1
    void setHorizonalAlignment(int);

    //!Returns the vertical alignment of the label, -1 = left, 0 = center, 1 = right
    int verticalAlignment() const;
    //!Sets the vertical alignment.  Valid values are -1, 0, 1
    void setVerticalAlignment(int);

    //!Returns the background color of the label
    QColor backgroundColor() const;
    //!Sets the background color
    void setBackgroundColor(const QColor&);

    //!Returns the foreground (text) color of the label
    QColor foregroundColor() const;
    //!Srets the foreground (text) color
    void setForegroundColor(const QColor&);

    //!Returns the background opacity
    int backgroundOpacity() const;
    //!Sets the background opacity.  Values 0-100
    void setBackgroundOpacity(int);

    //!Returns the line color of the label
    QColor lineColor() const;
    //!Sets the line color of the label
    void setLineColor(const QColor&);

    //!Returns the line weight (width) of the label
    int lineWeight() const;
    //!Sets the line weight (width) of the label
    void setLineWeight(int);

    //! Returns the line style.  Valid values are those from Qt::PenStyle (0-5)
    int lineStyle() const;
    //! Srts the line style.  Valid values are those from Qt::PenStyle (0-5)
    void setLineStyle(int);

    //!Returns the position of the label within the parent section
    QPointF position() const;
    //!Sets the position of the label within the parent section
    void setPosition(const QPointF&);

    //!Returns the size of the label
    QSizeF size() const;
    //!Serts the size of the label
    void setSize(const QSizeF&);

private:
    KReportItemLabel *m_label;
};
}

#endif
