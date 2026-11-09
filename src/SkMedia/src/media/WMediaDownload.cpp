//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

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

#include "WMediaDownload.h"

#ifndef SK_NO_MEDIADOWNLOAD

// Qt includes
#include <QFile>
#include <QDateTime>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerMedia>
#include <WFfmpegDownload>

//=================================================================================================
// WAbstractDownloadHookPrivate
//=================================================================================================

WAbstractDownloadHookPrivate::WAbstractDownloadHookPrivate(WAbstractDownloadHook * p)
    : WPrivate(p) {}

//=================================================================================================
// WAbstractDownloadHook
//=================================================================================================
// Protected

/* explicit */ WAbstractDownloadHook::WAbstractDownloadHook(QObject * parent)
    : QObject(parent), WPrivatable(new WAbstractDownloadHookPrivate(this)) {}

WAbstractDownloadHook::WAbstractDownloadHook(WAbstractDownloadHookPrivate * p, QObject * parent)
    : QObject(parent), WPrivatable(p) {}

//=================================================================================================
// WMediaDownloadPrivate
//=================================================================================================

WMediaDownloadPrivate::WMediaDownloadPrivate(WMediaDownload * p) : WPrivate(p) {}

void WMediaDownloadPrivate::init(const QVariantMap & track,
                                 const QString     & path, const QStringList & options)
{
    this->track   = track;
    this->path    = path;
    this->options = options;

    thread = NULL;

    hook = NULL;

    started = false;
    running = true;
    error   = false;

    progress = 0;

    received =  0;
    total    = -1;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WMediaDownloadPrivate::start(const QStringList & urls, bool withProgress)
{
    Q_Q(WMediaDownload);

    started = true;

    // NOTE: The thread belongs to the controller, so it can remove its files after an abort.
    thread = new WFfmpegDownload(urls, path, options, wControllerMedia);

    const WControllerMediaPrivate * p = wControllerMedia->d_func();

    if (p->downloadProxy.isEmpty() == false)
    {
        thread->setProxy(p->downloadProxy, p->downloadPassword);
    }

    if (withProgress)
    {
        QObject::connect(thread, SIGNAL(progress(int, qint64, qint64)),
                         q,      SLOT(applyProgress(int, qint64, qint64)));
    }

    QObject::connect(thread, SIGNAL(finished()), q, SLOT(onFinished()));

    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void WMediaDownloadPrivate::finish(bool ok)
{
    Q_Q(WMediaDownload);

    running = false;
    error   = (ok == false);

    W_GET_CONTROLLER(WControllerMedia, controller);

    if (controller) controller->d_func()->downloads.removeOne(q);

    if (hook == NULL) return;

    // NOTE: This releases what the hook created, like a torrent in its cache.
    hook->clear(q);

    hook = NULL;
}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ void WMediaDownloadPrivate::addValue(QString & vbml, const QString & key,
                                                                  const QString & value)
{
    QString string = value.simplified();

    // NOTE: A value starting with one of these is read as a structure, unless it's in a block.
    if (string.startsWith('#') || string.startsWith('|') || string.startsWith('>'))
    {
        string.prepend("|\n    ");
    }

    Sk::bmlPair(vbml, key, string, "\n\n");
}

/* static */ QString WMediaDownloadPrivate::getVbml(const QVariantMap & track,
                                                    const QString     & fileName)
{
    QString vbml = WControllerPlaylist::vbml();

    Sk::bmlPair(vbml, "type", "track", "\n\n");

    // NOTE: The file is next to the VBML.
    addValue(vbml, "source", fileName.mid(fileName.lastIndexOf('/') + 1));

    addValue(vbml, "title", track.value("title").toString());
    addValue(vbml, "cover", track.value("cover").toString());

    addValue(vbml, "author", track.value("author").toString());
    addValue(vbml, "feed",   track.value("feed")  .toString());

    QDateTime date = track.value("date").toDateTime();

    if (date.isValid())
    {
        Sk::bmlPair(vbml, "date", Sk::bmlDate(date), "\n\n");
    }

    // NOTE: We clear the last '\n'.
    vbml.chop(1);

    return vbml;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WMediaDownloadPrivate::onFinished()
{
    Q_Q(WMediaDownload);

    fileName = thread->fileName();

    thread = NULL;

    if (fileName.isEmpty())
    {
        finish(false);

        emit q->ended();

        return;
    }

    QString fileVbml = WControllerNetwork::removeFileExtension(fileName) + ".vbml";

    source = WControllerFile::fileUrl(fileVbml);

    received = QFile(fileName).size();

    QByteArray vbml = getVbml(track, fileName).toUtf8();

    WControllerFileReply * reply = wControllerFile->startWriteFile(fileVbml, vbml);

    QObject::connect(reply, SIGNAL(complete(bool)), q, SLOT(onWritten(bool)));
}

void WMediaDownloadPrivate::onWritten(bool ok)
{
    Q_Q(WMediaDownload);

    // NOTE: The download was aborted during the write.
    if (running == false) return;

    if (ok)
    {
        progress = 100;

        emit q->progressChanged();
    }
    else source = QString();

    finish(ok);

    emit q->ended();
}

//=================================================================================================
// WMediaDownload
//=================================================================================================
// Private

WMediaDownload::WMediaDownload(const QVariantMap & track,
                               const QString     & path,
                               const QStringList & options, QObject * parent)
    : QObject(parent), WPrivatable(new WMediaDownloadPrivate(this))
{
    Q_D(WMediaDownload); d->init(track, path, options);
}

//-------------------------------------------------------------------------------------------------
// Public

/* virtual */ WMediaDownload::~WMediaDownload() { abort(); }

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WMediaDownload::abort()
{
    Q_D(WMediaDownload);

    if (d->running == false) return;

    // NOTE: The thread removes its files and deletes itself, we don't wait for it.
    if (d->thread)
    {
        disconnect(d->thread, NULL, this, NULL);

        d->thread->abort();
    }
    // NOTE: The VBML is being written, the deletion comes after it on the same thread.
    else if (d->fileName.isEmpty() == false)
    {
        wControllerFile->startDeleteFiles(QStringList() << d->fileName
                                                        << WControllerFile::filePath(d->source));

        d->source = QString();
    }

    d->finish(false);
}

/* Q_INVOKABLE */ void WMediaDownload::remove()
{
    abort();

    deleteLater();
}

/* Q_INVOKABLE */ void WMediaDownload::applyFile(const QString & fileName)
{
    Q_D(WMediaDownload);

    // NOTE: The hook can fail after we started, when its cache is cleared during the copy.
    if (d->running == false || d->started) return;

    if (fileName.isEmpty())
    {
        d->finish(false);

        emit ended();
    }
    // NOTE: The progress is the hook's, the copy that follows is short.
    else d->start(QStringList(fileName), false);
}

/* Q_INVOKABLE */ void WMediaDownload::applyProgress(int percent, qint64 received, qint64 total)
{
    Q_D(WMediaDownload);

    if (d->running == false) return;

    d->progress = percent;

    d->received = received;
    d->total    = total;

    emit progressChanged();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WMediaDownload::isRunning() const
{
    Q_D(const WMediaDownload); return d->running;
}

bool WMediaDownload::hasError() const
{
    Q_D(const WMediaDownload); return d->error;
}

QString WMediaDownload::source() const
{
    Q_D(const WMediaDownload); return d->source;
}

int WMediaDownload::progress() const
{
    Q_D(const WMediaDownload); return d->progress;
}

qint64 WMediaDownload::received() const
{
    Q_D(const WMediaDownload); return d->received;
}

qint64 WMediaDownload::total() const
{
    Q_D(const WMediaDownload); return d->total;
}

#endif // SK_NO_MEDIADOWNLOAD
