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

#include "WBookmarkTrack.h"

#ifndef SK_NO_BOOKMARKTRACK

// Sk includes
#include <WTrack>
#include <WTabTrack>

// Private includes
#include <private/WTabTrack_p>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WBookmarkTrack_p.h"

WBookmarkTrackPrivate::WBookmarkTrackPrivate(WBookmarkTrack * p) : WPrivate(p) {}

/* virtual */ WBookmarkTrackPrivate::~WBookmarkTrackPrivate()
{
    if (playlist)
    {
        Q_Q(WBookmarkTrack);

        playlist->unregisterWatcher(q);
    }
}

//-------------------------------------------------------------------------------------------------

void WBookmarkTrackPrivate::init()
{
    id = -1;

    parentTab = NULL;

    playlist = NULL;
    track    = NULL;

    idTrack = -1;

    state = WTrack::Default;

    duration = -1;

    quality = WAbstractBackend::QualityInvalid;

    currentTime = -1;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBookmarkTrackPrivate::setParentTab(WTabTrack * parent)
{
    if (parentTab)
    {
        qWarning("WBookmarkTrackPrivate::setParentTab: Item already has a parent.");

        return;
    }

    parentTab = parent;
}

//-------------------------------------------------------------------------------------------------

void WBookmarkTrackPrivate::setPlaylist(WPlaylist * playlist)
{
    if (this->playlist == playlist) return;

    Q_Q(WBookmarkTrack);

    if (this->playlist)
    {
        this->playlist->unregisterWatcher(q);
    }

    this->playlist = playlist;

    if (playlist)
    {
        updateTrackPointer();

        playlist->registerWatcher(q);
    }

    if (q->isCurrent())
    {
        parentTab->d_func()->updatePlaylist(q);
    }
}

void WBookmarkTrackPrivate::clearPlaylist()
{
    Q_Q(WBookmarkTrack);

    playlist->unregisterWatcher(q);

    if (q->isCurrent())
    {
        parentTab->d_func()->setPlaylist(NULL);
    }

    clearTrack();

    q->save();
}

//-------------------------------------------------------------------------------------------------

void WBookmarkTrackPrivate::setTrack(const WTrack & track)
{
    this->track = &track;

    idTrack = track.id();

    setPlaylist(track.playlist());

    if (playlist)
    {
        idPlaylist = playlist->idFull();
    }

    state = track.state();

    source = track.source();

    title = track.title();
    cover = track.cover();

    author = track.author();
    feed   = track.feed  ();

    duration = track.duration();

    date = track.date();

    quality = track.quality();

    videoShot = QUrl();

    currentTime = -1;
}

void WBookmarkTrackPrivate::clearTrack()
{
    playlist = NULL;
    track    = NULL;

    idPlaylist = QList<int>();
    idTrack    = -1;

    emitUpdated();
}

//-------------------------------------------------------------------------------------------------

void WBookmarkTrackPrivate::updateTrackPointer()
{
    const WTrack * track = playlist->trackPointerFromId(idTrack);

    if (this->track == track) return;

    this->track = track;
}

//-------------------------------------------------------------------------------------------------

void WBookmarkTrackPrivate::emitUpdated()
{
    Q_Q(WBookmarkTrack);

    if (q->isCurrent())
    {
        emit parentTab->currentBookmarkUpdated();
    }
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WBookmarkTrack::WBookmarkTrack(const WTrack & track)
    : WPrivatable(new WBookmarkTrackPrivate(this))
{
    Q_D(WBookmarkTrack);

    d->init();

    d->setTrack(track);
}

WBookmarkTrack::WBookmarkTrack() : WPrivatable(new WBookmarkTrackPrivate(this))
{
    Q_D(WBookmarkTrack); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WBookmarkTrack::setTrack(const WTrack & track)
{
    Q_D(WBookmarkTrack);

    if (d->track == &track) return;

    d->setTrack(track);

    save();
}

//-------------------------------------------------------------------------------------------------

WTrack WBookmarkTrack::toTrack() const
{
    Q_D(const WBookmarkTrack);

    WTrack track(d->source);

    track.setState(d->state);

    track.setTitle(d->title);
    track.setCover(d->cover);

    track.setAuthor(d->author);
    track.setFeed  (d->feed);

    track.setDuration(d->duration);

    track.setDate(d->date);

    track.setQuality(d->quality);

    return track;
}

QVariantMap WBookmarkTrack::toTrackData() const
{
    return toTrack().toMap();
}

//-------------------------------------------------------------------------------------------------

void WBookmarkTrack::save()
{
    Q_D(WBookmarkTrack);

    if (d->parentTab) d->parentTab->save();
}

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

WBookmarkTrack::WBookmarkTrack(const WBookmarkTrack & other)
    : WPrivatable(new WBookmarkTrackPrivate(this))
{
    Q_D(WBookmarkTrack);

    d->init();

    *this = other;
}

//-------------------------------------------------------------------------------------------------

bool WBookmarkTrack::operator==(const WBookmarkTrack & other) const
{
    Q_D(const WBookmarkTrack);

    const WBookmarkTrackPrivate * op = other.d_func();

    if (d->id == op->id &&

        d->parentTab == op->parentTab &&

        d->playlist == op->playlist &&
        d->track    == op->track    &&

        d->idPlaylist == op->idPlaylist &&
        d->idTrack    == op->idTrack    &&

        d->state == op->state &&

        d->source == op->source &&

        d->title == op->title &&
        d->cover == op->cover &&

        d->author == op->author &&
        d->feed   == op->feed   &&

        d->duration == op->duration &&

        d->date == op->date &&

        d->quality == op->quality &&

        d->videoShot   == op->videoShot &&
        d->currentTime == op->currentTime)
    {
         return true;
    }
    else return false;
}

WBookmarkTrack & WBookmarkTrack::operator=(const WBookmarkTrack & other)
{
    Q_D(WBookmarkTrack);

    const WBookmarkTrackPrivate * op = other.d_func();

    d->id = op->id;

    d->parentTab = op->parentTab;

    d->playlist = op->playlist;

    if (d->playlist) d->playlist->registerWatcher(this);

    d->track = op->track;

    d->idPlaylist = op->idPlaylist;
    d->idTrack    = op->idTrack;

    d->state = op->state;

    d->source = op->source;

    d->title = op->title;
    d->cover = op->cover;

    d->author = op->author;
    d->feed   = op->feed;

    d->duration = op->duration;

    d->date = op->date;

    d->quality = op->quality;

    d->videoShot   = op->videoShot;
    d->currentTime = op->currentTime;

    d->emitUpdated();

    save();

    return *this;
}

//-------------------------------------------------------------------------------------------------
// Protected WPlaylistWatcher reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WBookmarkTrack::endTracksInsert()
{
    Q_D(WBookmarkTrack);

    if (d->idTrack == -1 || d->track) return;

    d->updateTrackPointer();

    if (d->track == NULL) return;

    if (isCurrent())
    {
        d->parentTab->d_func()->updatePlaylist(this);
    }

    d->emitUpdated();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBookmarkTrack::beginTracksRemove(int first, int last)
{
    Q_D(WBookmarkTrack);

    int index = d->playlist->indexFromId(d->idTrack);

    if (index < first || index > last) return;

    d->clearPlaylist();
}

/* virtual */ void WBookmarkTrack::beginTracksClear()
{
    Q_D(WBookmarkTrack); d->clearPlaylist();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBookmarkTrack::trackUpdated(int index)
{
    Q_D(WBookmarkTrack);

    const WTrack * track = d->playlist->trackPointerAt(index);

    if (d->track != track) return;

    d->state = track->state();

    d->source = track->source();

    d->title = track->title();
    d->cover = track->cover();

    d->author = track->author();
    d->feed   = track->feed  ();

    d->duration = track->duration();

    d->date = track->date();

    d->quality = track->quality();

    d->emitUpdated();

    save();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WBookmarkTrack::playlistDestroyed()
{
    Q_D(WBookmarkTrack);

    d->playlist = NULL;
    d->track    = NULL;

    d->emitUpdated();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

int WBookmarkTrack::id() const
{
    Q_D(const WBookmarkTrack); return d->id;
}

//-------------------------------------------------------------------------------------------------

bool WBookmarkTrack::isDefault() const
{
    Q_D(const WBookmarkTrack); return (d->state == WTrack::Default);
}

bool WBookmarkTrack::isLoading() const
{
    Q_D(const WBookmarkTrack); return (d->state == WTrack::Loading);
}

bool WBookmarkTrack::isLoaded() const
{
    Q_D(const WBookmarkTrack); return (d->state == WTrack::Loaded);
}

//-------------------------------------------------------------------------------------------------

bool WBookmarkTrack::isValid() const
{
    Q_D(const WBookmarkTrack); return d->source.isValid();
}

//-------------------------------------------------------------------------------------------------

bool WBookmarkTrack::isCurrent() const
{
    Q_D(const WBookmarkTrack);

    return (d->parentTab && d->parentTab->currentBookmark() == this);
}

//-------------------------------------------------------------------------------------------------

WTabTrack * WBookmarkTrack::parentTab() const
{
    Q_D(const WBookmarkTrack); return d->parentTab;
}

//-------------------------------------------------------------------------------------------------

QList<int> WBookmarkTrack::folderIds() const
{
    Q_D(const WBookmarkTrack);

    QList<int> idFull = d->idPlaylist;

    if (idFull.count())
    {
        idFull.removeFirst();
    }

    idFull.append(d->idTrack);

    return idFull;
}

//-------------------------------------------------------------------------------------------------

WPlaylist * WBookmarkTrack::playlist() const
{
    Q_D(const WBookmarkTrack); return d->playlist;
}

const WTrack * WBookmarkTrack::track() const
{
    Q_D(const WBookmarkTrack); return d->track;
}

//-------------------------------------------------------------------------------------------------

QList<int> WBookmarkTrack::idPlaylist() const
{
    Q_D(const WBookmarkTrack); return d->idPlaylist;
}

int WBookmarkTrack::idTrack() const
{
    Q_D(const WBookmarkTrack); return d->idTrack;
}

int WBookmarkTrack::idFolderRoot() const
{
    Q_D(const WBookmarkTrack);

    if (d->idPlaylist.isEmpty())
    {
         return -1;
    }
    else return d->idPlaylist.first();
}

//-------------------------------------------------------------------------------------------------

WTrack::State WBookmarkTrack::state() const
{
    Q_D(const WBookmarkTrack); return d->state;
}

//-------------------------------------------------------------------------------------------------

QUrl WBookmarkTrack::source() const
{
    Q_D(const WBookmarkTrack); return d->source;
}

//-------------------------------------------------------------------------------------------------

QString WBookmarkTrack::title() const
{
    Q_D(const WBookmarkTrack); return d->title;
}

QUrl WBookmarkTrack::cover() const
{
    Q_D(const WBookmarkTrack); return d->cover;
}

//-------------------------------------------------------------------------------------------------

QString WBookmarkTrack::author() const
{
    Q_D(const WBookmarkTrack); return d->author;
}

QString WBookmarkTrack::feed() const
{
    Q_D(const WBookmarkTrack); return d->feed;
}

//-------------------------------------------------------------------------------------------------

QDateTime WBookmarkTrack::date() const
{
    Q_D(const WBookmarkTrack); return d->date;
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Quality WBookmarkTrack::quality() const
{
    Q_D(const WBookmarkTrack); return d->quality;
}

//-------------------------------------------------------------------------------------------------

int WBookmarkTrack::duration() const
{
    Q_D(const WBookmarkTrack); return d->duration;
}

//-------------------------------------------------------------------------------------------------

QUrl WBookmarkTrack::videoShot() const
{
    Q_D(const WBookmarkTrack); return d->videoShot;
}

//-------------------------------------------------------------------------------------------------

int WBookmarkTrack::currentTime() const
{
    Q_D(const WBookmarkTrack); return d->currentTime;
}

#endif // SK_NO_BOOKMARKTRACK
