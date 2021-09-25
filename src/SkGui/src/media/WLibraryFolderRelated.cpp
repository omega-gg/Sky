//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

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

    QString trackSource = track.source();

    if (trackSource.isEmpty()) return;

    if (d->currentItem && d->currentItem->label() == trackSource)
    {
        d->currentItem->reloadQuery();

        WPlaylist * playlist = d->currentItem->toPlaylist();

        if (playlist && playlist->containsSource(trackSource) == false)
        {
            playlist->addTrack(track);
        }

        return;
    }

    QString source = wControllerPlaylist->sourceRelatedTracks(trackSource);

    if (source.isEmpty())
    {
        source = track.feed();
    }

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

    playlist->setLabel(trackSource);

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
    WTrack track(data.value("source").toString());

    track.setType(static_cast<WTrack::Type> (data.value("type").toInt()));

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
