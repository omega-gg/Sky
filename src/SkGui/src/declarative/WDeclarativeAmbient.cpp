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
#include <WPlayer>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeAmbientPrivate::WDeclarativeAmbientPrivate(WDeclarativeAmbient * p)
    : WDeclarativePlayerPrivate(p) {}

void WDeclarativeAmbientPrivate::init()
{
    playerSource = NULL;

    player->setRepeat(WPlayer::RepeatOne);

    player->setOutput(WAbstractBackend::OutputAudio);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeAmbientPrivate::onSourceChanged()
{
    QString source = WControllerPlaylist::cleanSource(playerSource->source());

    if (currentSource == source) return;

    currentSource = source;

    hash.clear();

    hash.insert(source, -1);

    q_func()->clear();
}

void WDeclarativeAmbientPrivate::onUpdate()
{
    Q_Q(WDeclarativeAmbient);

    if (q->backend() == NULL || playerSource->hasOutput())
    {
        if (q->source().isEmpty()) return;

        hash.insert(currentSource, q->currentTime());

        q->clear();

        return;
    }

    QString ambient = playerSource->ambient();

    if (ambient.isEmpty())
    {
        // NOTE: We clear the ambient source as late as possible to avoid blanks.
        if (q->source().isEmpty()/* || playerSource->isDefault() == false*/) return;

        hash.insert(currentSource, q->currentTime());

        q->clear();

        return;
    }

    if (q->source() == ambient) return;

    q->setSource(ambient);

    int msec = hash.value(currentSource);

    if (msec != -1) q->seek(msec);

    if (playerSource->state() != WAbstractBackend::StatePlaying) return;

    q->play();
}

void WDeclarativeAmbientPrivate::onState()
{
    Q_Q(WDeclarativeAmbient);

    if (q->source().isEmpty()) return;

    WAbstractBackend::State state = playerSource->state();

    if      (state == WAbstractBackend::StateStopped) q->stop ();
    else if (state == WAbstractBackend::StatePlaying) q->play ();
    else                                              q->pause();
}

void WDeclarativeAmbientPrivate::onVolume()
{
    Q_Q(WDeclarativeAmbient);

    q->setVolume(playerSource->volume());
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

WDeclarativePlayer * WDeclarativeAmbient::playerSource() const
{
    Q_D(const WDeclarativeAmbient); return d->playerSource;
}

void WDeclarativeAmbient::setPlayerSource(WDeclarativePlayer * playerSource)
{
    Q_D(WDeclarativeAmbient);

    if (d->playerSource == playerSource) return;

    if (d->playerSource)
    {
        disconnect(d->playerSource, 0, this, 0);

        clear();

        d->currentSource = QString();

        d->hash.clear();
    }

    d->playerSource = playerSource;

    if (playerSource)
    {
        connect(playerSource, SIGNAL(sourceChanged()), this, SLOT(onSourceChanged()));

        connect(playerSource, SIGNAL(backendChanged      ()), this, SLOT(onUpdate()));
        connect(playerSource, SIGNAL(stateLoadChanged    ()), this, SLOT(onUpdate()));
        connect(playerSource, SIGNAL(currentOutputChanged()), this, SLOT(onUpdate()));
        connect(playerSource, SIGNAL(ambientChanged      ()), this, SLOT(onUpdate()));

        connect(playerSource, SIGNAL(stateChanged()),  this, SLOT(onState ()));
        connect(playerSource, SIGNAL(volumeChanged()), this, SLOT(onVolume()));

        setVolume(playerSource->volume());

        d->onSourceChanged();

        d->onUpdate();
    }

    emit playerSourceChanged();
}

#endif // SK_NO_DECLARATIVEAMBIENT
