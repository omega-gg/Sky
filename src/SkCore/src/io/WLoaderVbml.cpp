//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#include "WLoaderVbml.h"

// Qt includes
#include <WAbstractThreadAction>
#include <WAbstractThreadReply>
#include <QBuffer>

// Sk includes
#include <WControllerFile>
#include <WControllerDownload>
#include <WUnzipper>

#ifndef SK_NO_LOADERVBML

//=================================================================================================
// WLoaderVbmlRead
//=================================================================================================

class WLoaderVbmlRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WLoaderVbmlRead(const QString & uri)
    {
        this->uri = uri;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    QString uri;
};

class WLoaderVbmlReply: public WAbstractThreadReply
{
    Q_OBJECT

public:
    WLoaderVbmlReply() { action = NULL; }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

signals:
    void loaded(WLoaderVbmlRead * action, const QByteArray & data);

public: // Variables
    WLoaderVbmlRead * action;

    QByteArray text;
};

//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadReply * WLoaderVbmlRead::createReply() const
{
    return new WLoaderVbmlReply;
}

/* virtual */ bool WLoaderVbmlRead::run()
{
    WLoaderVbmlReply * reply = qobject_cast<WLoaderVbmlReply *> (this->reply());

    reply->action = this;

    // NOTE: Removing the 'vbml:' part.
    uri = uri.remove(0, 5);

#ifdef QT_4
    // FIXME Qt4: This version does not support encoding flags.
    QByteArray data = QByteArray::fromBase64(uri.toUtf8());
#else
    QByteArray data = QByteArray::fromBase64(uri.toUtf8(), QByteArray::Base64UrlEncoding |
                                                           QByteArray::OmitTrailingEquals);
#endif

    reply->text = WUnzipper::extract(data);

    return true;
}

/* virtual */ void WLoaderVbmlReply::onCompleted(bool)
{
    emit loaded(action, text);
}

//=================================================================================================
// WLoaderVbmlPrivate
//=================================================================================================

WLoaderVbmlPrivate::WLoaderVbmlPrivate(WLoaderVbml * p) : WAbstractLoaderPrivate(p) {}

void WLoaderVbmlPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLoaderVbmlPrivate::onLoaded(WLoaderVbmlRead * action, const QByteArray & data)
{
    Q_Q(WLoaderVbml);

    QBuffer * buffer = static_cast<QBuffer *> (replies.key(action));

    replies.remove(buffer);

    buffer->setData(data);

    buffer->open(QIODevice::ReadOnly);

    q->complete(buffer);
}

//=================================================================================================
// WLoaderVbml
//=================================================================================================

/* explicit */ WLoaderVbml::WLoaderVbml(QObject * parent)
    : WAbstractLoader(new WLoaderVbmlPrivate(this), parent)
{
    Q_D(WLoaderVbml); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QIODevice * WLoaderVbml::load(WRemoteData * data)
{
    Q_D(WLoaderVbml);

    WLoaderVbmlRead * action = new WLoaderVbmlRead(data->url());

    QBuffer * buffer = new QBuffer;

    d->replies.insert(buffer, action);

    WLoaderVbmlReply * reply = qobject_cast<WLoaderVbmlReply *>
                               (wControllerFile->startReadAction(action));

    connect(reply, SIGNAL(loaded(WLoaderVbmlRead *, const QByteArray &)),
            this,  SLOT(onLoaded(WLoaderVbmlRead *, const QByteArray &)));

    return buffer;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractLoader reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderVbml::abort(QIODevice * reply)
{
    Q_D(WLoaderVbml);

    WAbstractThreadAction * action = d->replies.take(reply);

    disconnect(action->reply(), 0, this, 0);

    reply->open(QIODevice::ReadOnly);

    complete(reply);

    action->abortAndDelete();
}

#endif // SK_NO_LOADERVBML

#include "WLoaderVbml.moc"
