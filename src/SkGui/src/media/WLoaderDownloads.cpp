//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

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

#include "WLoaderDownloads.h"

#ifndef SK_NO_LOADERDOWNLOADS

// Qt includes
#include <QDir>

// Sk includes
#include <WControllerFile>
#include <WControllerPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int LOADERDOWNLOADS_MAX_COUNT = 500;

//=================================================================================================
// WLoaderDownloadsReply
//=================================================================================================

class WLoaderDownloadsReply : public WLoaderPlaylistReply
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extract(const QString & path, int maximum);
};

/* Q_INVOKABLE */ void WLoaderDownloadsReply::extract(const QString & path, int maximum)
{
    WLoaderPlaylistData data;

    // NOTE: An empty path would list the current folder.
    if (path.isEmpty())
    {
        emit loaded(data);

        return;
    }

    QStringList & sources = data.sources;

    // NOTE: The VBML is written once its media is complete, so its date is the download date.
    QFileInfoList list = QDir(path).entryInfoList(QStringList("*.vbml"), QDir::Files,
                                                  QDir::Time);

    foreach (const QFileInfo & info, list)
    {
        sources.append(WControllerFile::fileUrl(info.absoluteFilePath()));

        if (sources.count() == maximum) break;
    }

    emit loaded(data);
}

//=================================================================================================
// WLoaderDownloadsPrivate
//=================================================================================================

WLoaderDownloadsPrivate::WLoaderDownloadsPrivate(WLoaderDownloads * p)
    : WLoaderPlaylistPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WLoaderDownloadsPrivate::init()
{
    path = WControllerFile::pathDownloads();

    reply = NULL;

    const QMetaObject * meta = &(WLoaderDownloadsReply::staticMetaObject);

    method = meta->method(meta->indexOfMethod("extract(QString,int)"));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WLoaderDownloadsPrivate::updateSources()
{
    Q_Q(WLoaderDownloads);

    q->setQueryLoading(true);

    if (reply) reply->abortAndDelete(q);

    reply = new WLoaderDownloadsReply;

    QObject::connect(reply, SIGNAL(loaded(const WLoaderPlaylistData &)),
                     q,     SLOT(onLoaded(const WLoaderPlaylistData &)));

    reply->moveToThread(wControllerPlaylist->thread());

    method.invoke(reply, Q_ARG(const QString &, path), Q_ARG(int, LOADERDOWNLOADS_MAX_COUNT));
}

//-------------------------------------------------------------------------------------------------

void WLoaderDownloadsPrivate::clearQueries()
{
    Q_Q(WLoaderDownloads);

    if (reply)
    {
        reply->abortAndDelete(q);

        reply = NULL;
    }

    q->clearQueries();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WLoaderDownloadsPrivate::onLoaded(const WLoaderPlaylistData & data)
{
    Q_Q(WLoaderDownloads);

    reply->deleteLater();

    reply = NULL;

    // NOTE: The tracks we have are kept, the new ones are loaded from their VBML.
    q->applySources(data.sources, QHash<QString, const WTrack *>(), true);

    q->setQueryLoading(false);
}

//=================================================================================================
// WLoaderDownloads
//=================================================================================================

/* explicit */ WLoaderDownloads::WLoaderDownloads(WLibraryFolder * folder, int id)
    : WLoaderPlaylist(new WLoaderDownloadsPrivate(this), folder, id)
{
    Q_D(WLoaderDownloads); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected WLoaderPlaylist implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WLoaderDownloads::onStart()
{
    Q_D(WLoaderDownloads); d->updateSources();
}

/* virtual */ void WLoaderDownloads::onStop()
{
    Q_D(WLoaderDownloads); d->clearQueries();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WLoaderDownloads::path() const
{
    Q_D(const WLoaderDownloads); return d->path;
}

void WLoaderDownloads::setPath(const QString & path)
{
    Q_D(WLoaderDownloads);

    if (d->path == path) return;

    d->path = path;

    if (d->active) d->updateSources();

    emit pathChanged();
}

#endif // SK_NO_LOADERDOWNLOADS

#include "WLoaderDownloads.moc"
