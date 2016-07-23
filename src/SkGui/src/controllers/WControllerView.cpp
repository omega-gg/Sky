//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

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
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void WControllerViewPrivate::registerMainView(WMainView * mainView)
{
    mainViews.append(mainView);
}

void WControllerViewPrivate::unregisterMainView(WMainView * mainView)
{
    mainViews.removeOne(mainView);
}

//-------------------------------------------------------------------------------------------------

void WControllerViewPrivate::paintRecursive(QPainter        * painter,
                                            QGraphicsObject * item, bool forceVisible) const
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

    paintItem(painter, item);

    foreach (QGraphicsObject * child, childs)
    {
        paintChild(painter, child, forceVisible);
    }
}

void WControllerViewPrivate::paintChild(QPainter        * painter,
                                        QGraphicsObject * item, bool forceVisible) const
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

void WControllerViewPrivate::paintItem(QPainter * painter, QGraphicsObject * item) const
{
    QStyleOptionGraphicsItem style;

    style.rect = item->boundingRect().toRect();

    style.exposedRect = style.rect;

    item->paint(painter, &style);
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
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WControllerView::screenNumber(const QWidget * widget) const
{
    return qApp->desktop()->screenNumber(widget);
}

/* Q_INVOKABLE */ int WControllerView::screenNumber(const QPoint & pos) const
{
    return qApp->desktop()->screenNumber(pos);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ const QRect WControllerView::availableGeometry(int screen) const
{
    return qApp->desktop()->availableGeometry(screen);
}

/* Q_INVOKABLE */ const QRect WControllerView::availableGeometry(const QWidget * widget) const
{
    return qApp->desktop()->availableGeometry(widget);
}

/* Q_INVOKABLE */ const QRect WControllerView::availableGeometry(const QPoint & pos) const
{
    return qApp->desktop()->availableGeometry(pos);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ const QRect WControllerView::screenGeometry(int screen) const
{
    return qApp->desktop()->screenGeometry(screen);
}

/* Q_INVOKABLE */ const QRect WControllerView::screenGeometry(const QWidget * widget) const
{
    return qApp->desktop()->screenGeometry(widget);
}

/* Q_INVOKABLE */ const QRect WControllerView::screenGeometry(const QPoint & pos) const
{
    return qApp->desktop()->screenGeometry(pos);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QPixmap WControllerView::takeItemShot(QGraphicsObject * item,
                                                        bool              recursive,
                                                        const QColor    & background,
                                                        bool              forceVisible) const
{
    Q_ASSERT(item);

    Q_D(const WControllerView);

    QPixmap pixmap(item->boundingRect().size().toSize());

    pixmap.fill(background);

    QPainter painter(&pixmap);

    if (recursive)
    {
        d->paintRecursive(&painter, item, forceVisible);
    }
    else if (item->isVisible() || forceVisible)
    {
        d->paintItem(&painter, item);
    }

    painter.end();

    return pixmap;
}

/* Q_INVOKABLE */ bool WControllerView::saveItemShot(const QString   & fileName,
                                                     QGraphicsObject * item,
                                                     bool              recursive,
                                                     const QColor    & background,
                                                     bool              forceVisible) const
{
    QImage image = takeItemShot(item, recursive, background, forceVisible).toImage();

    return image.save(fileName, "png");
}

//-------------------------------------------------------------------------------------------------
// Static interface
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

#endif // SK_NO_CONTROLLERVIEW
