//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkMedia module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WVlcEngine.h"

#ifndef SK_NO_VLCENGINE

// Qt includes
#include <QCoreApplication>

// Vlc includes
#include <vlc/vlc.h>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WVlcEngine_p.h"

WVlcEnginePrivate::WVlcEnginePrivate(WVlcEngine * p) : WPrivate(p) {}

/* virtual */ WVlcEnginePrivate::~WVlcEnginePrivate()
{
    Q_Q(WVlcEngine);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WVlcEnginePrivate::EventStop)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------

void WVlcEnginePrivate::init(QThread * thread)
{
    Q_Q(WVlcEngine);

    instance = NULL;

    if (thread) q->moveToThread(thread);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WVlcEnginePrivate::EventStart)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WVlcEngine::WVlcEngine(QThread * thread, QObject * parent)
    : QObject(parent), WPrivatable(new WVlcEnginePrivate(this))
{
    Q_D(WVlcEngine); d->init(thread);
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

bool WVlcEngine::event(QEvent * event)
{
    Q_D(WVlcEngine);

    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WVlcEnginePrivate::EventStart))
    {
        const char * const args[] =
        {
            "--intf=dummy",    /* Don't use any interface */
            "--dummy-quiet",   /* Don't open command prompt */
            "--ignore-config", /* Don't use VLC's config  */
            "--no-spu",        /* Don't use sub-pictures */
            "--no-osd",        /* No video overlay */
            "--no-stats",
            "--no-media-library",
            "--input-fast-seek",
            "--avcodec-fast"
            //"--avcodec-dr",
            //"--avcodec-hurry-up",
            //"--avcodec-hw=any",
            //"--avcodec-error-resilience=1",
            //"--avcodec-workaround-bugs=1",
            //"--avcodec-skip-frame=0",
            //"--avcodec-skip-idct=0",
            //"--avcodec-vismv=0",
            //"--avcodec-lowres=0",
            //"--avcodec-skiploopfilter=0",
            //"--network-caching=200",
            //"--cr-average=10000",
            //"--clock-synchro=0",
            //"--verbose=2"
        };

        d->instance = libvlc_new(sizeof(args) / sizeof(*args), args);

        if (libvlc_errmsg())
        {
            qWarning("WVlcEngine::event: LibVLC error: %s", libvlc_errmsg());
        }

        return true;
    }
    else if (d->instance == NULL)
    {
        return QObject::event(event);
    }
    else if (type == static_cast<QEvent::Type> (WVlcEnginePrivate::EventStop))
    {
        libvlc_release(d->instance);

        d->instance = NULL;

        return true;
    }
    else return QObject::event(event);
}

#endif // SK_NO_VLCENGINE
