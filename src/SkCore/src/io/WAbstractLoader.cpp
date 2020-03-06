//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WAbstractLoader.h"

#ifndef SK_NO_ABSTRACTLOADER

// Sk includes
#include <WControllerDownload>

// Private includes
#include <private/WControllerDownload_p>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int ABSTRACTLOADER_MAX_REDIRECT = 16;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WAbstractLoader_p.h"

WAbstractLoaderPrivate::WAbstractLoaderPrivate(WAbstractLoader * p) : WPrivate(p) {}

/* virtual */ WAbstractLoaderPrivate::~WAbstractLoaderPrivate()
{
    QHashIterator<QIODevice *, WRemoteData *> i(replies);

    W_GET_CONTROLLER(WControllerDownload, controller);

    if (controller)
    {
        Q_Q(WAbstractLoader);

        WControllerDownloadPrivate * p = controller->d_func();

        bool process = false;

        while (i.hasNext())
        {
            i.next();

            clear(i.key());

            p->jobs.removeOne(i.value());

            process = true;
        }

        p->clearLoader(q);

        if (process) p->processJobs();
    }
    else
    {
        while (i.hasNext())
        {
            i.next();

            clear(i.key());
        }
    }

    replies.clear();
}

//-------------------------------------------------------------------------------------------------

void WAbstractLoaderPrivate::init()
{
    maxRedirect = ABSTRACTLOADER_MAX_REDIRECT;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WAbstractLoaderPrivate::clear(QIODevice * reply)
{
    Q_Q(WAbstractLoader);

    WRemoteData * data = replies.take(reply);

    data->_loader = NULL;

    q->abort(reply);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WAbstractLoader::WAbstractLoader(QObject * parent)
    : QObject(parent), WPrivatable(new WAbstractLoaderPrivate(this))
{
    Q_D(WAbstractLoader); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WAbstractLoader::WAbstractLoader(WAbstractLoaderPrivate * p, QObject * parent)
    : QObject(parent), WPrivatable(p)
{
    Q_D(WAbstractLoader); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractLoader::get(WRemoteData * data)
{
    Q_ASSERT(data);

    Q_D(WAbstractLoader);

    QIODevice * reply = load(data);

    reply->setParent(NULL);

    data->_reply = reply;

    d->replies.insert(reply, data);
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractLoader::abort(QIODevice *) {}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

WRemoteData * WAbstractLoader::getData(QIODevice * reply) const
{
    Q_D(const WAbstractLoader);

    return d->replies.value(reply);
}

//-------------------------------------------------------------------------------------------------

void WAbstractLoader::redirect(QIODevice * reply, const QString & url)
{
    Q_D(WAbstractLoader);

    WRemoteData * data = d->replies.take(reply);

    if (data == NULL)
    {
        qWarning("WAbstractLoader::redirect: Cannot redirect. Invalid reply.");
    }
    else if (data->_redirectCount < d->maxRedirect)
    {
        data->_url = url;

        data->_redirectCount++;

        get(data);

        reply->deleteLater();

        qDebug("Redirected %s", url.C_STR);
    }
    else
    {
        qWarning("WAbstractLoader::redirect: Get failed. Maximum redirects reached.");

        data->_error = "Maximum redirects reached";

        data->_loader = NULL;

        wControllerDownload->d_func()->completeJob(data);

        emit data->loaded(data);
    }
}

void WAbstractLoader::complete(QIODevice * reply)
{
    Q_D(WAbstractLoader);

    WRemoteData * data = d->replies.take(reply);

    if (data)
    {
        data->_loader = NULL;

        wControllerDownload->d_func()->completeJob(data);

        emit data->loaded(data);
    }
    else qWarning("WAbstractLoader::complete: Cannot complete. Invalid reply.");
}

//-------------------------------------------------------------------------------------------------

void WAbstractLoader::setError(WRemoteData * data, const QString & error)
{
    data->_error = error;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

int WAbstractLoader::maxRedirect() const
{
    Q_D(const WAbstractLoader); return d->maxRedirect;
}

void WAbstractLoader::setMaxRedirect(int max)
{
    Q_D(WAbstractLoader);

    if (d->maxRedirect == max) return;

    d->maxRedirect = max;

    emit maxRedirectChanged();
}

#endif // SK_NO_ABSTRACTLOADER
