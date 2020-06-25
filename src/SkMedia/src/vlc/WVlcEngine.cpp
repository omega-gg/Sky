//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMedia.

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

#include "WVlcEngine.h"

#ifndef SK_NO_VLCENGINE

// Qt includes
#include <QCoreApplication>

// FIXME MSVC: ssize_t is required by vlc headers.
#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

// Vlc includes
#include <vlc/vlc.h>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WVlcEngine_p.h"

WVlcEnginePrivate::WVlcEnginePrivate(WVlcEngine * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WVlcEnginePrivate::init(QThread * thread)
{
    Q_Q(WVlcEngine);

    instance = NULL;

    if (thread) q->moveToThread(thread);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WVlcEnginePrivate::EventCreate)),
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
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcEngine::deleteInstance()
{
    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcEnginePrivate::EventClear)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WVlcEngine::event(QEvent * event)
{
    Q_D(WVlcEngine);

    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WVlcEnginePrivate::EventCreate))
    {
        const char * const args[] =
        {
            "--intf=dummy", /* No interface     */
#ifdef Q_OS_WIN
            "--dummy-quiet", /* No command-line  */
#elif defined(Q_OS_MAC)
            "--vout=macosx",
#endif
            "--ignore-config",     /* No configuration */
            "--no-spu",            /* No sub-pictures  */
            "--no-osd",            /* No video overlay */
            "--no-stats",          /* No statistics    */
#ifndef Q_OS_MAC
            "--no-media-library"   /* No Media Library */
#else
            "--no-media-library"
#endif
            //"--http-reconnect",  /* Auto reconnect   */
            //"--avcodec-fast",    /* Speed tricks     */
            //"--input-fast-seek", /* Fast seek        */
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

#ifdef Q_OS_MAC
        qputenv("VLC_PLUGIN_PATH", QCoreApplication::applicationDirPath().toLatin1());
#endif

        d->instance = libvlc_new(sizeof(args) / sizeof(*args), args);

#ifdef Q_OS_LINUX
        if (d->instance == NULL)
        {
            qFatal("WVlcEngine::event: Cannot create VLC instance. Is VLC installed ?");
        }
#endif

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
    else if (type == static_cast<QEvent::Type> (WVlcEnginePrivate::EventClear))
    {
        libvlc_release(d->instance);

        d->instance = NULL;

        return true;
    }
    else return QObject::event(event);
}

#endif // SK_NO_VLCENGINE
