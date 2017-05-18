/* This file is part of the KDE project
 * Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)
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

#ifndef KREPORTSCRIPTCHECK_H
#define KREPORTSCRIPTCHECK_H

#include <QObject>

#include "KReportItemCheck.h"

namespace Scripting
{
    /**
     @brief Checkbox item script interface

     The user facing interface for scripting report checkbox items
     */
    class CheckBox : public QObject
    {
        Q_OBJECT
        public:
            explicit CheckBox(KReportItemCheckBox *);

            ~CheckBox() override;

        public Q_SLOTS:

            //! @return the value of the checkbox
            bool value() const;
            //! Set the value of the checkbox, defaults to checked if no value is given
            void setValue(bool val = true);

            //! @return the style of the checkbox as Cross, Tick or Dot
            QString checkStyle() const;
            //!Sets the style of the checkbox, valid values are Cross, Tick or Dot
            void setCheckStyle(const QString&);

            //! @return the foreground (text) color of the text-item
            QColor foregroundColor() const;

            //! Sets the foreground (text) color of the text-item to the given color
            void setForegroundColor(const QColor&);

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

        private:
            KReportItemCheckBox *m_check;
    };

}
#endif // KREPORTSCRIPTCHECK_H
