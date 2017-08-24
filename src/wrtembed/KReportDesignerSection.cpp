/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2014 Jarosław Staniek <staniek@kde.org>
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

#include "KReportDesignerSection.h"

#include "KReportDesignerSectionScene.h"
#include "KReportDesignerSectionView.h"
#include "KReportDesigner.h"
#include "KReportDesignerItemBase.h"
#include "KReportUtils.h"
#include "KReportPluginInterface.h"
#include "KReportPluginManager.h"
#include "KReportDesignerItemRectBase.h"
#include "KReportDesignerItemLine.h"
#include "KReportRuler_p.h"
#include "KReportZoomHandler_p.h"
#include "KReportUtils_p.h"
#include "KReportPluginMetaData.h"
#include "kreport_debug.h"

#include <QLabel>
#include <QFrame>
#include <QDomDocument>
#include <QLayout>
#include <QGridLayout>
#include <QMouseEvent>
#include <QApplication>
#include <QApplication>
#include <QIcon>


//! @internal
class ReportResizeBar : public QFrame
{
    Q_OBJECT
public:
    explicit ReportResizeBar(QWidget * parent = nullptr, Qt::WindowFlags f = nullptr);

Q_SIGNALS:
    void barDragged(int delta);

protected:
    void mouseMoveEvent(QMouseEvent * e) override;
};

//! @internal
class KReportDesignerSectionTitle : public QLabel
{
    Q_OBJECT
public:
    explicit KReportDesignerSectionTitle(QWidget *parent = nullptr);
    ~KReportDesignerSectionTitle() override;

Q_SIGNALS:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

//! @internal
class Q_DECL_HIDDEN KReportDesignerSection::Private
{
public:
    explicit Private()
    {}

    ~Private()
    {}

    KReportDesignerSectionTitle *title;
    KReportDesignerSectionScene *scene;
    ReportResizeBar *resizeBar;
    KReportDesignerSectionView *sceneView;
    KReportDesigner*reportDesigner;
    KReportRuler *sectionRuler;

    KReportSectionData *sectionData;
    int dpiY;
};


KReportDesignerSection::KReportDesignerSection(KReportDesigner * rptdes,
                                               const KReportZoomHandler &zoomHandler)
        : QWidget(rptdes)
        , d(new Private())
{
    Q_ASSERT(rptdes);
    d->sectionData = new KReportSectionData(this);
    connect(d->sectionData->propertySet(), SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));

    d->dpiY = KReportPrivate::dpiY();

    d->reportDesigner = rptdes;
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QGridLayout * glayout = new QGridLayout(this);
    glayout->setSpacing(0);
    glayout->setMargin(0);
    glayout->setColumnStretch(1, 1);
    glayout->setRowStretch(1, 1);
    glayout->setSizeConstraint(QLayout::SetFixedSize);

    // ok create the base interface
    d->title = new KReportDesignerSectionTitle(this);
    d->title->setObjectName(QLatin1String("detail"));
    d->title->setText(tr("Detail"));

    d->sectionRuler = new KReportRuler(this, Qt::Vertical, zoomHandler);
    d->sectionRuler->setUnit(d->reportDesigner->pageUnit());
    d->scene = new KReportDesignerSectionScene(d->reportDesigner->pageWidthPx(), d->dpiY, rptdes);
    d->scene->setBackgroundBrush(d->sectionData->backgroundColor());
    
    d->sceneView = new KReportDesignerSectionView(rptdes, d->scene, this);
    d->sceneView->setObjectName(QLatin1String("scene view"));
    d->sceneView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    d->resizeBar = new ReportResizeBar(this);

    connect(d->resizeBar, SIGNAL(barDragged(int)), this, SLOT(slotResizeBarDragged(int)));
    connect(d->reportDesigner, SIGNAL(pagePropertyChanged(KPropertySet&)),
        this, SLOT(slotPageOptionsChanged(KPropertySet&)));
    connect(d->scene, SIGNAL(clicked()), this, (SLOT(slotSceneClicked())));
    connect(d->scene, SIGNAL(lostFocus()), d->title, SLOT(update()));
    connect(d->title, SIGNAL(clicked()), this, (SLOT(slotSceneClicked())));

    glayout->addWidget(d->title, 0, 0, 1, 2);
    glayout->addWidget(d->sectionRuler, 1, 0);
    glayout->addWidget(d->sceneView , 1, 1);
    glayout->addWidget(d->resizeBar, 2, 0, 1, 2);
    d->sectionRuler->setFixedWidth(d->sectionRuler->sizeHint().width());

    setLayout(glayout);
    slotResizeBarDragged(0);
}

