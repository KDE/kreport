/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2012 by Friedrich W. H. Kossebau (kossebau@kde.org)
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

#include "KReportSectionEditor.h"

#include "KReportDesigner.h"
#include "KReportSection.h"
#include "KReportDesignerSectionDetail.h"
#include "KReportDetailGroupSectionDialog.h"
#include "KReportDesignerSectionDetailGroup.h"

#include <QIcon>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>

enum {
    KeyRole = Qt::UserRole
};

// KReportDesigner currently prepends an empty key/fieldname pair to the list
// of fields, possibly to offer the option to have report elements not yet
// bound to fields
static inline bool isEditorHelperField(const QString &key)
{
    return key.isEmpty();
}

/*
 *  Constructs a SectionEditor as a child of 'parent'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
KReportSectionEditor::KReportSectionEditor(KReportDesigner* designer)
  : QDialog(designer)
{
    Q_ASSERT(designer);
    m_reportDesigner = designer;
    m_reportSectionDetail = m_reportDesigner->detailSection();

    //! @todo check section editor
    //setButtons(Close);
    //setCaption(tr("Section Editor"));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QPushButton* closeButton = buttonBox->button(QDialogButtonBox::Close);

    closeButton->setDefault(true);
    closeButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));

    QWidget *widget = new QWidget(this);
    m_ui.setupUi(widget);
    m_btnAdd = new QPushButton(QIcon::fromTheme(QLatin1String("list-add")), tr("Add..."), this);
    m_ui.lGroupSectionsButtons->addWidget(m_btnAdd);
    m_btnEdit = new QPushButton(QIcon::fromTheme(QLatin1String("document-edit")), tr("Edit..."), this);
    m_ui.lGroupSectionsButtons->addWidget(m_btnEdit);
    m_btnRemove = new QPushButton(QIcon::fromTheme(QLatin1String("list-remove")), tr("Delete"), this);
    m_ui.lGroupSectionsButtons->addWidget(m_btnRemove);
    m_btnMoveUp = new QPushButton(QIcon::fromTheme(QLatin1String("arrow-up")), tr("Move Up"), this);
    m_ui.lGroupSectionsButtons->addWidget(m_btnMoveUp);
    m_btnMoveDown = new QPushButton(QIcon::fromTheme(QLatin1String("arrow-down")), tr("Move Down"), this);
    m_ui.lGroupSectionsButtons->addWidget(m_btnMoveDown);
    m_ui.lGroupSectionsButtons->addStretch();

    mainLayout->addWidget(widget);
    mainLayout->addWidget(buttonBox);

    //setMainWidget(widget);

    // signals and slots connections
    connect(m_ui.cbReportHeader, SIGNAL(toggled(bool)), this, SLOT(cbReportHeader_toggled(bool)));
    connect(m_ui.cbReportFooter, SIGNAL(toggled(bool)), this, SLOT(cbReportFooter_toggled(bool)));
    connect(m_ui.cbHeadFirst, SIGNAL(toggled(bool)), this, SLOT(cbHeadFirst_toggled(bool)));
    connect(m_ui.cbHeadLast, SIGNAL(toggled(bool)), this, SLOT(cbHeadLast_toggled(bool)));
    connect(m_ui.cbHeadEven, SIGNAL(toggled(bool)), this, SLOT(cbHeadEven_toggled(bool)));
    connect(m_ui.cbHeadOdd, SIGNAL(toggled(bool)), this, SLOT(cbHeadOdd_toggled(bool)));
    connect(m_ui.cbFootFirst, SIGNAL(toggled(bool)), this, SLOT(cbFootFirst_toggled(bool)));
    connect(m_ui.cbFootLast, SIGNAL(toggled(bool)), this, SLOT(cbFootLast_toggled(bool)));
    connect(m_ui.cbFootEven, SIGNAL(toggled(bool)), this, SLOT(cbFootEven_toggled(bool)));
    connect(m_ui.cbFootOdd, SIGNAL(toggled(bool)), this, SLOT(cbFootOdd_toggled(bool)));
    connect(m_ui.cbHeadAny, SIGNAL(toggled(bool)), this, SLOT(cbHeadAny_toggled(bool)));
    connect(m_ui.cbFootAny, SIGNAL(toggled(bool)), this, SLOT(cbFootAny_toggled(bool)));
    connect(m_ui.lbGroups, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(updateButtonsForItem(QListWidgetItem*)));
    connect(m_ui.lbGroups, SIGNAL(currentRowChanged(int)),
            this, SLOT(updateButtonsForRow(int)));

    connect(m_btnAdd, SIGNAL(clicked(bool)), this, SLOT(btnAdd_clicked()));
    connect(m_btnEdit, SIGNAL(clicked(bool)), this, SLOT(btnEdit_clicked()));
    connect(m_btnRemove, SIGNAL(clicked(bool)), this, SLOT(btnRemove_clicked()));
    connect(m_btnMoveUp, SIGNAL(clicked(bool)), this, SLOT(btnMoveUp_clicked()));
    connect(m_btnMoveDown, SIGNAL(clicked(bool)), this, SLOT(brnMoveDown_clicked()));

    // set all the properties

    m_ui.cbReportHeader->setChecked(m_reportDesigner->section(KReportSectionData::ReportHeader));
    m_ui.cbReportFooter->setChecked(m_reportDesigner->section(KReportSectionData::ReportFooter));

    m_ui.cbHeadFirst->setChecked(m_reportDesigner->section(KReportSectionData::PageHeaderFirst));
    m_ui.cbHeadOdd->setChecked(m_reportDesigner->section(KReportSectionData::PageHeaderOdd));
    m_ui.cbHeadEven->setChecked(m_reportDesigner->section(KReportSectionData::PageHeaderEven));
    m_ui.cbHeadLast->setChecked(m_reportDesigner->section(KReportSectionData::PageHeaderLast));
    m_ui.cbHeadAny->setChecked(m_reportDesigner->section(KReportSectionData::PageHeaderAny));

    m_ui.cbFootFirst->setChecked(m_reportDesigner->section(KReportSectionData::PageFooterFirst));
    m_ui.cbFootOdd->setChecked(m_reportDesigner->section(KReportSectionData::PageFooterOdd));
    m_ui.cbFootEven->setChecked(m_reportDesigner->section(KReportSectionData::PageFooterEven));
    m_ui.cbFootLast->setChecked(m_reportDesigner->section(KReportSectionData::PageFooterLast));
    m_ui.cbFootAny->setChecked(m_reportDesigner->section(KReportSectionData::PageFooterAny));

    // now set the rw value
    if (m_reportSectionDetail) {
        const QStringList columnNames = m_reportDesigner->fieldNames();
        const QStringList keys = m_reportDesigner->fieldKeys();
        for (int i = 0; i < m_reportSectionDetail->groupSectionCount(); ++i) {
            const QString key = m_reportSectionDetail->groupSection(i)->column();
            const int idx = keys.indexOf(key);
            const QString columnName = columnNames.value(idx);
            QListWidgetItem *item = new QListWidgetItem(columnName);
            item->setData(KeyRole, key);
            m_ui.lbGroups->addItem(item);
        }
    }
    if (m_ui.lbGroups->count() == 0) {
    } else {
        m_ui.lbGroups->setCurrentItem(m_ui.lbGroups->item(0));
    }
    updateButtonsForItem(m_ui.lbGroups->currentItem());
    updateAddButton();
    updateButtonsForRow(m_ui.lbGroups->currentRow());
}

/*
 *  Destroys the object and frees any allocated resources
 */
