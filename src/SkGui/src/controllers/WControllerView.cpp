//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WControllerView.h"

#ifndef SK_NO_CONTROLLERVIEW

// Qt includes
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsObject>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDir>

W_INIT_CONTROLLER(WControllerView)

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WControllerView_p.h"

WControllerViewPrivate::WControllerViewPrivate(WControllerView * p) : WControllerPrivate(p) {}

/* virtual */ WControllerViewPrivate::~WControllerViewPrivate()
{
    W_CLEAR_CONTROLLER(WControllerView);
}

//-------------------------------------------------------------------------------------------------

void WControllerViewPrivate::init()
{
    loadMode = WControllerView::LoadAlways;

    scaleDelay = 220;
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WControllerViewPrivate::registerView(WView * view)
{
    views.append(view);
}

void WControllerViewPrivate::unregisterView(WView * view)
{
    views.removeOne(view);
}

//-------------------------------------------------------------------------------------------------
// Private ctor / dtor
//-------------------------------------------------------------------------------------------------

WControllerView::WControllerView() : WController(new WControllerViewPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerView::init()
{
    Q_D(WControllerView); d->init();
}

//-------------------------------------------------------------------------------------------------
// Static interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ int WControllerView::screenNumber(const QWidget * widget)
{
    return qApp->desktop()->screenNumber(widget);
}

/* Q_INVOKABLE static */ int WControllerView::screenNumber(const QPoint & pos)
{
    return qApp->desktop()->screenNumber(pos);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ const QRect WControllerView::availableGeometry(int screen)
{
    return qApp->desktop()->availableGeometry(screen);
}

/* Q_INVOKABLE static */ const QRect WControllerView::availableGeometry(const QWidget * widget)
{
    return qApp->desktop()->availableGeometry(widget);
}

/* Q_INVOKABLE static */ const QRect WControllerView::availableGeometry(const QPoint & pos)
{
    return qApp->desktop()->availableGeometry(pos);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ const QRect WControllerView::screenGeometry(int screen)
{
    return qApp->desktop()->screenGeometry(screen);
}

/* Q_INVOKABLE static */ const QRect WControllerView::screenGeometry(const QWidget * widget)
{
    return qApp->desktop()->screenGeometry(widget);
}

/* Q_INVOKABLE static */ const QRect WControllerView::screenGeometry(const QPoint & pos)
{
    return qApp->desktop()->screenGeometry(pos);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QPixmap WControllerView::takeItemShot(QGraphicsObject * item,
                                                               const QColor    & background,
                                                               bool              forceVisible)
{
    Q_ASSERT(item);

    QSize size = item->boundingRect().size().toSize();

    if (size.isValid() == false)
    {
        return QPixmap();
    }

    QPixmap pixmap(size);

    pixmap.fill(background);

    QPainter painter(&pixmap);

    paintRecursive(&painter, item, forceVisible);

    painter.end();

    return pixmap;
}

/* Q_INVOKABLE static */ bool WControllerView::saveItemShot(const QString   & fileName,
                                                            QGraphicsObject * item,
                                                            const QColor    & background,
                                                            bool              forceVisible)
{
    QImage image = takeItemShot(item, background, forceVisible).toImage();

    return image.save(fileName, "png");
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QImage WControllerView::desaturate(const QImage & image)
{
    QImage result(image.width(), image.height(), image.format());

    for (int y = 0; y < image.height(); y++)
    {
        const QRgb * lineA = (QRgb *) image.scanLine(y);

        QRgb * lineB = (QRgb *) result.scanLine(y);

        for (int x = 0; x < image.width(); x++)
        {
            const QRgb & color = *lineA;

            int average = (qRed(color) + qGreen(color) + qBlue(color)) / 3;

            *lineB = qRgba(average, average, average, qAlpha(color));

            lineA++;
            lineB++;
        }
    }

    return result;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerView::compressShots(const QString & path, int quality)
{
    QDir dir(path);

    if (dir.exists() == false) return false;

    QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);

    foreach (QFileInfo info, list)
    {
        if (info.suffix().toLower() == "png")
        {
            const QString & path = info.filePath();

            qDebug("Compressing: %s", path.C_STR);

            QImage image(path);

            if (image.save(path, "png", quality) == false)
            {
                qWarning("WControllerView::compressShots: Failed to save image.");
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ void WControllerView::paintRecursive(QPainter        * painter,
                                                  QGraphicsObject * item, bool forceVisible)
{
    if (item->isVisible() == false && forceVisible == false) return;

    QList<QGraphicsObject *> childs;

    foreach (QGraphicsItem * item, item->childItems())
    {
        QGraphicsObject * child = item->toGraphicsObject();

        if (child)
        {
            if (child->zValue() < 0)
            {
                paintChild(painter, child, forceVisible);
            }
            else childs.append(child);
        }
    }

    QStyleOptionGraphicsItem style;

    style.rect = item->boundingRect().toRect();

    style.exposedRect = style.rect;

    item->paint(painter, &style);

    foreach (QGraphicsObject * child, childs)
    {
        paintChild(painter, child, forceVisible);
    }
}

/* static */ void WControllerView::paintChild(QPainter        * painter,
                                              QGraphicsObject * item, bool forceVisible)
{
    painter->save();

    painter->translate(item->x(), item->y());

    qreal rotation = item->rotation();

    if (rotation)
    {
        QPointF origin = item->transformOriginPoint();

        painter->translate(origin);

        painter->rotate(rotation);

        painter->translate(-origin);
    }

    if (item->flags() & QGraphicsItem::ItemClipsChildrenToShape)
    {
        painter->setClipRect(item->boundingRect(), Qt::IntersectClip);
    }

    painter->setOpacity(item->effectiveOpacity());

    paintRecursive(painter, item, forceVisible);

    painter->restore();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WControllerView::LoadMode WControllerView::loadMode() const
{
    Q_D(const WControllerView); return d->loadMode;
}

void WControllerView::setLoadMode(LoadMode mode)
{
    Q_D(WControllerView);

    if (d->loadMode == mode) return;

    d->loadMode = mode;

    emit loadModeChanged();
}

//-------------------------------------------------------------------------------------------------

int WControllerView::scaleDelay() const
{
    Q_D(const WControllerView); return d->scaleDelay;
}

void WControllerView::setScaleDelay(int delay)
{
    Q_D(WControllerView);

    if (d->scaleDelay == delay) return;

    d->scaleDelay = delay;

    emit scaleDelayChanged();
}

#endif // SK_NO_CONTROLLERVIEW
