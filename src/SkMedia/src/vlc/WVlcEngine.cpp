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

// Sk includes
#include <WControllerFile>
#include <WVlcPlayer>

// FIXME MSVC: ssize_t is required by vlc headers.
#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

// VLC includes
#include <vlc/vlc.h>

// Private includes
#include <private/WVlcPlayer_p>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WVlcEngine_p.h"

WVlcEnginePrivate::WVlcEnginePrivate(WVlcEngine * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WVlcEnginePrivate::init(const QStringList & options, QThread * thread)
{
    Q_Q(WVlcEngine);

    this->thread = thread;

    instance = NULL;

    this->options = options;

    log = false;

    if (thread == NULL)
    {
        create();

        return;
    }

    q->moveToThread(thread);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WVlcEnginePrivate::EventCreate)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WVlcEnginePrivate::create()
{
#ifdef Q_OS_MACOS
    qputenv("VLC_PLUGIN_PATH", QCoreApplication::applicationDirPath().toLatin1());
#endif

    if (options.isEmpty())
    {
        const char * const args[] =
        {
            "--intf=dummy",             /* No interface     */
#ifdef Q_OS_WIN
#if LIBVLC_VERSION_MAJOR < 4
            "--dummy-quiet",            /* No command-line  */
#endif
#elif defined(Q_OS_MACOS)
            "--vout=macosx",
#endif
            "--ignore-config",          /* No configuration */
            "--no-spu",                 /* No sub-pictures  */
            "--no-osd",                 /* No video overlay */
            "--no-stats",               /* No statistics    */
            "--no-media-library",       /* No Media Library */
            "--text-renderer=none",     /* No FreeType      */
            // NOTE VLC 3.0.20: This is required for the preferred-resolution to be applied.
            "--adaptive-logic=highest"  /* High resolution  */
            // FIXME VLC 3.0.18: Sometimes the end of the video is reached too soon.
            //"--http-reconnect",         /* Auto reconnect   */
            // NOTE: This is useful for the mkv default language.
            //"--audio-language=en",      /* Audio english    */
            //"--input-fast-seek",        /* Fast seek        */
            //"--avcodec-fast",           /* Speed tricks     */
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

        instance = libvlc_new(sizeof(args) / sizeof(*args), args);
    }
    else
    {
        int argc = options.length();

        char ** args = new char * [argc];

        int index = 0;

        foreach (const QString & string, options)
        {
            args[index] = new char [string.length()];

            strcpy(args[index], string.C_STR);

            index++;
        }

        instance = libvlc_new(argc, args);
    }

#ifdef Q_OS_LINUX
    if (instance == NULL)
    {
        qFatal("WVlcEnginePrivate::create: Cannot create VLC instance. Is VLC installed ?");
    }
#endif

    if (libvlc_errmsg())
    {
        qWarning("WVlcEnginePrivate::create: LibVLC error: %s", libvlc_errmsg());
    }
}

void WVlcEnginePrivate::startLog()
{
    if (log) return;

    log = true;

    libvlc_log_set(instance, onLog, NULL);
}

void WVlcEnginePrivate::deleteInstance()
{
    clearDiscoverers();

    libvlc_release(instance);

    instance = NULL;
}

void WVlcEnginePrivate::startScan(WVlcPlayerPrivate * player, bool enabled)
{
    if (players.isEmpty())
    {
        if (enabled == false) return;
    }
    else if (enabled == false)
    {
        players.removeOne(player);

        if (players.isEmpty()) clearDiscoverers();

        return;
    }

    players.append(player);

    if (players.count() != 1) return;

    libvlc_rd_description_t ** services;

    ssize_t count = libvlc_renderer_discoverer_list_get(instance, &services);

    QStringList names;

    for (int i = 0; i < count; i++)
    {
        libvlc_rd_description_t * service = services[i];

        QString name = service->psz_name;

        // FIXME VLC 3.0.21: Sometimes the list contains duplicates.
        if (names.contains(name)) continue;

        names.append(name);

        libvlc_renderer_discoverer_t * discoverer
            = libvlc_renderer_discoverer_new(instance, name.C_STR);

        libvlc_event_manager_t * manager
            = libvlc_renderer_discoverer_event_manager(discoverer);

        libvlc_event_attach(manager, libvlc_RendererDiscovererItemAdded,
                            onRendererAdded, this);

        libvlc_event_attach(manager, libvlc_RendererDiscovererItemDeleted,
                            onRendererDeleted, this);

        discoverers.append(discoverer);

        libvlc_renderer_discoverer_start(discoverer);
    }

    libvlc_renderer_discoverer_list_release(services, count);
}

void WVlcEnginePrivate::clearDiscoverers()
{
    foreach (libvlc_renderer_discoverer_t * discoverer, discoverers)
    {
        // NOTE: This calls also stops the discoverer.
        libvlc_renderer_discoverer_release(discoverer);
    }

    discoverers.clear();
    renderers  .clear();

    outputs.clear();

    foreach (WVlcPlayerPrivate * player, players)
    {
        player->onOutputCleared();
    }

    players.clear();
}

//-------------------------------------------------------------------------------------------------
// Private static events
//-------------------------------------------------------------------------------------------------

/* static */ void WVlcEnginePrivate::onLog(void *, int, const libvlc_log_t *, const char * fmt,
                                                                              va_list      args)
{
    char message[1024];

    vsnprintf(message, sizeof(message), fmt, args);

    // NOTE: Calling qDebug manually on mobile because libVLC does not print anything on the
    //       console. The file logging is invoked from the message handler.
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    qDebug("[vlc] %s", message);
#else
    QString text = QString("[vlc] %1").arg(message);

    wControllerFile->writeLog(text);
#endif
}

/* static */ void WVlcEnginePrivate::onRendererAdded(const struct libvlc_event_t * event,
                                                     void                        * data)
{
    WVlcEnginePrivate * d = static_cast<WVlcEnginePrivate *> (data);

    libvlc_renderer_item_t * item = event->u.renderer_discoverer_item_added.item;

    d->renderers.append(item);

    QString string = libvlc_renderer_item_type(item);

    WAbstractBackend::OutputType type;

    if (string == "chromecast") type = WAbstractBackend::OutputChromecast;
    else                        type = WAbstractBackend::OutputUnknown;

    WBackendOutput output(libvlc_renderer_item_name(item), type);

    d->outputs.append(output);

    foreach (WVlcPlayerPrivate * player, d->players)
    {
        player->onOutputAdded(output);
    }
}

/* static */ void WVlcEnginePrivate::onRendererDeleted(const struct libvlc_event_t * event,
                                                       void                        * data)
{
    WVlcEnginePrivate * d = static_cast<WVlcEnginePrivate *> (data);

    libvlc_renderer_item_t * item = event->u.renderer_discoverer_item_deleted.item;

    for (int i = 0; i < d->renderers.count(); i++)
    {
        if (d->renderers.at(i) != item) continue;

        d->renderers.removeAt(i);
        d->outputs  .removeAt(i);

        foreach (WVlcPlayerPrivate * player, d->players)
        {
            player->onOutputRemoved(i);
        }

        return;
    }
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WVlcEngine::WVlcEngine(const QStringList & options,
                                      QThread * thread, QObject * parent)
    : QObject(parent), WPrivatable(new WVlcEnginePrivate(this))
{
    Q_D(WVlcEngine); d->init(options, thread);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcEngine::startLog()
{
    Q_D(WVlcEngine);

    if (d->thread == NULL)
    {
        d->startLog();

        return;
    }

    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcEnginePrivate::EventLog)));
}

/* Q_INVOKABLE */ void WVlcEngine::deleteInstance()
{
    Q_D(WVlcEngine);

    if (d->thread == NULL)
    {
        d->deleteInstance();

        return;
    }

    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcEnginePrivate::EventClear)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QStringList WVlcEngine::getOptions()
{
    QStringList options;

    options.append("--intf=dummy");
#ifdef Q_OS_WIN
    options.append("--dummy-quiet");
#elif defined(Q_OS_MACOS)
    options.append("--vout=macosx");
#endif
    options.append("--ignore-config");
    options.append("--no-spu");
    options.append("--no-osd");
    options.append("--no-stats");
    options.append("--no-media-library");
    options.append("--text-renderer=none");

    // NOTE VLC 3.0.20: This is required for the preferred-resolution to be applied.
    options.append("--adaptive-logic=highest");

    // FIXME VLC 3.0.18: Sometimes the end of the video is reached too soon.
    //options.append("--http-reconnect");

    // NOTE: This is useful for the mkv default language.
    //options.append("--audio-language=en");

    return options;
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
        d->create();

        return true;
    }
    else if (d->instance == NULL)
    {
        return QObject::event(event);
    }
    else if (type == static_cast<QEvent::Type> (WVlcEnginePrivate::EventLog))
    {
        d->startLog();

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcEnginePrivate::EventClear))
    {
        d->deleteInstance();

        return true;
    }
    else return QObject::event(event);
}

#endif // SK_NO_VLCENGINE
