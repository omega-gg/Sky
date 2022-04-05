//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.

    - Private License Usage:
    Sky kit licensees holding valid private licenses may use this file in accordance with the
    private license agreement provided with the Software or, alternatively, in accordance with the
    terms contained in written agreement between you and Sky kit authors. For further information
    contact us at contact@omega.gg.
*/
//=================================================================================================

#include "WDeclarativeItem.h"

#ifndef SK_NO_DECLARATIVEITEM

// Sk includes
#include <WView>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeItemPrivate::WDeclarativeItemPrivate(WDeclarativeItem * p) : WPrivate(p) {}

void WDeclarativeItemPrivate::init()
{
    view = NULL;

#ifdef QT_4
    viewport = NULL;
#endif
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeItemPrivate::onRatioChanged()
{
    Q_Q(WDeclarativeItem); q->updateRatioPixel();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WDeclarativeItem::WDeclarativeItem(QDeclarativeItem * parent)
    : QDeclarativeItem(parent), WPrivatable(new WDeclarativeItemPrivate(this))
#else
/* explicit */ WDeclarativeItem::WDeclarativeItem(QQuickItem * parent)
    : QQuickItem(parent), WPrivatable(new WDeclarativeItemPrivate(this))
#endif
{
    Q_D(WDeclarativeItem); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WDeclarativeItem::WDeclarativeItem(WDeclarativeItemPrivate * p, QDeclarativeItem * parent)
    : QDeclarativeItem(parent), WPrivatable(p)
#else
WDeclarativeItem::WDeclarativeItem(WDeclarativeItemPrivate * p, QQuickItem * parent)
    : QQuickItem(parent), WPrivatable(p)
#endif
{
    Q_D(WDeclarativeItem); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeItem::updateRatioPixel() {}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ QVariant WDeclarativeItem::itemChange(GraphicsItemChange change,
                                                    const QVariant &   value)
#else
/* virtual */ void WDeclarativeItem::itemChange(ItemChange change, const ItemChangeData & value)
#endif
{
#ifdef QT_4
    if (change == ItemSceneHasChanged)
#else
    if (change == ItemSceneChange)
#endif
    {
        Q_D(WDeclarativeItem);

#ifdef QT_4
        QGraphicsScene * scene = qvariant_cast<QGraphicsScene *> (value);

        WViewScene * mainScene = static_cast<WViewScene *> (scene);

        if (mainScene)
        {
            d->view = mainScene->view;

            d->viewport = d->view->item();
        }
        else
        {
            d->view     = NULL;
            d->viewport = NULL;
        }
#else
        if (d->view) disconnect(d->view, 0, this, 0);

        d->view = static_cast<WView *> (value.window);

        connect(d->view, SIGNAL(ratioPixelChanged()), this, SLOT(onRatioChanged()));
#endif

        emit viewChanged();
    }

#ifdef QT_4
    return QDeclarativeItem::itemChange(change, value);
#else
    QQuickItem::itemChange(change, value);
#endif
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WView * WDeclarativeItem::view() const
{
    Q_D(const WDeclarativeItem); return d->view;
}

#ifdef QT_4

QDeclarativeItem * WDeclarativeItem::viewport() const
{
    Q_D(const WDeclarativeItem); return d->viewport;
}

#endif

#endif // SK_NO_DECLARATIVEITEM
