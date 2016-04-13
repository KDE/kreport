/* This file is part of the KDE project
 * Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
 * Copyright (C) 2006 Peter Simonsson <peter.simonsson@gmail.com>
 * Copyright (C) 2007 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KREPORTRULER_P_H
#define KREPORTRULER_P_H

#include "KReportUnit.h"
#include <QWidget>
#include <QTextOption>

class KReportZoomHandler;
class KReportRulerPrivate;

class RulerTabChooser : public QWidget
{
public:
    RulerTabChooser(QWidget *parent) : QWidget(parent), m_type(QTextOption::LeftTab), m_showTabs(false) {}
    virtual ~RulerTabChooser() {}

    inline QTextOption::TabType type() {return m_type;}
    void setShowTabs(bool showTabs) { if (m_showTabs == showTabs) return; m_showTabs = showTabs; update(); }
    void mousePressEvent(QMouseEvent *);

    void paintEvent(QPaintEvent *);

private:
    QTextOption::TabType m_type;
    bool m_showTabs :1;
};

class PaintingStrategy
{
public:
    /// constructor
    PaintingStrategy() {}
    /// destructor
    virtual ~PaintingStrategy() {}

    /**
     * Draw the background of the ruler.
     * @param ruler the ruler to draw on.
     * @param painter the painter we can paint with.
     */
    virtual QRectF drawBackground(const KReportRulerPrivate *ruler, QPainter *painter) = 0;

    /**
     * Draw the indicators for text-tabs.
     * @param ruler the ruler to draw on.
     * @param painter the painter we can paint with.
     */
    virtual void drawTabs(const KReportRulerPrivate *ruler, QPainter *painter) = 0;

    /**
     * Draw the indicators for the measurements which typically are drawn every [unit].
     * @param ruler the ruler to draw on.
     * @param painter the painter we can paint with.
     * @param rectangle
     */
    virtual void drawMeasurements(const KReportRulerPrivate *ruler, QPainter *painter, const QRectF &rectangle) = 0;

    /**
     * Draw the indicators for the indents of a text paragraph
     * @param ruler the ruler to draw on.
     * @param painter the painter we can paint with.
     */
    virtual void drawIndents(const KReportRulerPrivate *ruler, QPainter *painter) = 0;

    /**
     *returns the size suggestion for a ruler with this strategy.
     */
    virtual QSize sizeHint() = 0;
};

class HorizontalPaintingStrategy : public PaintingStrategy
{
public:
    HorizontalPaintingStrategy() : lengthInPixel(1) {}

    virtual QRectF drawBackground(const KReportRulerPrivate *ruler, QPainter *painter);
    virtual void drawTabs(const KReportRulerPrivate *ruler, QPainter *painter);
    virtual void drawMeasurements(const KReportRulerPrivate *ruler, QPainter *painter, const QRectF &rectangle);
    virtual void drawIndents(const KReportRulerPrivate *ruler, QPainter *painter);
    virtual QSize sizeHint();

private:
    qreal lengthInPixel;
};

class VerticalPaintingStrategy : public PaintingStrategy
{
public:
    VerticalPaintingStrategy() : lengthInPixel(1) {}

    virtual QRectF drawBackground(const KReportRulerPrivate *ruler, QPainter *painter);
    virtual void drawTabs(const KReportRulerPrivate *, QPainter *) {}
    virtual void drawMeasurements(const KReportRulerPrivate *ruler, QPainter *painter, const QRectF &rectangle);
    virtual void drawIndents(const KReportRulerPrivate *, QPainter *) { }
    virtual QSize sizeHint();

private:
    qreal lengthInPixel;
};

class HorizontalDistancesPaintingStrategy : public HorizontalPaintingStrategy
{
public:
    HorizontalDistancesPaintingStrategy() {}

    virtual void drawMeasurements(const KReportRulerPrivate *ruler, QPainter *painter, const QRectF &rectangle);

private:
    void drawDistanceLine(const KReportRulerPrivate *d, QPainter *painter, qreal start, qreal end);
};

/**
 * Decorator widget to draw a single ruler around a canvas.
 */
class KReportRuler : public QWidget
{
Q_OBJECT
public:
    /**
     * Creates a ruler with the orientation @p orientation
     * @param parent parent widget
     * @param orientation the orientation of the ruler
     * @param viewConverter the view converter used to convert from point to pixel
     */
    KReportRuler(QWidget* parent, Qt::Orientation orientation, const KReportZoomHandler* viewConverter);
    ~KReportRuler();