KReportSectionEditor::~KReportSectionEditor()
{
    // no need to delete child widgets, Qt does it all for us
}

void KReportSectionEditor::cbReportHeader_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::ReportHeader);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::ReportHeader);
        }
    }

}

void KReportSectionEditor::cbReportFooter_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::ReportFooter);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::ReportFooter);
        }
    }

}

void KReportSectionEditor::cbHeadFirst_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::PageHeaderFirst);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::PageHeaderFirst);
        }
    }

}

void KReportSectionEditor::cbHeadLast_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::PageHeaderLast);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::PageHeaderLast);
        }
    }

}

void KReportSectionEditor::cbHeadEven_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::PageHeaderEven);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::PageHeaderEven);
        }
    }

}

void KReportSectionEditor::cbHeadOdd_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::PageHeaderOdd);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::PageHeaderOdd);
        }
    }

}

void KReportSectionEditor::cbFootFirst_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::PageFooterFirst);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::PageFooterFirst);
        }
    }

}

void KReportSectionEditor::cbFootLast_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::PageFooterLast);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::PageFooterLast);
        }
    }

}

void KReportSectionEditor::cbFootEven_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::PageFooterEven);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::PageFooterEven);
        }
    }

}

void KReportSectionEditor::cbFootOdd_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::PageFooterOdd);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::PageFooterOdd);
        }
    }

}

