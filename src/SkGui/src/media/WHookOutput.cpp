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

// Sk includes
#include <WBarcodeWriter>
#ifndef SK_NO_QML
#include <WDeclarativePlayer>
#endif

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

    QObject::connect(backend, SIGNAL(currentOutputChanged()), q, SLOT(onOutputChanged()));

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
    const WBroadcastSource & source = client.source();

    WHookOutputData * data = getData(source);

    if (client.isConnected())
    {
        int index;

        if (data == NULL)
        {
            Q_Q(WHookOutput);

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
    }
    // NOTE: When we loose the connection we stop the playback and select the default output.
    else if (currentData == data)
    {
        backend->stop();

        currentData = NULL;

        backend->setCurrentOutput(0);

        setActive(false);
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
        if (d->backend->isPaused() || url.isEmpty())
        {
            setState    (WAbstractBackend::StateStopped);
            setStateLoad(WAbstractBackend::StateLoadDefault);
        }

        d->source = url;

        setDuration   (duration);
        setCurrentTime(currentTime);

        updateSource();
    }

    QStringList parameter;

#ifdef SK_NO_QML
    parameter.append(url);
#else
    WDeclarativePlayer * player = d->backend->player();

    if (player)
    {
        // NOTE: Sending VBML enables us to push more informations. As a result, the receiver will
        //       be able to apply them without relying on the network.
        QString vbml = player->toVbml();

        if (vbml.isEmpty() == false)
        {
             // NOTE: Maybe we could run this in a thread.
             parameter.append(WBarcodeWriter::encode(vbml, WBarcodeWriter::Vbml));
        }
        else parameter.append(url);
    }
    else parameter.append(url);
#endif

    parameter.append(QString::number(duration));
    parameter.append(QString::number(currentTime));

    d->client.addAndSend(WBroadcastMessage::SOURCE, parameter);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookOutput::play()
{
    Q_D(WHookOutput);

    setState(WAbstractBackend::StatePlaying);

    setStateLoad(WAbstractBackend::StateLoadStarting);

    d->client.addAndSend(WBroadcastMessage::PLAY);
}

/* Q_INVOKABLE virtual */ void WHookOutput::replay()
{
    Q_D(WHookOutput);

    setState(WAbstractBackend::StatePlaying);

    d->client.addAndSend(WBroadcastMessage::REPLAY);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookOutput::pause()
{
    Q_D(WHookOutput);

    WAbstractBackend::State state = d->backend->state();

    if (state == WAbstractBackend::StatePaused) return;

    if (state == WAbstractBackend::StatePlaying)
    {
        setState(WAbstractBackend::StatePaused);

        d->client.addAndSend(WBroadcastMessage::PAUSE);
    }
    else stop();
}

/* Q_INVOKABLE virtual */ void WHookOutput::stop()
{
    Q_D(WHookOutput);

    setState    (WAbstractBackend::StateStopped);
    setStateLoad(WAbstractBackend::StateLoadDefault);

    d->client.addAndSend(WBroadcastMessage::STOP);
}

/* Q_INVOKABLE virtual */ void WHookOutput::clear()
{
    stop();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookOutput::seek(int msec)
{
    setCurrentTime(msec);
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
