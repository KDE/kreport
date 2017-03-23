/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC <info@openmfg.com>
 * Copyright (C) 2007-2008 by Adam Pigg <adam@piggz.co.uk>
 * Copyright (C) 2011-2017 Jarosław Staniek <staniek@kde.org>
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
#ifndef KREPORTDESIGNER_H
#define KREPORTDESIGNER_H

#include <QWidget>

#include "KReportDocument.h"
#include "KReportDataSource.h"

class KProperty;
class KPropertySet;

class KReportItemBase;

class QGraphicsScene;
class QActionGroup;
class QGraphicsSceneContextMenuEvent;
class QString;

class KReportDesignerSectionDetail;
class KReportDesignerSection;
class KReportUnit;
class KReportDesignerSectionScene;
class KReportDesignerSectionView;
class QAction;

//
// Class ReportDesigner
//     The ReportDesigner is the main widget for designing a report
//
class KREPORT_EXPORT KReportDesigner : public QWidget
{
    Q_OBJECT
public:
    /**
    @brief Constructor that create a blank designer
    @param widget QWidget parent
    */
    explicit KReportDesigner(QWidget *parent = 0);

    /**
    @brief Constructor that create a designer, and loads the report described in the QDomElement
    @param widget QWidget parent
    @param element Report structure XML element
    */
    KReportDesigner(QWidget *, const QDomElement &data);

    /**
    @brief Desctructor
    */
    ~KReportDesigner();

    /**
    @brief Sets the report data
    The report data interface contains functions to retrieve data
    and information about the fields.
    @param kodata Pointer to KReportDataSource instance, ownership is transferred
    */
    void setDataSource(KReportDataSource* source);

    /**
    @brief Return a pointer to the reports data
    @return Pointer to report data
    */
    KReportDataSource *reportDataSource() const;

    /**
    @brief Return a pointer to the section specified
    @param section KReportSectionData::Section enum value of the section to return
    @return Pointer to report section object, or 0 if no section exists
    */
    KReportDesignerSection* section(KReportSectionData::Section) const;

    /**
    @brief Creates new section
    @return Pointer to a new report section section object, ownership is transferred to
            the caller
    */
    KReportDesignerSection* createSection() Q_REQUIRED_RESULT;

    /**
    @brief Deletes the section specified
    @param section KReportSectionData::Section enum value of the section to return
    */
    void removeSection(KReportSectionData::Section);

    /**
    @brief Create a new section and insert it into the report
    @param section KReportSectionData::Section enum value of the section to return
    */
    void insertSection(KReportSectionData::Section);

    /**
    @brief Return a pointer to the detail section.
    The detail section contains the actual detail section and related group sections
    @return Pointer to detail section
    */
    KReportDesignerSectionDetail* detailSection() const;

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
    @return a list of field names in the selected KReportData
    */
    QStringList fieldNames() const;

    /**
    @return a list of field keys in the selected KReportData
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
    KPropertySet* propertySet() const;

    /**
    @brief Give a hint on the size of the widget
    */
    virtual QSize sizeHint() const;

    /**
    @brief Return the current unit assigned to the report
    */
    KReportUnit pageUnit() const;

    /**
    @brief Handle the context menu event for a report section
    @param scene The associated scene (section)
    */
    void sectionContextMenuEvent(KReportDesignerSectionScene *, QGraphicsSceneContextMenuEvent * e);

    /**
    @brief Handle the mouse release event for a report section
    */
    void sectionMouseReleaseEvent(KReportDesignerSectionView *, QMouseEvent * e);

    void sectionMousePressEvent(KReportDesignerSectionView *, QMouseEvent * e);

    /**
    @brief Sets the property set for the currently selected item
    @param set Property set of item
    */
    void changeSet(KPropertySet *);

    /**
    @brief Return the property set for the curently selected item
    */
    KPropertySet* selectedItemPropertySet() const;

    /**
    @brief Sets the modified status, defaulting to true for modified
    @param modified Modified status
    */
    void setModified(bool modified);

    /**
    @brief Return a unique name that can be used by the entity
    @param entity Name of entity
    */
    QString suggestEntityName(const QString &) const;

    /**
    @brief Checks if the supplied name is unique among all entities
    */
    bool isEntityNameUnique(const QString &, KReportItemBase* = 0) const;

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

    /**
     * @brief Adds meta-properties to the property set @a set for consumption by property editor
     * - "this:classString" - user-visible translated name of element type, e.g. tr("Label")
     * - "this:iconName" - name of user-visible icon, e.g. "kreport-label-element"
     *
     * All the properties are set to invisible.
     * @see propertySet()
     */
    static void addMetaProperties(KPropertySet* set, const QString &classString,
                                  const QString &iconName);

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

private:
    void init();

    /**
    @brief Sets the detail section to the given section
    */
    void setDetail(KReportDesignerSectionDetail *rsd);

    /**
    @brief Deletes the detail section
    */
    void deleteDetail();

    virtual void resizeEvent(QResizeEvent * event);

    //Properties
    void createProperties();

    unsigned int selectionCount() const;

    void setSectionCursor(const QCursor&);
    void unsetSectionCursor();

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

private:
    Q_DISABLE_COPY(KReportDesigner)
    class Private;
    Private * const d;
};

#endif
