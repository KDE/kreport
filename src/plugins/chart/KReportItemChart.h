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

#ifndef KRCHARTDATA_H
#define KRCHARTDATA_H

#include <KDChartWidget>

#include "KReportItemBase.h"
#include "KReportSize.h"
#include "KReportPosition.h"
#include "KReportData.h"

namespace Scripting
{
class Chart;
}

/**
*/
class KReportItemChart : public KReportItemBase
{
    Q_OBJECT
public:
    KReportItemChart();
    explicit KReportItemChart(QDomNode *element);
    ~KReportItemChart();

    virtual QString typeName() const;

    virtual int renderReportData(OROPage *page, OROSection *section, const QPointF &offset, KReportData *data, KReportScriptHandler *script);

    KDChart::Widget *widget() {
        return m_chartWidget;
    }

    /**
    @brief Perform the query for the chart and set the charts data
    */
    void populateData();
    void setConnection(const KReportData *c);

    /**
    @brief Set the value of a field from the master (report) data set
    This data will be used when retrieving the data for the chart
    as the values in a 'where' clause.
    */
    void setLinkData(QString, QVariant);

    /**
    @brief Return the list of master fields
    The caller will use this to set the current value for each field
    at that stage in the report
    */
    QStringList masterFields() const;

    /**
    @brief The chart object is a complex object that uses its own data source
    @return true
    */
    virtual bool supportsSubQuery() const { return true; }

protected:

    KProperty * m_dataSource;
    KProperty * m_font;
    KProperty * m_chartType;
    KProperty * m_chartSubType;
    KProperty * m_threeD;
    KProperty * m_colorScheme;
    KProperty * m_aa;
    KProperty * m_xTitle;
    KProperty * m_yTitle;

    KProperty *m_backgroundColor;
    KProperty *m_displayLegend;
    KProperty *m_legendPosition;
    KProperty *m_legendOrientation;

    KProperty *m_linkMaster;
    KProperty *m_linkChild;

    KDChart::Widget *m_chartWidget;

    void set3D(bool td);
    void setAA(bool aa);
    void setColorScheme(const QString &cs);
    void setAxis(const QString &xa, const QString &ya);
    void setBackgroundColor(const QColor&);
    void setLegend(bool le, const QStringList &legends = QStringList());

private:
    virtual void createProperties();
    const KReportData *m_reportData;

    friend class Scripting::Chart;

    QMap<QString, QVariant> m_links; //Map of field->value for child/master links

};

#endif
