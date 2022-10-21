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

#include "WHookOutput.h"

#ifndef SK_NO_HOOKOUTPUT

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WHookOutputPrivate::WHookOutputPrivate(WHookOutput * p) : WAbstractHookPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WHookOutputPrivate::init()
{
    Q_Q(WHookOutput);

    currentData = NULL;

    active = false;

    QObject::connect(&client, SIGNAL(connectedChanged()), q, SIGNAL(connectedChanged()));

    QObject::connect(backend, SIGNAL(currentOutputChanged()), q, SLOT(onCurrentOutputChanged()));

    QObject::connect(&client, SIGNAL(connectedChanged()), q, SLOT(onConnectedChanged()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

WHookOutputData * WHookOutputPrivate::getData(const WBroadcastSource & source)
{
    for (int i = 0; i < datas.count(); i++)
    {
        WHookOutputData & data = datas[i];

        if (data.source == source) return &data;
    }

    return NULL;
}

WHookOutputData * WHookOutputPrivate::getData(const WBackendOutput * output)
{
    for (int i = 0; i < datas.count(); i++)
    {
        WHookOutputData & data = datas[i];

        if (data.output == output) return &data;
    }

    return NULL;
}

void WHookOutputPrivate::setActive(bool active)
{
    if (this->active == active) return;

    Q_Q(WHookOutput);

    this->active = active;

    emit q->hookUpdated();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WHookOutputPrivate::onOutputChanged()
{
    client.sendMessage(WBroadcastMessage::OUTPUT,
                       WAbstractBackend::outputToString(backend->output()));
}

void WHookOutputPrivate::onQualityChanged()
{
    client.sendMessage(WBroadcastMessage::QUALITY,
                       WAbstractBackend::qualityToString(backend->quality()));
}

void WHookOutputPrivate::onFillModeChanged()
{
    client.sendMessage(WBroadcastMessage::FILLMODE,
                       WAbstractBackend::fillModeToString(backend->fillMode()));
}

void WHookOutputPrivate::onSpeedChanged()
{
    client.sendMessage(WBroadcastMessage::SPEED, QString::number(backend->speed()));
}

void WHookOutputPrivate::onVideoChanged()
{
    client.sendMessage(WBroadcastMessage::VIDEO, QString::number(backend->trackVideo()));
}

void WHookOutputPrivate::onAudioChanged()
{
    client.sendMessage(WBroadcastMessage::AUDIO, QString::number(backend->trackAudio()));
}

void WHookOutputPrivate::onSubtitleChanged()
{
    client.sendMessage(WBroadcastMessage::SUBTITLE, backend->subtitle());
}

//-------------------------------------------------------------------------------------------------

void WHookOutputPrivate::onCurrentOutputChanged()
{
    WHookOutputData * data = getData(backend->currentOutputPointer());

    if (currentData == data) return;

    if (currentData)
    {
        client.disconnectHost();

        setActive(false);
    }

    currentData = data;

    if (data) client.connectToHost(data->source);
}

void WHookOutputPrivate::onConnectedChanged()
{
    Q_Q(WHookOutput);

    const WBroadcastSource & source = client.source();

    WHookOutputData * data = getData(source);

    if (client.isConnected())
    {
        int index;

        if (data == NULL)
        {
            WBackendOutput output(source.name, WAbstractBackend::OutputVbml);

            WHookOutputData data(q->addOutput(output));

            data.source = source;

            datas.append(data);

            currentData = &(datas.last());

            index = backend->indexOutput(data.output);
        }
        else
        {
            currentData = data;

            index = backend->indexOutput(data->output);
        }

        backend->setCurrentOutput(index);

        setActive(true);

        QObject::connect(&client, SIGNAL(reply(const WBroadcastReply &)),
                         q,       SLOT(onReply(const WBroadcastReply &)));

        QObject::connect(backend, SIGNAL(outputChanged    ()), q, SLOT(onOutputChanged  ()));
        QObject::connect(backend, SIGNAL(qualityChanged   ()), q, SLOT(onQualityChanged ()));
        QObject::connect(backend, SIGNAL(fillModeChanged  ()), q, SLOT(onFillModeChanged()));
        QObject::connect(backend, SIGNAL(speedChanged     ()), q, SLOT(onSpeedChanged   ()));
        QObject::connect(backend, SIGNAL(trackVideoChanged()), q, SLOT(onVideoChanged   ()));
        QObject::connect(backend, SIGNAL(trackAudioChanged()), q, SLOT(onAudioChanged   ()));
        QObject::connect(backend, SIGNAL(subtitleChanged  ()), q, SLOT(onSubtitleChanged()));

        // NOTE: Propagating backend's current settings.
        onOutputChanged  ();
        onQualityChanged ();
        onFillModeChanged();
        onSpeedChanged   ();
        onVideoChanged   ();
        onAudioChanged   ();
        onSubtitleChanged();
    }
    else
    {
        QObject::disconnect(&client, SIGNAL(reply(const WBroadcastReply &)),
                            q,       SLOT(onReply(const WBroadcastReply &)));

        QObject::disconnect(backend, SIGNAL(outputChanged    ()), q, SLOT(onOutputChanged  ()));
        QObject::disconnect(backend, SIGNAL(qualityChanged   ()), q, SLOT(onQualityChanged ()));
        QObject::disconnect(backend, SIGNAL(fillModeChanged  ()), q, SLOT(onFillModeChanged()));
        QObject::disconnect(backend, SIGNAL(speedChanged     ()), q, SLOT(onSpeedChanged   ()));
        QObject::disconnect(backend, SIGNAL(trackVideoChanged()), q, SLOT(onVideoChanged   ()));
        QObject::disconnect(backend, SIGNAL(trackAudioChanged()), q, SLOT(onAudioChanged   ()));
        QObject::disconnect(backend, SIGNAL(subtitleChanged  ()), q, SLOT(onSubtitleChanged()));

        if (currentData != data) return;

        //-----------------------------------------------------------------------------------------
        // NOTE: When we loose the connection we stop the playback and select the default output.

        backend->stop();

        currentData = NULL;

        backend->setCurrentOutput(0);

        setActive(false);
    }
}

void WHookOutputPrivate::onReply(const WBroadcastReply & reply)
{
    WBroadcastReply::Type type = reply.type;

    if (type == WBroadcastReply::STATE)
    {
        Q_Q(WHookOutput);

        q->setState(WAbstractBackend::stateFromString(reply.parameters.first()));
    }
    else if (type == WBroadcastReply::STATELOAD)
    {
        Q_Q(WHookOutput);

        q->setStateLoad(WAbstractBackend::stateLoadFromString(reply.parameters.first()));
    }
    else if (type == WBroadcastReply::LIVE)
    {
        Q_Q(WHookOutput);

        q->setLive(reply.parameters.first().toInt());
    }
    else if (type == WBroadcastReply::ENDED)
    {
        Q_Q(WHookOutput);

        q->setEnded(reply.parameters.first().toInt());
    }
    else if (type == WBroadcastReply::TIME)
    {
        Q_Q(WHookOutput);

        q->setCurrentTime(reply.parameters.first().toInt());
    }
    else if (type == WBroadcastReply::DURATION)
    {
        Q_Q(WHookOutput);

        q->setDuration(reply.parameters.first().toInt());
    }
    else if (type == WBroadcastReply::PROGRESS)
    {
        Q_Q(WHookOutput);

        q->setProgress(reply.parameters.first().toFloat());
    }
    else if (type == WBroadcastReply::OUTPUT)
    {
        Q_Q(WHookOutput);

        q->setOutputActive(WAbstractBackend::outputFromString(reply.parameters.first()));
    }
    else if (type == WBroadcastReply::QUALITY)
    {
        Q_Q(WHookOutput);

        q->setQualityActive(WAbstractBackend::qualityFromString(reply.parameters.first()));
    }
    else if (type == WBroadcastReply::VIDEOS)
    {
        Q_Q(WHookOutput);

        const QStringList & parameters = reply.parameters;

        int id = parameters.first().toInt();

        QList<WBackendTrack> videos;

        for (int i = 1; i < parameters.count(); i++)
        {
            videos.append(WAbstractBackend::trackFromString(parameters.at(i)));
        }

        q->applyVideos(videos, id);
    }
    else if (type == WBroadcastReply::AUDIOS)
    {
        Q_Q(WHookOutput);

        const QStringList & parameters = reply.parameters;

        int id = parameters.first().toInt();

        QList<WBackendTrack> audios;

        for (int i = 1; i < parameters.count(); i++)
        {
            audios.append(WAbstractBackend::trackFromString(parameters.at(i)));
        }

        q->applyAudios(audios, id);
    }
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WHookOutput::WHookOutput(WAbstractBackend * backend)
    : WAbstractHook(new WHookOutputPrivate(this), backend)
{
    Q_D(WHookOutput); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WHookOutput::WHookOutput(WHookOutputPrivate * p, WAbstractBackend * backend)
    : WAbstractHook(p, backend)
{
    Q_D(WHookOutput); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WHookOutput::connectToHost(const QString & url)
{
    Q_D(WHookOutput);

    if (d->client.isConnected())
    {
        qWarning("WHookOutput::connectToHost: Already connected.");

        return;
    }

    d->client.connectToHost(url);
}

/* Q_INVOKABLE */ void WHookOutput::disconnectHost()
{
    Q_D(WHookOutput);

    d->setActive(false);

    d->client.disconnectHost();
}

//-------------------------------------------------------------------------------------------------
// WAbstractHook reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookOutput::loadSource(const QString & url, int duration,
                                                                            int currentTime)
{
    Q_D(WHookOutput);

    if (d->source == url)
    {
        setDuration   (duration);
        setCurrentTime(currentTime);
    }
    else
    {
        d->source = url;

        setDuration   (duration);
        setCurrentTime(currentTime);

        updateSource();
    }

    QStringList parameters;

    parameters.append(getSource(url));
    parameters.append(QString::number(duration));
    parameters.append(QString::number(currentTime));

    d->client.sendMessage(WBroadcastMessage::SOURCE, parameters);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookOutput::play()
{
    Q_D(WHookOutput);

    d->client.sendMessage(WBroadcastMessage::PLAY);
}

/* Q_INVOKABLE virtual */ void WHookOutput::replay()
{
    Q_D(WHookOutput);

    d->client.sendMessage(WBroadcastMessage::REPLAY);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookOutput::pause()
{
    Q_D(WHookOutput);

    d->client.sendMessage(WBroadcastMessage::PAUSE);
}

/* Q_INVOKABLE virtual */ void WHookOutput::stop()
{
    Q_D(WHookOutput);

    d->client.sendMessage(WBroadcastMessage::STOP);
}

/* Q_INVOKABLE virtual */ void WHookOutput::clear()
{
    Q_D(WHookOutput);

    stop();

    d->source = QString();

    setDuration   (-1);
    setCurrentTime(-1);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookOutput::seek(int msec)
{
    Q_D(WHookOutput);

    d->client.sendMessage(WBroadcastMessage::SEEK, QString::number(msec));
}

//-------------------------------------------------------------------------------------------------
// Virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ QString WHookOutput::getSource(const QString & url) const
{
    return url;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractHook reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WHookOutput::hookCheck(const QString &)
{
    Q_D(WHookOutput); return d->active;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WHookOutput::isConnected() const
{
    Q_D(const WHookOutput); return d->client.isConnected();
}

#endif // SK_NO_HOOKOUTPUT