    /// For paragraphs each tab definition is represented by this struct.
    struct Tab {
        inline Tab(qreal aPosition, QTextOption::TabType aType) : position(aPosition), type(aType) {}
        inline Tab() : position(0.0), type(QTextOption::LeftTab) {}
        qreal position;            ///< distance in point from the start of the text-shape
        QTextOption::TabType type; ///< Determine which type is used.
    };

    /// The ruler's unit
    KReportUnit unit() const;

    /// The length of the ruler in points (pt)
    qreal rulerLength() const;

    /// The orientation of the ruler
    Qt::Orientation orientation() const;

    /// The start indent of the first line
    qreal firstLineIndent() const;

    /// The start indent of the rest of the lines
    qreal paragraphIndent() const;

    /// The end indent of all lines
    qreal endIndent() const;

    /// The tab chooser widget, which you must put into a layout along with the ruler.
    /// Returns 0 for vertical rulers,
    QWidget *tabChooser();

    /**
     * set a list of actions that will be shown in a popup should the user right click on this ruler.
     * @param popupActionList the list of actions
     * @see popupActionList()
     */
    void setPopupActionList(const QList<QAction*> &popupActionList);
    /**
     * Return the actions list.
     * @see setPopupActionList()
     */
    QList<QAction*> popupActionList() const;

    /// reimplemented
    virtual QSize minimumSizeHint() const;

    /// reimplemented
    virtual QSize sizeHint() const;

public Q_SLOTS:
    /// Set the unit of the ruler
    void setUnit(const KReportUnit &unit);

    /** Set the offset. Use this function to sync the ruler with
      * the canvas' position on screen
      * @param offset The offset in pixels
      */
    void setOffset(int offset);

    /// Sets the length of the ruler to @p length in points (pt)
    void setRulerLength(qreal length);

    /** Set the active range, ie the part of the ruler that is most likely used.
      * set to 0, 0 when there is no longer any active range
      * @param start the start of the range in pt
      * @param end the end of the range in pt
      */
    void setActiveRange(qreal start, qreal end);

    /** Set the override active range, ie the part of the ruler that is most likely used.
      * set to 0, 0 when there is no longer any active range
      * The override, means that if set it takes precedence over the normal active range.
      * @param start the start of the range in pt
      * @param end the end of the range in pt
      */
    void setOverrideActiveRange(qreal start, qreal end);

    /** Set the state of the ruler so that it shows everything in right to left mode.
      * @param isRightToLeft state of right to left mode. Default is false.
      */
    void setRightToLeft(bool isRightToLeft);

    /** Set if the ruler should show indents as used in textditors.
      * Set the indents with setFirstLineIndent(), setParagraphIndent(), setEndIndent() .
      * @param show show indents if true. Default is false.
      */
    void setShowIndents(bool show);

    /** Set the position of the first line start indent relative to the active range.
      * If Right To left is set the indent is relative to the right side of the active range .
      * @param indent the value relative to the active range.
      */
    void setFirstLineIndent(qreal indent);

    /** Set the position of the rest of the lines start indent relative to the active range.
      * If Right To left is set the indent is relative to the right side of the active range .
      * @param indent the value relative to the active range.
      */
    void setParagraphIndent(qreal indent);

    /** Set the position of the end indent relative to the active range.
      * If Right To left is set the indent is relative to the left side of the active range .
      * @param indent the value relative to the active range.
      */
    void setEndIndent(qreal indent);

    /** Set whether the ruler should show the current mouse position.
      * Update the position with updateMouseCoordinate().
      * @param show show mouse position if true. Default is false.
      */
    void setShowMousePosition(bool show);

    /** Update the current position of the mouse pointer, repainting if changed.
      * The ruler offset will be applied before painting.
      * @param coordinate Either the x or y coordinate of the mouse depending
      *                   of the orientation of the ruler.
      */
    void updateMouseCoordinate(int coordinate);

    /**
     * Set whether the ruler should show the selection borders
     * @param show show selection borders if true, default is false.
     */
    void setShowSelectionBorders(bool show);

    /**
     * Update the selection borders
     * @param first the first selection border in points
     * @param second the other selection border in points
     */
    void updateSelectionBorders(qreal first, qreal second);

    /**
     * Set whether the ruler should show tabs
     * @param show show selection borders if true, default is false.
     */
    void setShowTabs(bool show);

    /**
     * Set whether the tabs is relative to the paragraph indent
     * @param relative tabs are relative to pragraph indent if true, default is false.
     */
    void setRelativeTabs(bool relative);

