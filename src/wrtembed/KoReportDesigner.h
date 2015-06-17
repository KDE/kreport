/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC <info@openmfg.com>
 * Copyright (C) 2007-2008 by Adam Pigg <adam@piggz.co.uk>
 * Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
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
#ifndef REPORTDESIGNER_H
#define REPORTDESIGNER_H

#include <QWidget>

#include "krreportdata.h"
#include "KoReportData.h"

class KProperty;
class KPropertySet;

class KoReportItemBase;

class QGraphicsScene;
class QActionGroup;
class QGraphicsSceneContextMenuEvent;
class QString;

class KoZoomHandler;
class ReportSectionDetail;
class ReportSection;
class KReportUnit;
class ReportScene;
class ReportSceneView;
class ReportWriterSectionData;
class QAction;

//
// Class ReportDesigner
//     The ReportDesigner is the main widget for designing a report
//
class KREPORT_EXPORT KoReportDesigner : public QWidget
{
    Q_OBJECT
public:
    qreal m_pressX;
    qreal m_pressY;
    qreal m_releaseX;
    qreal m_releaseY;

    /**
    @brief Constructor that create a blank designer
    @param widget QWidget parent
    */
    explicit KoReportDesigner(QWidget *parent = 0);

    /**
    @brief Constructor that create a designer, and loads the report described in the QDomElement
    @param widget QWidget parent
    @param element Report structure XML element
    */
    KoReportDesigner(QWidget *, const QDomElement &data);

    /**
    @brief Desctructor
    */
    ~KoReportDesigner();

    /**
    @brief Sets the report data
    The report data interface contains functions to retrieve data
    and information about the fields.
    @param kodata Pointer to KoReportData instance
    */
    void setReportData(KoReportData* kodata);

    /**
    @brief Return a pointer to the reports data
    @return Pointer to report data
    */
    KoReportData *reportData(){return m_kordata;}

    /**
    @brief Return a pointer to the section specified
    @param section KRSectionData::Section enum value of the section to return
    @return Pointer to report section object, or 0 if no section exists
    */
    ReportSection* section(KRSectionData::Section) const;

    /**
    @brief Deletes the section specified
    @param section KRSectionData::Section enum value of the section to return
    */
    void removeSection(KRSectionData::Section);

    /**
    @brief Create a new section and insert it into the report
    @param section KRSectionData::Section enum value of the section to return
    */
    void insertSection(KRSectionData::Section);

    /**
    @brief Return a pointer to the detail section.
    The detail section contains the actual detail section and related group sections
    @return Pointer to detail section
    */
    ReportSectionDetail* detailSection() const {
        return m_detail;
    }

    /**
    @brief Sets the title of the reportData
    @param title Report Title
    */
    void setReportTitle(const QString &);

    /**
    @brief Sets the parameters for the display of the background gridpoints
    @param visible Grid visibility
    @param divisions Number of minor divisions between major points
    */
    void setGridOptions(bool visible, int divisions);

    /**
    @brief Return the title of the report
    */
    QString reportTitle() const;

    /**
    @brief Return an XML description of the report
    @return QDomElement describing the report definition
    */
    QDomElement document() const;

    /**
    @brief Return true if the design has been modified
    @return modified status
    */
    bool isModified() const;

    /**
    @return a list of field names in the selected KoReportData
    */
    QStringList fieldNames() const;

    /**
    @return a list of field keys in the selected KoReportData
    The keys can be used to reference the names
    */
    QStringList fieldKeys() const;

    /**
    @brief Calculate the width of the page in pixels given the paper size, orientation, dpi and margin
    @return integer value of width in pixels
    */
    int pageWidthPx() const;

    /**
    @return the scene (section) that is currently active
    */
    QGraphicsScene* activeScene() const;

    /**
    @brief Sets the active Scene
    @param scene The scene to make active
    */
    void setActiveScene(QGraphicsScene* scene);

    /**
    @return the property set for the general report properties
    */
    KPropertySet* propertySet() const {
        return m_set;
    }

    /**
    @brief Give a hint on the size of the widget
    */
    virtual QSize sizeHint() const;

    /**
    @brief Return a pointer to the zoom handler
    */
    KoZoomHandler* zoomHandler() const;

    /**
    @brief Return the current unit assigned to the report
    */
    KReportUnit pageUnit() const;

    /**
    @brief Handle the context menu event for a report section
    @param scene The associated scene (section)
    */
    void sectionContextMenuEvent(ReportScene *, QGraphicsSceneContextMenuEvent * e);

    /**
    @brief Handle the mouse release event for a report section
    */
    void sectionMouseReleaseEvent(ReportSceneView *, QMouseEvent * e);

    void sectionMousePressEvent(ReportSceneView *, QMouseEvent * e);

