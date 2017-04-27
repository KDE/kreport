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

            //! Return the value of the checkbox
            bool value() const;
            //! Set the value of the checkbox, defaults to checked if no value is given
            void setValue(bool val = true);

            //! Returns the style of the checkbox as Cross, Tick or Dot
            QString checkStyle() const;
            //!Sets the style of the checkbox, valid values are Cross, Tick or Dot
            void setCheckStyle(const QString&);

            //!Returns the foreground color
            QColor foregroundColor() const;
            //!Sets the foreground color
            void setForegroundColor(const QColor&);

            //!Returs the line color
            QColor lineColor() const;
            //!Sets the line color
            void setLineColor(const QColor&);

            //!Returns the line weight (thickness)
            int lineWeight() const;
            //!Serts the line weight (thicknes)
            void setLineWeight(int);

            //! Returns the line style.  Valid values are those from Qt::PenStyle (0-5)
            int lineStyle() const;
            //! Srts the line style.  Valid values are those from Qt::PenStyle (0-5)
            void setLineStyle(int);

            //!Returns the position of the checkbox within the parent section
            QPointF position() const;
            //!Sets the position of the checkbox within the parent section
            void setPosition(const QPointF&);

            //!Returns the size of the checkbox
            QSizeF size() const;
            //!Serts the size of the checkbox
            void setSize(const QSizeF&);

        private:
            KReportItemCheckBox *m_check;
    };

}
#endif // KREPORTSCRIPTCHECK_H