bool KReportSectionEditor::editDetailGroup(KReportDesignerSectionDetailGroup * rsdg)
{
    KReportDetailGroupSectionDialog * dgsd = new KReportDetailGroupSectionDialog(this);

    // add the current column and all columns not yet used for groups
    const QStringList keys = m_reportDesigner->fieldKeys();
    const QStringList columnNames = m_reportDesigner->fieldNames();
    // in case of to-be-added group that column needs to be added to the used
    const QSet<QString> usedColumns = groupingColumns() << rsdg->column();
    // if the current column is not among the keys, something is broken.
    // for now just simply select no column in the combobox, achieved by -1
    int indexOfCurrentColumn = -1;
    for (int i = 0; i < keys.count(); ++i) {
        const QString &key = keys.at(i);
        // skip any editor helper fields
        if (isEditorHelperField(key)) {
            continue;
        }
        // already used?
        if (usedColumns.contains(key)) {
            // and not the one of the group?
            if (key != rsdg->column()) {
                continue;
            }
            // remember index
            indexOfCurrentColumn = dgsd->cbColumn->count();
        }
        dgsd->cbColumn->insertItem( i, columnNames.value(i), key);
    }
    dgsd->cbColumn->setCurrentIndex(indexOfCurrentColumn);

    dgsd->cbSort->addItem(tr("Ascending"), Qt::AscendingOrder);
    dgsd->cbSort->addItem(tr("Descending"), Qt::DescendingOrder);
    dgsd->cbSort->setCurrentIndex(dgsd->cbSort->findData(rsdg->sort()));

    dgsd->breakAfterFooter->setChecked(rsdg->pageBreak() == KReportDesignerSectionDetailGroup::PageBreak::AfterGroupFooter);
    dgsd->cbHead->setChecked(rsdg->groupHeaderVisible());
    dgsd->cbFoot->setChecked(rsdg->groupFooterVisible());

    const bool isOkayed = (dgsd->exec() == QDialog::Accepted);

    if (isOkayed) {
        const QString newColumn =
            dgsd->cbColumn->itemData(dgsd->cbColumn->currentIndex()).toString();
        const QString oldColumn = rsdg->column();
        if (newColumn != oldColumn) {
            rsdg->setColumn(newColumn);
        }

        rsdg->setGroupHeaderVisible(dgsd->cbHead->isChecked());
        rsdg->setGroupFooterVisible(dgsd->cbFoot->isChecked());

        const KReportDesignerSectionDetailGroup::PageBreak pageBreak
            = dgsd->breakAfterFooter->isChecked()
            ? KReportDesignerSectionDetailGroup::PageBreak::AfterGroupFooter
            : KReportDesignerSectionDetailGroup::PageBreak::None;
        rsdg->setPageBreak(pageBreak);

        const Qt::SortOrder sortOrder =
            static_cast<Qt::SortOrder>(dgsd->cbSort->itemData(dgsd->cbSort->currentIndex()).toInt());
        rsdg->setSort(sortOrder);
    }

    delete dgsd;

    return isOkayed;
}

QString KReportSectionEditor::columnName(const QString &column) const
{
    const QStringList keys = m_reportDesigner->fieldKeys();
    const QStringList columnNames = m_reportDesigner->fieldNames();
    return columnNames.at(keys.indexOf(column));
}

QSet<QString> KReportSectionEditor::groupingColumns() const
{
    QSet<QString> result;
    for (int i = 0; i < m_ui.lbGroups->count(); ++i) {
        result.insert(m_ui.lbGroups->item(i)->data(KeyRole).toString());
    }
    return result;
}

void KReportSectionEditor::cbHeadAny_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::PageHeaderAny);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::PageHeaderAny);
        }
    }
}

void KReportSectionEditor::cbFootAny_toggled(bool yes)
{
    if (m_reportDesigner) {
        if (yes) {
            m_reportDesigner->insertSection(KReportSectionData::PageFooterAny);
        } else {
            m_reportDesigner->removeSection(KReportSectionData::PageFooterAny);
        }
    }
}

void KReportSectionEditor::btnEdit_clicked()
{
    if (m_reportSectionDetail) {
        const int idx = m_ui.lbGroups->currentRow();
        if (idx < 0) {
            return;
        }
        KReportDesignerSectionDetailGroup * rsdg = m_reportSectionDetail->groupSection(idx);
        if (editDetailGroup(rsdg)) {
            // update name in list
            m_ui.lbGroups->item(idx)->setText(columnName(rsdg->column()));
        }
    }
}