KReportDesignerSection::~KReportDesignerSection()
{
    delete d;
}

void KReportDesignerSection::setTitle(const QString & s)
{
    d->title->setText(s);
}

void KReportDesignerSection::slotResizeBarDragged(int delta, bool changeSet)
{
    if (d->sceneView->designer() && d->sceneView->designer()->propertySet()->property("page-size").value().toString() == QLatin1String("Labels")) {
        return; // we don't want to allow this on reports that are for labels
    }
    
    if (changeSet) {
      slotSceneClicked(); // switches property set to this section
    }
    
    qreal h = d->scene->height() + delta;

    if (h < 1) h = 1;

    h = d->scene->gridPoint(QPointF(0, h)).y();
    d->sectionData->m_height->setValue(INCH_TO_POINT(h/d->dpiY));
    d->sectionRuler->setRulerLength(h);

    d->scene->setSceneRect(0, 0, d->scene->width(), h);
    d->sceneView->resizeContents(QSize(d->scene->width(), h));

    d->reportDesigner->setModified(true);
}

void KReportDesignerSection::buildXML(QDomDocument *doc, QDomElement *section)
{
    KReportUtils::setAttribute(section, QLatin1String("svg:height"), d->sectionData->m_height->value().toDouble());
    section->setAttribute(QLatin1String("fo:background-color"), d->sectionData->backgroundColor().name());

    // now get a list of all the QGraphicsItems on this scene and output them.
    QGraphicsItemList list = d->scene->items();
    for (QGraphicsItemList::iterator it = list.begin();
            it != list.end(); ++it) {
        KReportDesignerItemBase::buildXML((*it), doc, section);
    }
}

void KReportDesignerSection::initFromXML(const QDomNode & section)
{
    QDomNodeList nl = section.childNodes();
    QDomNode node;
    QString n;

    qreal h = KReportUnit::parseValue(section.toElement().attribute(QLatin1String("svg:height"), QLatin1String("2.0cm")));
    d->sectionData->m_height->setValue(h);

    h  = POINT_TO_INCH(h) * d->dpiY;
    //kreportDebug() << "Section Height: " << h;
    d->scene->setSceneRect(0, 0, d->scene->width(), h);
    slotResizeBarDragged(0);

    d->sectionData->m_backgroundColor->setValue(QColor(section.toElement().attribute(QLatin1String("fo:background-color"), QLatin1String("#ffffff"))));

    for (int i = 0; i < nl.count(); ++i) {
        node = nl.item(i);
        n = node.nodeName();
        if (n.startsWith(QLatin1String("report:"))) {
            //Load objects
            //report:line is a special case as it is not a plugin
            QString reportItemName = n.mid(qstrlen("report:"));
            if (reportItemName == QLatin1String("line")) {
                (new KReportDesignerItemLine(node, d->sceneView->designer(), d->scene))->setVisible(true);
                continue;
            }
            KReportPluginManager* manager = KReportPluginManager::self();
            KReportPluginInterface *plugin = manager->plugin(reportItemName);
            if (plugin) {
                QObject *obj = plugin->createDesignerInstance(node, d->reportDesigner, d->scene);
                if (obj) {
                    KReportDesignerItemRectBase *entity = dynamic_cast<KReportDesignerItemRectBase*>(obj);
                    if (entity) {
                        entity->setVisible(true);
                    }
                    KReportItemBase *item = dynamic_cast<KReportItemBase*>(obj);
                    if (item) {
                        item->setUnit(d->reportDesigner->pageUnit());
                        KReportDesigner::addMetaProperties(item->propertySet(),
                                                           plugin->metaData()->name(),
                                                           plugin->metaData()->iconName());
                    }
                    continue;
                }
            }
        }
        kreportWarning() << "Encountered unknown node while parsing section: " << n;
    }
}

QSize KReportDesignerSection::sizeHint() const
{
    return QSize(d->scene->width()  + d->sectionRuler->frameSize().width(), d->title->frameSize().height() + d->sceneView->sizeHint().height() + d->resizeBar->frameSize().height());
}

