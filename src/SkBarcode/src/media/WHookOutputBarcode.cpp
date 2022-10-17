//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkBarcode.

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

#include "WHookOutputBarcode.h"

#ifndef SK_NO_HOOKOUTPUTBARCODE

// Sk includes
#include <WBarcodeWriter>
#ifndef SK_NO_QML
#include <WDeclarativePlayer>
#endif

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WHookOutputBarcode_p.h"

WHookOutputBarcodePrivate::WHookOutputBarcodePrivate(WHookOutputBarcode * p)
    : WHookOutputPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WHookOutputBarcodePrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WHookOutputBarcode::WHookOutputBarcode(WAbstractBackend * backend)
    : WHookOutput(new WHookOutputBarcodePrivate(this), backend)
{
    Q_D(WHookOutputBarcode); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WHookOutput reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WHookOutputBarcode::onSendSource(const QString & url,
                                                    int duration, int currentTime)
{
    Q_D(WHookOutputBarcode);

    QStringList parameters;

#ifdef SK_NO_QML
    parameters.append(url);
#else
    WDeclarativePlayer * player = d->backend->player();

    if (player)
    {
        // NOTE: Sending VBML enables us to push more informations. As a result, the receiver will
        //       be able to apply them without relying on the network.
        QString vbml = player->toVbml();

        if (vbml.isEmpty())
        {
            parameters.append(url);
        }
        // NOTE: Maybe we could run this in a thread.
        else parameters.append(WBarcodeWriter::encode(vbml, WBarcodeWriter::Vbml));
    }
    else parameters.append(url);
#endif

    parameters.append(QString::number(duration));
    parameters.append(QString::number(currentTime));

    d->client.sendMessage(WBroadcastMessage::SOURCE, parameters);
}

#endif // SK_NO_HOOKOUTPUTBARCODE
