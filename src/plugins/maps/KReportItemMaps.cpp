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

#include <KPropertyListData>
#include <KPropertySet>

#include <QStringList>

#include <KReportRenderObjects.h>

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
    nameProperty()->setValue(element.toElement().attribute(QLatin1String("report:name")));
    controlSource->setValue(element.toElement().attribute(QLatin1String("report:item-data-source")));
    setZ(element.toElement().attribute(QLatin1String("report:z-index")).toDouble());
    latitudeProperty->setValue(element.toElement().attribute(QLatin1String("report:latitude")).toDouble());
    longitudeProperty->setValue(element.toElement().attribute(QLatin1String("report:longitude")).toDouble());
    zoomProperty->setValue(element.toElement().attribute(QLatin1String("report:zoom")).toInt());
    QString themeId(element.toElement().attribute(QLatin1String("report:theme")));
    themeId = themeId.isEmpty() ? themeManager.mapThemeIds()[0] : themeId;
    themeProperty->setValue(themeId);

    parseReportRect(element.toElement());
}

KReportItemMaps::~KReportItemMaps()
{
}

void KReportItemMaps::createProperties()
{
    controlSource = new KProperty("item-data-source", new KPropertyListData, QVariant(), tr("Data Source"));

    latitudeProperty = new KProperty("latitude", 0.0, tr("Latitude"), QString(), KProperty::Double);
    latitudeProperty->setOption("min", -90);
    latitudeProperty->setOption("max", 90);
    latitudeProperty->setOption("unit", QString::fromUtf8("°"));
    latitudeProperty->setOption("precision", 7);

    longitudeProperty = new KProperty("longitude", 0.0, tr("Longitude"), QString(), KProperty::Double);
    longitudeProperty->setOption("min", -180);
    longitudeProperty->setOption("max", 180);
    longitudeProperty->setOption("unit", QString::fromUtf8("°"));
    longitudeProperty->setOption("precision", 7);

    zoomProperty     = new KProperty("zoom", 1000, tr("Zoom") );
    zoomProperty->setOption("min", 0);
    zoomProperty->setOption("max", 4000);
    zoomProperty->setOption("step", 100);
    zoomProperty->setOption("slider", true);

    QStringList mapThemIds(themeManager.mapThemeIds());

    themeProperty = new KProperty("theme", new KPropertyListData(mapThemIds, mapThemIds),
                                    QVariant(mapThemIds[1]), tr("Theme"));

    if (mapThemIds.contains(QLatin1String("earth/srtm/srtm.dgml"))) {
        themeProperty->setValue(QLatin1String("earth/srtm/srtm.dgml"), KProperty::DefaultValueOptions & ~KProperty::ValueOptions(KProperty::ValueOption::RememberOld));
    }

    propertySet()->addProperty(controlSource);
    propertySet()->addProperty(latitudeProperty);
    propertySet()->addProperty(longitudeProperty);
    propertySet()->addProperty(zoomProperty);
    propertySet()->addProperty(themeProperty);
}


void KReportItemMaps::setColumn(const QString &c)
{
    controlSource->setValue(c);
}

QString KReportItemMaps::itemDataSource() const
{
    return controlSource->value().toString();
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
    pageId = page;
    sectionId = section;
    this->offset = offset;

    oroPicture = new OROPicture();
    oroPicture->setPosition(scenePosition(position()) + offset);
    oroPicture->setSize(sceneSize(size()));

    if (pageId) {
        pageId->insertPrimitive(oroPicture);
    }

    if (sectionId) {
        OROPicture *i2 = dynamic_cast<OROPicture*>(oroPicture->clone());
        if (i2) {
            i2->setPosition(scenePosition(position()));
        }
    }

    mapRenderer.renderJob(this);

    return 0; //Item doesn't stretch the section height
}

void KReportItemMaps::deserializeData(const QVariant& serialized)
{
    //kreportpluginDebug() << "Map data for this record is" << serialized;
    QStringList dataList = serialized.toString().split(QLatin1Char(';'));
    if (dataList.size() == 3) {
        latitude = dataList[0].toDouble();
        longtitude = dataList[1].toDouble();
        zoom = dataList[2].toInt();
    } else {
        latitude = latitudeProperty->value().toReal();
        longtitude = longitudeProperty->value().toReal();
        zoom = zoomProperty->value().toInt();
    }
}

void KReportItemMaps::renderFinished()
{
    emit finishedRendering();
}

OROPicture* KReportItemMaps::oroImage()
{
    return oroPicture;
}

qreal KReportItemMaps::longtitudeValue() const
{
    return longtitude;
}

qreal KReportItemMaps::latitudeValue() const
{
    return latitude;
}

int KReportItemMaps::zoomValue() const
{
    return zoom;
}

QString KReportItemMaps::themeId() const
{
    return themeProperty->value().toString();
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
        zoom = zoomProperty->value().toInt();
    } else {
        lat = latitudeProperty->value().toReal();
        lon = longitudeProperty->value().toReal();
        zoom = zoomProperty->value().toInt();
    }

    if (longDataSetFromScript) {
        lon = longtitude;
    }
    if (latDataSetFromScript) {
        lat = latitude;
    }
    if (zoomDataSetFromScript) {
        zoom = zoom;
    }
    return QString(QLatin1String("%1;%2;%3")).arg(lat).arg(lon).arg(zoom);
}
