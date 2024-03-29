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

#include "WLoaderBarcode.h"

#ifndef SK_NO_LOADERBARCODE

// Qt includes
#include <QBuffer>
#include <QImage>

// Sk includes
#include <WControllerFile>
#include <WControllerDownload>
#include <WControllerPlaylist>
#include <WAbstractThreadAction>
#include <WAbstractThreadReply>
#include <WBarcodeReader>
#include <WUnzipper>

//=================================================================================================
// WLoaderBarcodeRead
//=================================================================================================

class WLoaderBarcodeRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WLoaderBarcodeRead(const QByteArray & data)
    {
        this->data = data;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QByteArray data;
};

class WLoaderBarcodeReply: public WAbstractThreadReply
{
    Q_OBJECT

public:
    WLoaderBarcodeReply() { action = NULL; }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

signals:
    void loaded(WLoaderBarcodeRead * action, const QByteArray & data, const QString & source);

public: // Variables
    WLoaderBarcodeRead * action;

    QByteArray data;

    QString source;
};

//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadReply * WLoaderBarcodeRead::createReply() const
{
    return new WLoaderBarcodeReply;
}

/* virtual */ bool WLoaderBarcodeRead::run()
{
    WLoaderBarcodeReply * reply = qobject_cast<WLoaderBarcodeReply *> (this->reply());

    reply->action = this;

    QImage image;

    image.loadFromData(data);

    QByteArray data;
    QString    source;

    QStringList texts = WBarcodeReader::reads(image);

    foreach (const QString & text, texts)
    {
        QString uri = WControllerPlaylist::generateSource(text);

        if (WControllerPlaylist::urlIsVbmlUri(uri) == false)
        {
            // NOTE: When we have multiple valid barcodes in the image we skip them.
            if (data.isEmpty() == false) return true;

            data = text.toUtf8();

            continue;
        }

        // NOTE: Removing the 'vbml:' part.
        uri.remove(0, 5);

        QByteArray array = WUnzipper::extractBase64(uri.toUtf8());

        if (WControllerPlaylist::textIsVbmlHash(array) == false)
        {
            // NOTE: When we have multiple valid barcodes in the image we skip them.
            if (data.isEmpty() == false) return true;

            data = array;

            source = uri;

            continue;
        }
    }

    reply->data = data;

    reply->source = source;

    return true;
}

/* virtual */ void WLoaderBarcodeReply::onCompleted(bool)
{
    emit loaded(action, data, source);
}

//=================================================================================================
// WLoaderBarcodePrivate
//=================================================================================================

WLoaderBarcodePrivate::WLoaderBarcodePrivate(WLoaderBarcode * p) : WAbstractLoaderPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WLoaderBarcodePrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLoaderBarcodePrivate::onLoaded(WRemoteData * data)
{
    Q_Q(WLoaderBarcode);

    QBuffer * buffer = static_cast<QBuffer *> (replies.key(data));

    replies.remove(buffer);

    if (data->hasError())
    {
        q->setError(q->getData(buffer), data->error());
    }

    WLoaderBarcodeRead * action = new WLoaderBarcodeRead(data->readAll());

    actions.insert(buffer, action);

    WLoaderBarcodeReply * reply = qobject_cast<WLoaderBarcodeReply *>
                                  (wControllerFile->startReadAction(action));

    QObject::connect(reply,
                     SIGNAL(loaded(WLoaderBarcodeRead *, const QByteArray &, const QString &)),
                     q,
                     SLOT(onAction(WLoaderBarcodeRead *, const QByteArray &, const QString &)));

    QObject::disconnect(data, 0, q, 0);

    data->deleteLater();
}

void WLoaderBarcodePrivate::onAction(WLoaderBarcodeRead * action, const QByteArray & data,
                                                                  const QString    & source)
{
    Q_Q(WLoaderBarcode);

    QBuffer * buffer = static_cast<QBuffer *> (actions.key(action));

    actions.remove(buffer);

    if (source.isEmpty() == false)
    {
        // NOTE: We want to set the VBML uri instead of the image url.
        q->applyUrl(q->getData(buffer), source);
    }

    buffer->setData(data);

    buffer->open(QIODevice::ReadOnly);

    q->complete(buffer);
}

//=================================================================================================
// WLoaderBarcode
//=================================================================================================

/* explicit */ WLoaderBarcode::WLoaderBarcode(QObject * parent)
    : WAbstractLoader(new WLoaderBarcodePrivate(this), parent)
{
    Q_D(WLoaderBarcode); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QIODevice * WLoaderBarcode::load(WRemoteData * data)
{
    Q_D(WLoaderBarcode);

    QString source = data->url();

    if (source.startsWith("image:", Qt::CaseInsensitive))
    {
        // NOTE: Removing the 'image:' part.
        source = source.remove(0, 6);
    }

    WRemoteData * reply = wControllerDownload->getData(source, this,
                                                       QNetworkRequest::NormalPriority,
                                                       data->parameters());

    QBuffer * buffer = new QBuffer;

    d->replies.insert(buffer, reply);

    connect(reply, SIGNAL(loaded(WRemoteData *)), this, SLOT(onLoaded(WRemoteData *)));

    return buffer;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderBarcode::abort(QIODevice * reply)
{
    Q_D(WLoaderBarcode);

    WRemoteData * data = d->replies.take(reply);

    if (data)
    {
        disconnect(data, 0, this, 0);

        reply->open(QIODevice::ReadOnly);

        complete(reply);

        delete data;
    }

    WAbstractThreadAction * action = d->actions.take(reply);

    if (action)
    {
        disconnect(action->reply(), 0, this, 0);

        reply->open(QIODevice::ReadOnly);

        complete(reply);

        action->abortAndDelete();
    }
}

#endif // SK_NO_LOADERBARCODE

#include "WLoaderBarcode.moc"