void KReportDesignerSection::slotPageOptionsChanged(KPropertySet &set)
{
    Q_UNUSED(set)

    KReportUnit unit = d->reportDesigner->pageUnit();

    d->sectionData->m_height->setOption("unit", unit.symbol());

    //update items position with unit
    QList<QGraphicsItem*> itms = d->scene->items();
    for (int i = 0; i < itms.size(); ++i) {
        KReportItemBase *obj = dynamic_cast<KReportItemBase*>(itms[i]);
        if (obj) {
            obj->setUnit(unit);
        }
    }

    d->scene->setSceneRect(0, 0, d->reportDesigner->pageWidthPx(), d->scene->height());
    d->title->setMinimumWidth(d->reportDesigner->pageWidthPx() + d->sectionRuler->frameSize().width());
    d->sectionRuler->setUnit(d->reportDesigner->pageUnit());

    //Trigger a redraw of the background
    d->sceneView->resetCachedContent();

    d->reportDesigner->adjustSize();
    d->reportDesigner->repaint();

    slotResizeBarDragged(0, false);
}

void KReportDesignerSection::slotSceneClicked()
{
    d->reportDesigner->setActiveScene(d->scene);
    d->reportDesigner->changeSet(d->sectionData->propertySet());
}

void KReportDesignerSection::slotPropertyChanged(KPropertySet &s, KProperty &p)
{
    Q_UNUSED(s)
    //kreportDebug() << p.name();

    //Handle Background Color
    if (p.name() == "background-color") {
        d->scene->setBackgroundBrush(p.value().value<QColor>());
    }

    if (p.name() == "height") {
    d->scene->setSceneRect(0, 0, d->scene->width(), POINT_TO_INCH(p.value().toDouble()) * d->dpiY);
    slotResizeBarDragged(0);
    }

    if (d->reportDesigner)
        d->reportDesigner->setModified(true);

    d->sceneView->resetCachedContent();
    d->scene->update();
}

void KReportDesignerSection::setSectionCursor(const QCursor& c)
{
    if (d->sceneView)
        d->sceneView->setCursor(c);
}

void KReportDesignerSection::unsetSectionCursor()
{
    if (d->sceneView)
        d->sceneView->unsetCursor();
}

QGraphicsItemList KReportDesignerSection::items() const
{
    QGraphicsItemList items;

    if (d->scene) {
        foreach (QGraphicsItem *itm, d->scene->items()) {
            if (itm->parentItem() == nullptr) {
                items << itm;
            }
        }
    }

    return items;
}


//
// class ReportResizeBar
//
ReportResizeBar::ReportResizeBar(QWidget * parent, Qt::WindowFlags f)
        : QFrame(parent, f)
{
    setCursor(QCursor(Qt::SizeVerCursor));
    setFrameStyle(QFrame::HLine);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

void ReportResizeBar::mouseMoveEvent(QMouseEvent * e)
{
    e->accept();
    emit barDragged(e->y());
}

//=============================================================================

KReportDesignerSectionTitle::KReportDesignerSectionTitle(QWidget*parent) : QLabel(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setMinimumHeight(qMax(fontMetrics().lineSpacing(),16 + 2)); //16 = Small icon size
}

KReportDesignerSectionTitle::~KReportDesignerSectionTitle()
{
}

//! \return true if \a o has parent \a par.
static bool hasParent(QObject* par, QObject* o)
{
    if (!o || !par)
        return false;
    while (o && o != par)
        o = o->parent();
    return o == par;
}

static void replaceColors(QPixmap* original, const QColor& color)
{
    QImage dest(original->toImage());
    QPainter p(&dest);
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.fillRect(dest.rect(), color);
    *original = QPixmap::fromImage(dest);
}

void KReportDesignerSectionTitle::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    KReportDesignerSection* section = dynamic_cast<KReportDesignerSection*>(parent());
    if (section) {
        const bool current = section->d->scene == section->d->reportDesigner->activeScene();
        QPalette::ColorGroup cg = QPalette::Inactive;
        QWidget *activeWindow = QApplication::activeWindow();
        if (activeWindow) {
            QWidget *par = activeWindow->focusWidget();
            if (qobject_cast<KReportDesignerSectionView*>(par)) {
                par = par->parentWidget(); // we're close, pick common parent
            }
            if (hasParent(par, this)) {
                cg = QPalette::Active;
            }
        }
        if (current) {
            painter.fillRect(rect(), palette().brush(cg, QPalette::Highlight));
        }
        painter.setPen(palette().color(cg, current ? QPalette::HighlightedText : QPalette::WindowText));
        QPixmap pixmap(QIcon::fromTheme(QLatin1String("arrow-down")).pixmap(16,16));
        replaceColors(&pixmap, painter.pen().color());
        const int left = 25;
        painter.drawPixmap(QPoint(left, (height() - pixmap.height()) / 2), pixmap);

        painter.drawText(rect().adjusted(left + pixmap.width() + 4, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, text());
    }
    QFrame::paintEvent(event);
}

void KReportDesignerSectionTitle::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

#include "KReportDesignerSection.moc"
