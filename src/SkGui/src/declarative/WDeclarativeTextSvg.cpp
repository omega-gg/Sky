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

#include "WDeclarativeTextSvg.h"

#ifndef SK_NO_DECLARATIVETEXTSVG

// Qt includes
#include <QSvgRenderer>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeTextSvgPrivate::WDeclarativeTextSvgPrivate(WDeclarativeTextSvg * p)
    : WDeclarativeItemPrivate(p) {}

void WDeclarativeTextSvgPrivate::init()
{
    Q_Q(WDeclarativeTextSvg);

    renderer = new QSvgRenderer(q);

    QObject::connect(renderer, SIGNAL(repaintNeeded()), q, SLOT(onRepaintNeeded()));

    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeTextSvgPrivate::onRepaintNeeded()
{
    Q_Q(WDeclarativeTextSvg); q->update();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WDeclarativeTextSvg::WDeclarativeTextSvg(QDeclarativeItem * parent)
    : WDeclarativeItem(new WDeclarativeTextSvgPrivate(this), parent)
{
    Q_D(WDeclarativeTextSvg); d->init();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeTextSvg::paint(QPainter *,
                                              const QStyleOptionGraphicsItem *, QWidget *) {}

#endif // SK_NO_DECLARATIVETEXTSVG
