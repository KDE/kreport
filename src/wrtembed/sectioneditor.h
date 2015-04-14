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

#ifndef SECTIONEDITOR_H
#define SECTIONEDITOR_H

#include <QDialog>
#include <QSet>

#include <ui_sectioneditor.h>

class KoReportDesigner;
class ReportSectionDetail;
class ReportSectionDetailGroup;

class SectionEditor : public QDialog
{
    Q_OBJECT

public:
    explicit SectionEditor(QWidget* parent = 0);
    ~SectionEditor();

public:
    void init(KoReportDesigner *rd);

private Q_SLOTS:
    void cbReportHeader_toggled(bool yes);
    void cbReportFooter_toggled(bool yes);
    void cbHeadFirst_toggled(bool yes);
    void cbHeadLast_toggled(bool yes);
    void cbHeadEven_toggled(bool yes);
    void cbHeadOdd_toggled(bool yes);
    void cbFootFirst_toggled(bool yes);
    void cbFootLast_toggled(bool yes);
    void cbFootEven_toggled(bool yes);
    void cbFootOdd_toggled(bool yes);
    void cbHeadAny_toggled(bool yes);
    void cbFootAny_toggled(bool yes);

    void btnAdd_clicked();
    void btnEdit_clicked();
    void btnRemove_clicked();
    void btnMoveUp_clicked();
    void brnMoveDown_clicked();

    void updateButtonsForItem(QListWidgetItem *currentItem);
    void updateButtonsForRow(int row);

private:
    bool editDetailGroup(ReportSectionDetailGroup *rsdg);
    void updateAddButton();

    QString columnName(const QString &column) const;
    QSet<QString> groupingColumns() const;

private:
    Ui::SectionEditor m_ui;
    QPushButton *m_btnAdd;
    QPushButton *m_btnEdit;
    QPushButton *m_btnRemove;
    QPushButton *m_btnMoveUp;
    QPushButton *m_btnMoveDown;

    KoReportDesigner *m_reportDesigner;
    ReportSectionDetail *m_reportSectionDetail;
};

#endif // SECTIONEDITOR_H
