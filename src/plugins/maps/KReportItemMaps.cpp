/*
 * Copyright (C) 2007-2016 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2011-2015 by Radoslaw Wicik (radoslaw@wicik.pl)
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
#include "KReportItemMaps.h"
#include "KReportUtils.h"
#include "KReportRenderObjects.h"

#include <KPropertyListData>
#include <KPropertySet>

#include <QStringList>

#include <sys/socket.h>

#define myDebug() if (0) kDebug(44021)

//! @todo replace with ReportItemMaps(const QDomNode &element = QDomNode())
KReportItemMaps::KReportItemMaps()
{
    createProperties();
}

KReportItemMaps::KReportItemMaps(const QDomNode &element)
    : KReportItemMaps()
{
    nameProperty()->setValue(KReportUtils::readNameAttribute(element.toElement()));
    m_controlSource->setValue(element.toElement().attribute(QLatin1String("report:item-data-source")));
    setZ(element.toElement().attribute(QLatin1String("report:z-index")).toDouble());
    m_latitudeProperty->setValue(element.toElement().attribute(QLatin1String("report:latitude")).toDouble());
    m_longitudeProperty->setValue(element.toElement().attribute(QLatin1String("report:longitude")).toDouble());
    m_zoomProperty->setValue(element.toElement().attribute(QLatin1String("report:zoom")).toInt());
    QString themeId(element.toElement().attribute(QLatin1String("report:theme")));
    themeId = themeId.isEmpty() ? m_themeManager.mapThemeIds()[0] : themeId;
    m_themeProperty->setValue(themeId);

    parseReportRect(element.toElement());
}

KReportItemMaps::~KReportItemMaps()
{
}

void KReportItemMaps::createProperties()
{
    m_controlSource = new KProperty("item-data-source", new KPropertyListData, QVariant(), tr("Data Source"));

    m_latitudeProperty = new KProperty("latitude", 0.0, tr("Latitude"), QString(), KProperty::Double);
    m_latitudeProperty->setOption("min", -90);
    m_latitudeProperty->setOption("max", 90);
    m_latitudeProperty->setOption("suffix", QString::fromUtf8("°"));
    m_latitudeProperty->setOption("precision", 7);

    m_longitudeProperty = new KProperty("longitude", 0.0, tr("Longitude"), QString(), KProperty::Double);
    m_longitudeProperty->setOption("min", -180);
    m_longitudeProperty->setOption("max", 180);
    m_longitudeProperty->setOption("suffix", QString::fromUtf8("°"));
    m_longitudeProperty->setOption("precision", 7);

    m_zoomProperty     = new KProperty("zoom", 1000, tr("Zoom") );
    m_zoomProperty->setOption("min", 0);
    m_zoomProperty->setOption("max", 4000);
    m_zoomProperty->setOption("step", 100);
    m_zoomProperty->setOption("slider", true);

    QStringList mapThemIds(m_themeManager.mapThemeIds());

    m_themeProperty = new KProperty("theme", new KPropertyListData(mapThemIds, mapThemIds),
                                    QVariant(mapThemIds[1]), tr("Theme"));

    if (mapThemIds.contains(QLatin1String("earth/srtm/srtm.dgml"))) {
        m_themeProperty->setValue(QLatin1String("earth/srtm/srtm.dgml"),
                                  KProperty::ValueOption::IgnoreOld);
    }

    propertySet()->addProperty(m_controlSource);
    propertySet()->addProperty(m_latitudeProperty);
    propertySet()->addProperty(m_longitudeProperty);
    propertySet()->addProperty(m_zoomProperty);
    propertySet()->addProperty(m_themeProperty);
}


void KReportItemMaps::setColumn(const QString &c)
{
    m_controlSource->setValue(c);
}

QString KReportItemMaps::itemDataSource() const
{
    return m_controlSource->value().toString();
}

QString KReportItemMaps::typeName() const
{
    return QLatin1String("maps");
}

int KReportItemMaps::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                       const QVariant &data, KReportScriptHandler *script)
{
    Q_UNUSED(script)

    deserializeData(data);
    m_pageId = page;
    m_sectionId = section;
    m_offset = offset;


    m_oroPicture = new OROPicture();
    m_oroPicture->setPosition(scenePosition(position()) + m_offset);
    m_oroPicture->setSize(sceneSize(size()));

    if (m_pageId) {
        m_pageId->insertPrimitive(m_oroPicture);
    }

    if (m_sectionId) {
        OROPicture *i2 = dynamic_cast<OROPicture*>(m_oroPicture->clone());
        if (i2) {
            i2->setPosition(scenePosition(position()));
        }
    }

    m_mapRenderer.renderJob(this);

    return 0; //Item doesn't stretch the section height
}

void KReportItemMaps::deserializeData(const QVariant& serialized)
{
    //kreportpluginDebug() << "Map data for this record is" << serialized;
    QStringList dataList = serialized.toString().split(QLatin1Char(';'));
    if (dataList.size() == 3) {
        m_latitude = dataList[0].toDouble();
        m_longtitude = dataList[1].toDouble();
        m_zoom = dataList[2].toInt();
    } else {
        m_latitude = m_latitudeProperty->value().toReal();
        m_longtitude = m_longitudeProperty->value().toReal();
        m_zoom = m_zoomProperty->value().toInt();
    }
}

void KReportItemMaps::renderFinished()
{
    emit finishedRendering();
}

OROPicture* KReportItemMaps::oroImage()
{
    return m_oroPicture;
}

qreal KReportItemMaps::longtitude() const
{
    return m_longtitude;
}

qreal KReportItemMaps::latitude() const
{
    return m_latitude;
}

int KReportItemMaps::zoom() const
{
    return m_zoom;
}

QString KReportItemMaps::themeId() const
{
    return m_themeProperty->value().toString();
}

QVariant KReportItemMaps::realItemData(const QVariant& itemData) const
{
    double lat, lon;
    int zoom;

    QStringList dataList = itemData.toString().split(QLatin1Char(';'));

    if (dataList.size() == 3) {
        lat = dataList[0].toDouble();
        lon = dataList[1].toDouble();
        zoom = dataList[2].toInt();
    } else if (dataList.size() == 2) {
        lat = dataList[0].toDouble();
        lon = dataList[1].toDouble();
        zoom = m_zoomProperty->value().toInt();
    } else {
        lat = m_latitudeProperty->value().toReal();
        lon = m_longitudeProperty->value().toReal();
        zoom = m_zoomProperty->value().toInt();
    }

    if (m_longDataSetFromScript) {
        lon = m_longtitude;
    }
    if (m_latDataSetFromScript) {
        lat = m_latitude;
    }
    if (m_zoomDataSetFromScript) {
        zoom = m_zoom;
    }
    return QString(QLatin1String("%1;%2;%3")).arg(lat).arg(lon).arg(zoom);
}
