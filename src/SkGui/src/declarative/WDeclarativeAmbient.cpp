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

#include "WDeclarativeAmbient.h"

#ifndef SK_NO_DECLARATIVEAMBIENT

// Sk includes
#include <WControllerPlaylist>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeAmbientPrivate::WDeclarativeAmbientPrivate(WDeclarativeAmbient * p)
    : WDeclarativePlayerPrivate(p) {}

void WDeclarativeAmbientPrivate::init()
{
    player = NULL;

    repeat = WDeclarativePlayer::RepeatOne;

    output = WAbstractBackend::OutputAudio;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeAmbientPrivate::onSourceChanged()
{
    QString source = WControllerPlaylist::cleanSource(player->source());

    if (currentSource == source) return;

    currentSource = source;

    hash.clear();

    hash.insert(source, -1);

    q_func()->clear();
}

void WDeclarativeAmbientPrivate::onUpdate()
{
    Q_Q(WDeclarativeAmbient);

    if (backend == NULL || player->hasOutput())
    {
        if (q->source().isEmpty()) return;

        hash.insert(currentSource, q->currentTime());

        q->clear();

        return;
    }

    QString ambient = player->ambient();

    if (ambient.isEmpty())
    {
        // NOTE: We clear the ambient source as late as possible to avoid blanks.
        if (q->source().isEmpty()/* || player->isDefault() == false*/) return;

        hash.insert(currentSource, q->currentTime());

        q->clear();

        return;
    }

    if (q->source() == ambient) return;

    q->setSource(ambient);

    int msec = hash.value(currentSource);

    if (msec != -1) q->seek(msec);

    if (player->state() != WAbstractBackend::StatePlaying) return;

    q->play();
}

void WDeclarativeAmbientPrivate::onState()
{
    Q_Q(WDeclarativeAmbient);

    if (q->source().isEmpty()) return;

    WAbstractBackend::State state = player->state();

    if      (state == WAbstractBackend::StateStopped) q->stop ();
    else if (state == WAbstractBackend::StatePlaying) q->play ();
    else                                              q->pause();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WDeclarativeAmbient::WDeclarativeAmbient(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeAmbient::WDeclarativeAmbient(QQuickItem * parent)
#endif
    : WDeclarativePlayer(new WDeclarativeAmbientPrivate(this), parent)
{
    Q_D(WDeclarativeAmbient); d->init();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WDeclarativePlayer * WDeclarativeAmbient::player() const
{
    Q_D(const WDeclarativeAmbient); return d->player;
}

void WDeclarativeAmbient::setPlayer(WDeclarativePlayer * player)
{
    Q_D(WDeclarativeAmbient);

    if (d->player == player) return;

    if (player)
    {
        disconnect(player, 0, this, 0);

        clear();

        d->currentSource = QString();

        d->hash.clear();
    }

    d->player = player;

    if (player)
    {
        connect(player, SIGNAL(sourceChanged()), this, SLOT(onSourceChanged()));

        connect(player, SIGNAL(backendChanged      ()), this, SLOT(onUpdate()));
        connect(player, SIGNAL(stateLoadChanged    ()), this, SLOT(onUpdate()));
        connect(player, SIGNAL(currentOutputChanged()), this, SLOT(onUpdate()));
        connect(player, SIGNAL(ambientChanged      ()), this, SLOT(onUpdate()));

        connect(player, SIGNAL(stateChanged()), this, SLOT(onState()));

        d->onSourceChanged();

        d->onUpdate();
    }

    emit playerChanged();
}

#endif // SK_NO_DECLARATIVEAMBIENT
