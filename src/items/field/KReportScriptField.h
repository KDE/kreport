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

#ifndef KRSCRIPTFIELD_H
#define KRSCRIPTFIELD_H

#include <QObject>

#include "KReportItemField.h"

/**
 @brief Field item script interface

 The user facing interface for scripting report field items
 */
namespace Scripting
{
class Field : public QObject
{
    Q_OBJECT
public:
    explicit Field(KReportItemField*);

    ~Field();

public Q_SLOTS:
    //!Returns the source (column) that the field gets its data from*
    QString source() const;
    //!Sets the source (column) for the field
    void setSource(const QString&);

    //!Returns the horizontal alignment of the field, -1 = left, 0 = center, 1 = right
    int horizontalAlignment() const;
    //!Sets the horizontal alignment.  Valid values are -1, 0, 1
    void setHorizonalAlignment(int);

    //!Returns the vertical alignment of the field, -1 = left, 0 = center, 1 = right
    int verticalAlignment() const;
    //!Sets the vertical alignment.  Valid values are -1, 0, 1
    void setVerticalAlignment(int);

    //!Returns the background color of the field
    QColor backgroundColor() const;
    //!Sets the background color
    void setBackgroundColor(const QColor&);

    //!Returns the foreground (text) color of the field
    QColor foregroundColor() const;
    //!Srets the foreground (text) color
    void setForegroundColor(const QColor&);

    //!Returns the background opacity
    int backgroundOpacity() const;
    //!Sets the background opacity.  Values 0-100
    void setBackgroundOpacity(int);

    //!Returns the line color of the field
    QColor lineColor() const;
    //!Sets the line color of the field
    void setLineColor(const QColor&);

    //!Returns the line weight (width) of the field
    int lineWeight() const;
    //!Sets the line weight (width) of the field
    void setLineWeight(int);

    //! Returns the line style.  Valid values are those from Qt::PenStyle (0-5)
    int lineStyle() const;
    //! Srts the line style.  Valid values are those from Qt::PenStyle (0-5)
    void setLineStyle(int);

    //!Returns the position of the field within the parent section
    QPointF position() const;
    //!Sets the position of the field within the parent section
    void setPosition(const QPointF&);

    //!Returns the size of the field
    QSizeF size() const;
    //!Serts the size of the field
    void setSize(const QSizeF&);

private:
    KReportItemField *m_field;

};
}
#endif
