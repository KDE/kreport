/*
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
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

#ifndef KREPORTITEMMAPS_H
#define KREPORTITEMMAPS_H

#include "KReportAsyncItemBase.h"

#include <marble/MarbleWidget.h>
#include <marble/MapThemeManager.h>

#include <KProperty>

#include "KReportMapRenderer.h"

#include <QDomNode>

class OROImage;
class OROPicture;
class OROPage;
class OROSection;

namespace Scripting
{
class Maps;
}

class KReportItemMaps : public KReportAsyncItemBase
{
    Q_OBJECT
public:
    KReportItemMaps();
    explicit KReportItemMaps(const QDomNode &element);
    ~KReportItemMaps() override;

    QString typeName() const override;
    int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset, const QVariant &data, KReportScriptHandler *script) override;

    QString itemDataSource() const override;
    QVariant realItemData(const QVariant &itemData) const override;

    void renderFinished();

    qreal longtitudeValue() const;
    qreal latitudeValue() const;
    int zoomValue() const;
    QString themeId() const;

    OROPicture* oroImage();

protected:
    void setColumn(const QString&);

    KProperty* controlSource;
    KProperty* latitudeProperty;
    KProperty* longitudeProperty;
    KProperty* zoomProperty;
    KProperty* themeProperty;

    qreal longtitude = 0.0;
    qreal latitude = 0.0;
    int zoom = 1200;
    OROPage *pageId = nullptr;
    OROSection *sectionId = nullptr;
    QPointF offset;
    OROPicture * oroPicture = nullptr;
    KReportMapRenderer mapRenderer;
    Marble::MapThemeManager themeManager;
    bool longDataSetFromScript = false;
    bool latDataSetFromScript = false;
    bool zoomDataSetFromScript = false;

private:
    void createProperties() override;
    void deserializeData(const QVariant& serialized);

    friend class Scripting::Maps;
};

#endif
