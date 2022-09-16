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

    QObject::connect(backend, SIGNAL(currentOutputChanged()), q, SLOT(onOutputChanged()));

    QObject::connect(&client, SIGNAL(connectedChanged()), q, SLOT(onConnectedChanged()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

WHookOutputData * WHookOutputPrivate::getSource(const WBroadcastSource & source)
{
    for (int i = 0; i < sources.count(); i++)
    {
        WHookOutputData & data = sources[i];

        if (data.source == source) return &data;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WHookOutputPrivate::onOutputChanged()
{
    const WBackendOutput * output = backend->currentOutputPointer();

    if (output == NULL || output->type != WAbstractBackend::OutputVbml) return;
}

void WHookOutputPrivate::onConnectedChanged()
{
    const WBroadcastSource & source = client.source();

    WHookOutputData * data = getSource(source);

    if (client.isConnected())
    {
        int index;

        if (data == NULL)
        {
            Q_Q(WHookOutput);

            WBackendOutput output(source.name, WAbstractBackend::OutputVbml);

            output.source = source.address + ':' + QString::number(source.port);

            WHookOutputData data(q->addOutput(output));

            data.source = source;

            sources.append(data);

            index = backend->indexOutput(data.output);
        }
        else index = backend->indexOutput(data->output);

        backend->setCurrentOutput(index);
    }
    else if (data && data->output == backend->currentOutputPointer())
    {
        backend->setCurrentOutput(-1);
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
    Q_D(WHookOutput); d->client.connectToHost(url);
}

/* Q_INVOKABLE */ void WHookOutput::disconnectHost()
{
    Q_D(WHookOutput); d->client.disconnectHost();
}

//-------------------------------------------------------------------------------------------------
// WAbstractHook reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookOutput::loadSource(const QString & url, int duration,
                                                                            int currentTime) {}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookOutput::play() {}

/* Q_INVOKABLE virtual */ void WHookOutput::replay() {}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookOutput::pause() {}

/* Q_INVOKABLE virtual */ void WHookOutput::stop() {}

/* Q_INVOKABLE virtual */ void WHookOutput::clear() {}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WHookOutput::seek(int msec) {}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractHook reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WHookOutput::hookCheckSource(const QString & url)
{
    Q_D(WHookOutput); return d->client.isConnected();
}

#endif // SK_NO_HOOKOUTPUT
