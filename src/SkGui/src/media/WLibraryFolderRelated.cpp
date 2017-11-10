//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WLibraryFolderRelated.h"

#ifndef SK_NO_LIBRARYFOLDERRELATED

// Sk includes
#include <WControllerNetwork>
#include <WControllerPlaylist>
#include <WBackendNet>
#include <WPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int LIBRARYFOLDERRELATED_MAX = 16;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WLibraryFolderRelatedPrivate::WLibraryFolderRelatedPrivate(WLibraryFolderRelated * p)
    : WLibraryFolderPrivate(p) {}

void WLibraryFolderRelatedPrivate::init()
{
    Q_Q(WLibraryFolderRelated);

    maxCount = LIBRARYFOLDERRELATED_MAX;

    QObject::connect(q, SIGNAL(currentIdChanged()), q, SIGNAL(playlistChanged()));
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WLibraryFolderRelated::WLibraryFolderRelated(WLibraryFolder * parent)
    : WLibraryFolder(new WLibraryFolderRelatedPrivate(this), FolderRelated, parent)
{
    Q_D(WLibraryFolderRelated); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolderRelated::loadTracks(const WTrack & track)
{
    Q_D(WLibraryFolderRelated);

    QUrl trackSource = track.source();

    QString label = trackSource.toString();

    if (label.isEmpty()) return;

    if (d->currentItem && d->currentItem->label() == label)
    {
        d->currentItem->reloadQuery();

        WPlaylist * playlist = d->currentItem->toPlaylist();

        if (playlist && playlist->containsSource(trackSource) == false)
        {
            playlist->addTrack(track);
        }

        return;
    }

    WBackendNet * backend = wControllerPlaylist->backendFromUrl(trackSource);

    QUrl source;

    if (backend != NULL)
    {
        QString id = backend->getTrackId(trackSource);

        if (id.isEmpty())
        {
            id = trackSource.toString();
        }

        source = WControllerPlaylist::createSource(backend->id(), "related", "tracks", id);
    }
    else source = track.feed();

    int index = currentIndex();

    if (index != -1)
    {
        while (index < d->items.count() - 1)
        {
            removeAt(index + 1);
        }
    }

    while (d->items.count() > (d->maxCount - 1))
    {
        removeAt(0);
    }

    WPlaylist * playlist = new WPlaylist;

    playlist->setLabel(label);

    playlist->setTitle(track.title());
    playlist->setCover(track.cover());

    playlist->loadSource(source);

    if (playlist->containsSource(trackSource) == false)
    {
        playlist->addTrack(track);
    }

    addLibraryItem(playlist);

    loadCurrentId(playlist->id(), true);

    playlist->tryDelete();
}

/* Q_INVOKABLE */ void WLibraryFolderRelated::loadTracks(const QVariantMap & data)
{
    WTrack track(WControllerNetwork::encodedUrl(data.value("source").toString()));

    WTrack::State state = static_cast<WTrack::State> (data.value("state").toInt());

    if (state == WTrack::Loading)
    {
         track.setState(WTrack::Default);
    }
    else track.setState(state);

    track.setTitle(data.value("title").toString());
    track.setCover(data.value("cover").toString());

    track.setAuthor(data.value("author").toString());
    track.setFeed  (data.value("feed")  .toString());

    track.setDuration(data.value("duration").toInt());

    track.setDate(data.value("date").toDateTime());

    track.setQuality(static_cast<WAbstractBackend::Quality> (data.value("quality").toInt()));

    loadTracks(track);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WLibraryFolderRelated::setPreviousPlaylist()
{
    if (hasPreviousPlaylist())
    {
        setCurrentIndex(currentIndex() - 1);
    }
}

/* Q_INVOKABLE */ void WLibraryFolderRelated::setNextPlaylist()
{
    if (hasNextPlaylist())
    {
        setCurrentIndex(currentIndex() + 1);
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WLibraryFolderRelated::hasPreviousPlaylist() const
{
    if (currentIndex() > 0)
    {
         return true;
    }
    else return false;
}

bool WLibraryFolderRelated::hasNextPlaylist() const
{
    int index = currentIndex();

    if (index != -1 && index < (count() - 1))
    {
         return true;
    }
    else return false;
}

#endif // SK_NO_LIBRARYFOLDERRELATED