    /**
     * Update the tabs
     * @param tabs a list of tabs that is shown on the ruler
     * @param tabDistance the distance between regular interval tabs
     */
    void updateTabs(const QList<Tab> &tabs, qreal tabDistance);

    /***
     * Return the list of tabs set on this ruler.
     */
    QList<Tab> tabs() const;

    /**
     * Clear all previously set hotspots.
     * A hotspot is a position on the ruler that the user can manipulate by dragging.
     */
    void clearHotSpots();

    /**
     * Add or set a hotspot.
     * A hotspot is a position on the ruler that the user can manipulate by dragging.
     * @param position the new position of the hotspot.
     * @param id the unique id for the hotspot. If the id has not been set before, it will be added.
     */
    void setHotSpot(qreal position, int id = -1);

    /**
     * Remove a previously set hotspot, returning true if one is actually returned.
     * @param id the unique id for the hotspot.
     * A hotspot is a position on the ruler that the user can manipulate by dragging.
     */
    bool removeHotSpot(int id);

Q_SIGNALS:
    /**
     * emitted when any of the indents is moved by the user.
     * @param final false until the user releases the mouse. So you can implement live update.
     */
    void indentsChanged(bool final);

    /**
     * Emitted when any of the tabs are moved, deleted or inserted by the user.
     * @param originalTabIndex the index in the list of tabs before the user interaction
     *          started, or -1 if this is a new tab
     * @param tab the new tab, or zero when the tab has been removed.
     */
    void tabChanged(int originalTabIndex, KReportRuler::Tab *tab);

    /// emitted when there the user is about to change a tab or hotspot
    void aboutToChange();

    void hotSpotChanged(int id, qreal newPosition);

    /// emitted when the mouse is drag+released outside the ruler
    void guideLineCreated(Qt::Orientation orientation, qreal viewPosition);

protected:
    /// reimplemented
    virtual void paintEvent(QPaintEvent* event);
    /// reimplemented
    virtual void mousePressEvent(QMouseEvent *ev);
    /// reimplemented
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    /// reimplemented
    virtual void mouseMoveEvent(QMouseEvent *ev);

private:
    KReportRulerPrivate * const d;
    friend class KReportRulerPrivate;
};


class KReportRulerPrivate
{
public:
    KReportRulerPrivate(KReportRuler *parent, const KReportZoomHandler *vc, Qt::Orientation orientation);
    ~KReportRulerPrivate();

    void emitTabChanged();

    KReportUnit unit;
    const Qt::Orientation orientation;
    const KReportZoomHandler * const viewConverter;

    int offset;
    qreal rulerLength;
    qreal activeRangeStart;
    qreal activeRangeEnd;
    qreal activeOverrideRangeStart;
    qreal activeOverrideRangeEnd;

    int mouseCoordinate;
    int showMousePosition;

    bool showSelectionBorders;
    qreal firstSelectionBorder;
    qreal secondSelectionBorder;

    bool showIndents;
    qreal firstLineIndent;
    qreal paragraphIndent;
    qreal endIndent;

    bool showTabs;
    bool relativeTabs;
    bool tabMoved; // set to true on first move of a selected tab
    QList<KReportRuler::Tab> tabs;
    int originalIndex; //index of selected tab before we started dragging it.
    int currentIndex; //index of selected tab or selected HotSpot - only valid when selected indicates tab or hotspot
    KReportRuler::Tab deletedTab;
    qreal tabDistance;

    struct HotSpotData {
        qreal position;
        int id;
    };
    QList<HotSpotData> hotspots;

    bool rightToLeft;
    enum Selection {
        None,
        Tab,
        FirstLineIndent,
        ParagraphIndent,
        EndIndent,
        HotSpot
    };
    Selection selected;
    int selectOffset;

    QList<QAction*> popupActions;

    RulerTabChooser *tabChooser;

    // Cached painting strategies
    PaintingStrategy * normalPaintingStrategy;
    PaintingStrategy * distancesPaintingStrategy;

    // Current painting strategy
    PaintingStrategy * paintingStrategy;

    KReportRuler *ruler;

    qreal numberStepForUnit() const;
    /// @return The rounding of value to the nearest multiple of stepValue
    qreal doSnapping(qreal value) const;
    Selection selectionAtPosition(const QPoint & pos, int *selectOffset = 0);
    int hotSpotIndex(const QPoint & pos);
    qreal effectiveActiveRangeStart() const;
    qreal effectiveActiveRangeEnd() const;

    friend class VerticalPaintingStrategy;
    friend class HorizontalPaintingStrategy;
};

#endif