    /**
    @brief Sets the property set for the currently selected item
    @param set Property set of item
    */
    void changeSet(KPropertySet *);

    /**
    @brief Return the property set for the curently selected item
    */
    KPropertySet* itemPropertySet() const {
        return m_itmset;
    }

    /**
    @brief Sets the modified status, defaulting to true for modified
    @param modified Modified status
    */
    void setModified(bool = true);

    /**
    @brief Return a unique name that can be used by the entity
    @param entity Name of entity
    */
    QString suggestEntityName(const QString &) const;

    /**
    @brief Checks if the supplied name is unique among all entities
    */
    bool isEntityNameUnique(const QString &, KoReportItemBase* = 0) const;

    /**
    @brief Returns a list of actions that represent the entities that can be inserted into the report.
    Actions are created as children of @a group and belong to the group.
    @return list of actions */
    static QList<QAction*> itemActions(QActionGroup* group = 0);

    /**
    @brief Populates the toolbar with actions that can be applied to the report
    Actions are created as children of @a group and belong to the group.
    @return list of actions */
    QList<QAction*> designerActions();

    /**
    @return X position of mouse when mouse press occurs
    */
    qreal getSelectionPressX() const;

    /**
    @return Y position of mouse when mouse press occurs
    */
    qreal getSelectionPressY() const;

    /**
    @return difference between X position of mouse release and press
    */
    qreal countSelectionWidth() const;

    /**
    @return difference between Y position of mouse release and press
    */
    qreal countSelectionHeight() const;

    /**
    @return point that contains X,Y coordinates of mouse press
    */
    QPointF getPressPoint() const;

    /**
    @return point that contains X,Y coordinates of mouse press
    */
    QPointF getReleasePoint() const;

    void plugItemActions(const QList<QAction*> &actList);

public Q_SLOTS:

    void slotEditDelete();
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotEditPaste(QGraphicsScene *);

    void slotItem(const QString&);

    void slotSectionEditor();

    void slotRaiseSelected();
    void slotLowerSelected();

protected:

    ReportSection *m_reportHeader;
    ReportSection *m_pageHeaderFirst;
    ReportSection *m_pageHeaderOdd;
    ReportSection *m_pageHeaderEven;
    ReportSection *m_pageHeaderLast;
    ReportSection *m_pageHeaderAny;

    ReportSection *m_pageFooterFirst;
    ReportSection *m_pageFooterOdd;
    ReportSection *m_pageFooterEven;
    ReportSection *m_pageFooterLast;
    ReportSection *m_pageFooterAny;
    ReportSection *m_reportFooter;
    ReportSectionDetail *m_detail;

private:
    class Private;
    Private * const d;

    void init();
    bool m_modified; // true if this document has been modified, false otherwise

    KoReportData *m_kordata;

    /**
    @brief Sets the detail section to the given section
    */
    void setDetail(ReportSectionDetail *rsd);

    /**
    @brief Deletes the detail section
    */
    void deleteDetail();

    virtual void resizeEvent(QResizeEvent * event);

    //Properties
    void createProperties();
    KPropertySet* m_set;
    KPropertySet* m_itmset;
    KProperty* m_title;
    KProperty* m_pageSize;
    KProperty* m_orientation;
    KProperty* m_unit;
    KProperty* m_customHeight;
    KProperty* m_customWidth;
    KProperty* m_leftMargin;
    KProperty* m_rightMargin;
    KProperty* m_topMargin;
    KProperty* m_bottomMargin;
    KProperty* m_showGrid;
    KProperty* m_gridDivisions;
    KProperty* m_gridSnap;
    KProperty* m_labelType;
    KProperty* m_interpreter;
    KProperty* m_script;

    ReportWriterSectionData *m_sectionData;
    unsigned int selectionCount() const;

    void setSectionCursor(const QCursor&);
    void unsetSectionCursor();

    //Actions
    QAction *m_editCutAction;
    QAction *m_editCopyAction;
    QAction *m_editPasteAction;
    QAction *m_editDeleteAction;
    QAction *m_sectionEdit;
    QAction *m_parameterEdit;
    QAction *m_itemRaiseAction;
    QAction *m_itemLowerAction;

    void createActions();

private Q_SLOTS:
    void slotPropertyChanged(KPropertySet &s, KProperty &p);

    /**
    @brief When the 'page' button in the top left is pressed, change the property set to the reports properties.
    */
    void slotPageButton_Pressed();

    void slotItemTriggered(bool checked);

Q_SIGNALS:
    void pagePropertyChanged(KPropertySet &s);
    void propertySetChanged();
    void dirty();
    void reportDataChanged();
    void itemInserted(const QString& entity);
};

#endif
