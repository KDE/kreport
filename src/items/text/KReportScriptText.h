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
#ifndef SCRIPTINGKRSCRIPTTEXT_H
#define SCRIPTINGKRSCRIPTTEXT_H

#include <QObject>

#include "KReportItemText.h"

namespace Scripting
{

/**
@brief Text item script interface

The user facing interface for scripting report text items
*/
class Text : public QObject
{
    Q_OBJECT
public:
    explicit Text(KReportItemText*);

    ~Text() override;
public Q_SLOTS:

    //! @return the source (column) that the text-item gets its data from*
    QString source() const;

    //! Sets the source (column) for the text-item.
    //! Valid values include a column name, fixed string if prefixed with '$'
    //! or a valid script expression if prefixed with a '='
    void setSource(const QString&);

    //! @return the horizontal alignment as an integer
    //! Valid values are left: -1, center: 0, right; 1
    int horizontalAlignment() const;

    //! Sets the horizontal alignment
    //! Valid values for alignment are left: -1, center: 0, right; 1
    void setHorizonalAlignment(int);

    //! @return the vertical alignment
    //! Valid values are top: -1, middle: 0, bottom: 1
    int verticalAlignment() const;

    //! Sets the vertical alignment
    //! Valid values for aligmnt are top: -1, middle: 0, bottom: 1
    void setVerticalAlignment(int);

    //! @return the background color of the lable
    QColor backgroundColor() const;

    //! Set the background color of the text-item to the given color
    void setBackgroundColor(const QColor&);

    //! @return the foreground (text) color of the text-item
    QColor foregroundColor() const;

    //! Sets the foreground (text) color of the text-item to the given color
    void setForegroundColor(const QColor&);

    //! @return the opacity of the text-item
    int backgroundOpacity() const;

    //! Sets the background opacity of the text-item
    //! Valid values are in the range 0-100
    void setBackgroundOpacity(int);

    //! @return the border line color of the text-item
    QColor lineColor() const;

    //! Sets the border line color of the text-item to the given color
    void setLineColor(const QColor&);

    //! @return the border line weight (thickness) of the text-item
    int lineWeight() const;

    //! Sets the border line weight (thickness) of the text-item
    void setLineWeight(int);

    //! @return the border line style of the text-item.  Values are from Qt::Penstyle range 0-5
    int lineStyle() const;

    //! Sets the border line style of the text-item to the given style in the range 0-5
    void setLineStyle(int);

    //! @returns the position of the text-item in points
    QPointF position() const;

    //! Sets the position of the text-item to the given point coordinates
    void setPosition(const QPointF&);

    //! @returns the size of the text-item in points
    QSizeF size() const;

    //! Sets the size of the text-item to the given size in points
    void setSize(const QSizeF&);

    //!Load the contets for the text item from the given file
    void loadFromFile(const QString& fileName);
private:
    KReportItemText *m_text;
};

}

#endif
