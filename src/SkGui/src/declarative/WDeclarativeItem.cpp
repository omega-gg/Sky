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

#include "WDeclarativeItem.h"

#ifndef SK_NO_DECLARATIVEITEM

// Sk includes
#include <WMainView>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WDeclarativeItem_p.h"

WDeclarativeItemPrivate::WDeclarativeItemPrivate(WDeclarativeItem * p) : WPrivate(p) {}

void WDeclarativeItemPrivate::init()
{
    view     = NULL;
    viewport = NULL;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WDeclarativeItem::WDeclarativeItem(QDeclarativeItem * parent)
    : QDeclarativeItem(parent), WPrivatable(new WDeclarativeItemPrivate(this))
{
    Q_D(WDeclarativeItem); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WDeclarativeItem::WDeclarativeItem(WDeclarativeItemPrivate * p, QDeclarativeItem * parent)
    : QDeclarativeItem(parent), WPrivatable(p)
{
    Q_D(WDeclarativeItem); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QVariant WDeclarativeItem::itemChange(GraphicsItemChange change,
                                                    const QVariant &   value)
{
    if (change == ItemSceneHasChanged)
    {
        Q_D(WDeclarativeItem);

        QGraphicsScene * scene = qvariant_cast<QGraphicsScene *> (value);

        WMainViewScene * mainScene = static_cast<WMainViewScene *> (scene);

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

        emit viewChanged();
    }

    return QDeclarativeItem::itemChange(change, value);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WMainView * WDeclarativeItem::view() const
{
    Q_D(const WDeclarativeItem); return d->view;
}

QDeclarativeItem * WDeclarativeItem::viewport() const
{
    Q_D(const WDeclarativeItem); return d->viewport;
}

#endif // SK_NO_DECLARATIVEITEM