void KReportSectionEditor::btnAdd_clicked()
{
    if (m_reportSectionDetail) {
        // lets add a new section
        // search for unused column
        QString column;
        const QStringList keys = m_reportDesigner->fieldKeys();
        const QSet<QString> columns = groupingColumns();
        foreach(const QString &key, keys) {
            // skip any editor helper fields
            if (isEditorHelperField(key)) {
                continue;
            }
            if (! columns.contains(key)) {
                column = key;
                break;
            }
        }
        // should not happen, but we do not really know if m_reportDesigner is in sync
        if (column.isEmpty()) {
            return;
        }

        // create new group, have it edited and add it, if not cancelled
        KReportDesignerSectionDetailGroup * rsdg =
            new KReportDesignerSectionDetailGroup(column, m_reportSectionDetail, m_reportSectionDetail);
        if (editDetailGroup(rsdg)) {
            // append to group sections
            m_reportSectionDetail->insertGroupSection(m_reportSectionDetail->groupSectionCount(), rsdg);
            // add to combobox
            const QString column = rsdg->column();
            QListWidgetItem *item = new QListWidgetItem(columnName(column));
            item->setData(KeyRole, column);
            m_ui.lbGroups->addItem(item);
            m_ui.lbGroups->setCurrentRow(m_ui.lbGroups->count() - 1);
            updateAddButton();
        } else {
            delete rsdg;
        }
    }
}


void KReportSectionEditor::btnRemove_clicked()
{
    if (m_reportSectionDetail) {
        const int index = m_ui.lbGroups->currentRow();
        if (index != -1) {
            QListWidgetItem *item = m_ui.lbGroups->takeItem(index);
            delete item;
            m_reportSectionDetail->removeGroupSection(index, true);
            // a field got usable, so make sure add button is available again
            m_btnAdd->setEnabled(true);
            // workaround for at least Qt 4.8.1, which does not emit the proper
            // currentRowChanged signal on deletion of the first element
            updateButtonsForRow(m_ui.lbGroups->currentRow());
        }
    }
}


void KReportSectionEditor::btnMoveUp_clicked()
{
    if (m_reportSectionDetail) {
        int idx = m_ui.lbGroups->currentRow();
        if (idx <= 0) return;
        QString s = m_ui.lbGroups->currentItem()->text();
        m_ui.lbGroups->takeItem(idx);
        m_ui.lbGroups->insertItem(idx - 1, s);
        m_ui.lbGroups->setCurrentRow(idx - 1, QItemSelectionModel::ClearAndSelect);
        KReportDesignerSectionDetailGroup * rsdg = m_reportSectionDetail->groupSection(idx);
        bool showgh = rsdg->groupHeaderVisible();
        bool showgf = rsdg->groupFooterVisible();
        m_reportSectionDetail->removeGroupSection(idx);
        m_reportSectionDetail->insertGroupSection(idx - 1, rsdg);
        rsdg->setGroupHeaderVisible(showgh);
        rsdg->setGroupFooterVisible(showgf);
    }
}


void KReportSectionEditor::brnMoveDown_clicked()
{
    if (m_reportSectionDetail) {
        int idx = m_ui.lbGroups->currentRow();
        if (idx == (int)(m_ui.lbGroups->count() - 1)) return;
        QString s = m_ui.lbGroups->currentItem()->text();
        m_ui.lbGroups->takeItem(idx);
        m_ui.lbGroups->insertItem (idx + 1, s);
        m_ui.lbGroups->setCurrentRow(idx + 1, QItemSelectionModel::ClearAndSelect);
        KReportDesignerSectionDetailGroup * rsdg = m_reportSectionDetail->groupSection(idx);
        bool showgh = rsdg->groupHeaderVisible();
        bool showgf = rsdg->groupFooterVisible();
        m_reportSectionDetail->removeGroupSection(idx);
        m_reportSectionDetail->insertGroupSection(idx + 1, rsdg);
        rsdg->setGroupHeaderVisible(showgh);
        rsdg->setGroupFooterVisible(showgf);
    }
}

void KReportSectionEditor::updateButtonsForItem(QListWidgetItem* currentItem)
{
    const bool isItemSelected = (currentItem != 0);

    m_btnEdit->setEnabled(isItemSelected);
    m_btnRemove->setEnabled(isItemSelected);

}

void KReportSectionEditor::updateButtonsForRow(int row)
{
    const bool enableMoveUpButton = (row > 0);
    const bool enableMoveDownButton = (0 <= row) && (row+1 < m_ui.lbGroups->count());

    m_btnMoveUp->setEnabled(enableMoveUpButton);
    m_btnMoveDown->setEnabled(enableMoveDownButton);
}

void KReportSectionEditor::updateAddButton()
{
    // search for unused column
    bool foundUnusedColumn = false;
    const QStringList keys = m_reportDesigner->fieldKeys();
    const QSet<QString> columns = groupingColumns();
    foreach(const QString &key, keys) {
        // skip any editor helper fields
        if (isEditorHelperField(key)) {
            continue;
        }
        if (! columns.contains(key)) {
            foundUnusedColumn = true;
            break;
        }
    }
    m_btnAdd->setEnabled(foundUnusedColumn);
}
