//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WAbstractPlaylist.h"

#ifndef SK_NO_ABSTRACTPLAYLIST

// Sk includes
#include <WControllerPlaylist>
#include <WPlaylistNet>
#include <WModelPlaylistNet>
#include <WLibraryFolder>

// Private includes
#include <private/WControllerPlaylist_p>
#include <private/WModelPlaylistNet_p>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int ABSTRACTPLAYLIST_MAX = 500;

//=================================================================================================
// WAbstractPlaylistWatcher
//=================================================================================================

/* virtual */ void WAbstractPlaylistWatcher::beginTracksInsert(int, int) {}
/* virtual */ void WAbstractPlaylistWatcher::endTracksInsert  ()         {}

/* virtual */ void WAbstractPlaylistWatcher::beginTracksMove(int, int, int) {}
/* virtual */ void WAbstractPlaylistWatcher::endTracksMove  ()              {}

/* virtual */ void WAbstractPlaylistWatcher::beginTracksRemove(int, int) {}
/* virtual */ void WAbstractPlaylistWatcher::endTracksRemove  ()         {}

/* virtual */ void WAbstractPlaylistWatcher::trackUpdated(int) {}

/* virtual */ void WAbstractPlaylistWatcher::beginTracksClear() {}
/* virtual */ void WAbstractPlaylistWatcher::endTracksClear  () {}

/* virtual */ void WAbstractPlaylistWatcher::selectedTracksChanged(const QList<int> &) {}

/* virtual */ void WAbstractPlaylistWatcher::currentIndexChanged(int) {}

/* virtual */ void WAbstractPlaylistWatcher::playlistDestroyed() {}

//=================================================================================================
// WAbstractPlaylistPrivate
//=================================================================================================

#include "WAbstractPlaylist_p.h"

WAbstractPlaylistPrivate::WAbstractPlaylistPrivate(WAbstractPlaylist * p)
    : WLibraryItemPrivate(p) {}

/* virtual */ WAbstractPlaylistPrivate::~WAbstractPlaylistPrivate()
{
    foreach (WAbstractPlaylistWatcher * watcher, watchers)
    {
        watcher->playlistDestroyed();
    }
}

//-------------------------------------------------------------------------------------------------

void WAbstractPlaylistPrivate::init()
{
    Q_Q(WAbstractPlaylist);

    currentTrack = NULL;

    currentIndex = -1;
    currentTime  = -1;

    scrollValue = 0;

    maxCount = ABSTRACTPLAYLIST_MAX;

    QObject::connect(q, SIGNAL(countChanged()), q, SIGNAL(playlistUpdated()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WAbstractPlaylistPrivate::setPrevious(bool cycle)
{
    if (currentIndex == -1) return;

    Q_Q(WAbstractPlaylist);

    int index = currentIndex;

    if (cycle)
    {
        if (index == 0)
        {
             index = q->count() - 1;
        }
        else index--;

        q->setCurrentIndex(index);
    }
    else if (hasPrevious(index))
    {
        q->setCurrentIndex(index - 1);
    }
}

void WAbstractPlaylistPrivate::setNext(bool cycle)
{
    if (currentIndex == -1) return;

    Q_Q(WAbstractPlaylist);

    int index = currentIndex;

    if (cycle)
    {
        if (index == q->count() - 1)
        {
             index = 0;
        }
        else index++;

        q->setCurrentIndex(index);
    }
    else if (hasNext(index))
    {
        q->setCurrentIndex(index + 1);
    }
}

//-------------------------------------------------------------------------------------------------

bool WAbstractPlaylistPrivate::hasPrevious(int index) const
{
    if (index > 0) return true;
    else           return false;
}

bool WAbstractPlaylistPrivate::hasNext(int index) const
{
    Q_Q(const WAbstractPlaylist);

    if (index >= 0 && index < (q->count() - 1))
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WAbstractPlaylistPrivate::insertSelected(const QList<int>     & indexes,
                                              const WAbstractTrack * track,
                                              int                    index)
{
    if (selectedTracks.contains(track)) return false;

    for (int i = 0; i < indexes.count(); i++)
    {
        if (indexes.at(i) > index)
        {
            selectedTracks.insert(i, track);

            return true;
        }
    }

    selectedTracks.append(track);

    return true;
}

//-------------------------------------------------------------------------------------------------

QList<int> WAbstractPlaylistPrivate::getSelected() const
{
    Q_Q(const WAbstractPlaylist);

    QList<int> selected;

    foreach (const WAbstractTrack * track, selectedTracks)
    {
        int index = q->indexOf(track);

        selected.append(index);
    }

    qSort(selected.begin(), selected.end());

    return selected;
}

//-------------------------------------------------------------------------------------------------

void WAbstractPlaylistPrivate::currentIndexChanged()
{
    foreach (WAbstractPlaylistWatcher * watcher, watchers)
    {
        watcher->currentIndexChanged(currentIndex);
    }
}

//-------------------------------------------------------------------------------------------------

void WAbstractPlaylistPrivate::emitSelectedTracksChanged(const QList<int> & indexes)
{
    Q_Q(WAbstractPlaylist);

    foreach (WAbstractPlaylistWatcher * watcher, watchers)
    {
        watcher->selectedTracksChanged(indexes);
    }

    emit q->selectedTracksChanged();
}

//=================================================================================================
// WAbstractPlaylist
//=================================================================================================
// Protected

WAbstractPlaylist::WAbstractPlaylist(WAbstractPlaylistPrivate * p, Type             type,
                                                                   WLibraryFolder * parent)
    : WLibraryItem(p, type, parent)
{
    Q_D(WAbstractPlaylist); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractPlaylist::setPreviousTrack(bool cycle)
{
    Q_D(WAbstractPlaylist);

    if (d->type == PlaylistFeed)
    {
         d->setNext(cycle);
    }
    else d->setPrevious(cycle);
}

/* Q_INVOKABLE */ void WAbstractPlaylist::setNextTrack(bool cycle)
{
    Q_D(WAbstractPlaylist);

    if (d->type == PlaylistFeed)
    {
         d->setPrevious(cycle);
    }
    else d->setNext(cycle);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WAbstractPlaylist::hasPreviousIndex(int index) const
{
    Q_D(const WAbstractPlaylist);

    if (d->type == PlaylistFeed)
    {
         return d->hasNext(index);
    }
    else return d->hasPrevious(index);
}

/* Q_INVOKABLE */ bool WAbstractPlaylist::hasNextIndex(int index) const
{
    Q_D(const WAbstractPlaylist);

    if (d->type == PlaylistFeed)
    {
         return d->hasPrevious(index);
    }
    else return d->hasNext(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractPlaylist::clearTracks()
{
    abortAll();

    if (count())
    {
        Q_D(WAbstractPlaylist);

        setCurrentId(-1);

        setScrollValue(0);

        d->selectedTracks.clear();

        foreach (WAbstractPlaylistWatcher * watcher, d->watchers)
        {
            watcher->beginTracksClear();
        }

        clearItems();

        foreach (WAbstractPlaylistWatcher * watcher, d->watchers)
        {
            watcher->endTracksClear();
        }

        emit tracksCleared();

        emit countChanged();
    }

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WAbstractPlaylist::checkFull(int count) const
{
    Q_D(const WAbstractPlaylist);

    return (this->count() + count >= d->maxCount);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractPlaylist::selectTrack(int index)
{
    if (index < 0 || index >= count()) return;

    Q_D(WAbstractPlaylist);

    const WAbstractTrack * track = itemAt(index);

    if (d->selectedTracks.contains(track)) return;

    d->selectedTracks.append(track);

    d->emitSelectedTracksChanged(QList<int>() << index);
}

/* Q_INVOKABLE */ void WAbstractPlaylist::selectTracks(int from, int to)
{
    if (from < 0 || from >= count() || to < 0 || to >= count()) return;

    Q_D(WAbstractPlaylist);

    QList<int> selected = d->getSelected();

    QList<int> changed;

    if (from < to)
    {
        for (int i = from; i <= to; i++)
        {
            const WAbstractTrack * track = itemAt(i);

            if (d->insertSelected(selected, track, i))
            {
                changed.append(i);
            }
        }
    }
    else
    {
        for (int i = from; i >= to; i--)
        {
            const WAbstractTrack * track = itemAt(i);

            if (d->insertSelected(selected, track, i))
            {
                changed.append(i);
            }
        }
    }

    if (changed.count())
    {
        qSort(changed.begin(), changed.end());

        d->emitSelectedTracksChanged(changed);
    }
}

/* Q_INVOKABLE */ void WAbstractPlaylist::selectAll()
{
    selectTracks(0, count() - 1);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractPlaylist::selectSingleTrack(int index)
{
    if (index < 0 || index >= count()) return;

    Q_D(WAbstractPlaylist);

    const WAbstractTrack * track = itemAt(index);

    if (d->selectedTracks.contains(track))
    {
        if (d->selectedTracks.count() == 1) return;

        d->selectedTracks.removeOne(track);
    }
    else d->selectedTracks.append(track);

    QList<int> selected = d->getSelected();

    d->selectedTracks.clear();

    d->selectedTracks.append(track);

    d->emitSelectedTracksChanged(selected);
}

/* Q_INVOKABLE */ void WAbstractPlaylist::selectCurrentTrack()
{
    Q_D(WAbstractPlaylist); selectSingleTrack(d->currentIndex);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractPlaylist::moveSelectedTo(int to)
{
    Q_D(WAbstractPlaylist);

    if (d->selectedTracks.isEmpty() || to < 0 || to > count()) return;

    QList<int> indexes = selectedTracks();

    foreach (const WAbstractTrack * track, d->selectedTracks)
    {
        int from = indexOf(track);

        if ((from > to && from != to)
            ||
            (from < to && from != (to - 1)))
        {
            beginTracksMove(from, from, to);

            if (from < to) to--;

            moveItemTo(from, to);

            endTracksMove();

            to++;
        }
        else if (from >= to) to++;
    }

    updateIndex();

    emit tracksMoved(indexes, to);

    emit playlistUpdated();

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractPlaylist::unselectTrack(int index)
{
    if (index < 0 || index >= count()) return;

    Q_D(WAbstractPlaylist);

    const WAbstractTrack * track = itemAt(index);

    if (d->selectedTracks.contains(track) == false) return;

    d->selectedTracks.removeOne(track);

    d->emitSelectedTracksChanged(QList<int>() << index);
}

/* Q_INVOKABLE */ void WAbstractPlaylist::unselectTracks()
{
    Q_D(WAbstractPlaylist);

    if (d->selectedTracks.isEmpty()) return;

    QList<int> selected = d->getSelected();

    d->selectedTracks.clear();

    d->emitSelectedTracksChanged(selected);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WAbstractPlaylist::indexSelected(int index) const
{
    if (index < 0 || index >= count()) return false;

    Q_D(const WAbstractPlaylist);

    const WAbstractTrack * track = itemAt(index);

    return (d->selectedTracks.contains(track) == true);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WAbstractPlaylist::closestSelected(int index) const
{
    Q_D(const WAbstractPlaylist);

    int closest = -1;

    QList<int> selected = d->getSelected();

    foreach (int selectedIndex, selected)
    {
        if (closest == -1 || (selectedIndex != index
                              &&
                              qAbs(index - selectedIndex) < qAbs(index - closest)))
        {
            closest = selectedIndex;
        }
    }

    return closest;
}

//-------------------------------------------------------------------------------------------------
// Pointers

/* Q_INVOKABLE */ const WAbstractTrack * WAbstractPlaylist::trackPointerFromId(int id) const
{
    return itemFromId(id);
}

/* Q_INVOKABLE */ const WAbstractTrack * WAbstractPlaylist::trackPointerAt(int index) const
{
    if (index < 0 || index >= count()) return NULL;

    return itemAt(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QList<const WAbstractTrack *> WAbstractPlaylist::trackPointers() const
{
    QList<const WAbstractTrack *> tracks;

    for (int i = 0; i < count(); i++)
    {
        tracks.append(trackPointerAt(i));
    }

    return tracks;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ const WAbstractTrack * WAbstractPlaylist::currentTrackPointer() const
{
    Q_D(const WAbstractPlaylist); return d->currentTrack;
}

/* Q_INVOKABLE */ void WAbstractPlaylist::setCurrentTrackPointer(const WAbstractTrack * track)
{
    setCurrentIndex(indexOf(track));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WAbstractPlaylist::containsTrackPointer(const WAbstractTrack * track) const
{
    return (indexOf(track) != -1);
}

//---------------------------------------------------------------------------------------------
// Watchers

/* Q_INVOKABLE */ void WAbstractPlaylist::registerWatcher(WAbstractPlaylistWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WAbstractPlaylist);

    if (d->watchers.contains(watcher)) return;

    d->watchers.append(watcher);
}

/* Q_INVOKABLE */ void WAbstractPlaylist::unregisterWatcher(WAbstractPlaylistWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WAbstractPlaylist);

    if (d->watchers.contains(watcher) == false) return;

    d->watchers.removeOne(watcher);
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WAbstractPlaylist::updateIndex()
{
    Q_D(WAbstractPlaylist);

    int index = indexOf(d->currentTrack);

    if (d->currentIndex != index)
    {
        d->currentIndex = index;

        d->currentIndexChanged();

        emit currentIndexChanged();
    }
}

//-------------------------------------------------------------------------------------------------

void WAbstractPlaylist::updateTrack(int index)
{
    Q_D(WAbstractPlaylist);

    foreach (WAbstractPlaylistWatcher * watcher, d->watchers)
    {
        watcher->trackUpdated(index);
    }

    emit trackUpdated(index);

    if (d->currentIndex == index)
    {
        emit currentTrackUpdated();
    }

    save();
}

void WAbstractPlaylist::updateTrack(const WAbstractTrack * track)
{
    updateTrack(indexOf(track));
}

//-------------------------------------------------------------------------------------------------

void WAbstractPlaylist::beginTracksInsert(int first, int last) const
{
    Q_D(const WAbstractPlaylist);

    foreach (WAbstractPlaylistWatcher * watcher, d->watchers)
    {
        watcher->beginTracksInsert(first, last);
    }
}

void WAbstractPlaylist::beginTracksMove(int first, int last, int to) const
{
    Q_D(const WAbstractPlaylist);

    foreach (WAbstractPlaylistWatcher * watcher, d->watchers)
    {
        watcher->beginTracksMove(first, last, to);
    }
}

void WAbstractPlaylist::beginTracksRemove(int first, int last) const
{
    Q_D(const WAbstractPlaylist);

    foreach (WAbstractPlaylistWatcher * watcher, d->watchers)
    {
        watcher->beginTracksRemove(first, last);
    }
}

//-------------------------------------------------------------------------------------------------

void WAbstractPlaylist::endTracksInsert() const
{
    Q_D(const WAbstractPlaylist);

    foreach (WAbstractPlaylistWatcher * watcher, d->watchers)
    {
        watcher->endTracksInsert();
    }
}

void WAbstractPlaylist::endTracksMove() const
{
    Q_D(const WAbstractPlaylist);

    foreach (WAbstractPlaylistWatcher * watcher, d->watchers)
    {
        watcher->endTracksMove();
    }
}

void WAbstractPlaylist::endTracksRemove() const
{
    Q_D(const WAbstractPlaylist);

    foreach (WAbstractPlaylistWatcher * watcher, d->watchers)
    {
        watcher->endTracksRemove();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected WLibraryItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractPlaylist::onApplyCurrentIds(const QList<int> & ids)
{
    if (ids.count() > 1)
    {
        qWarning("WAbstractPlaylist::onApplyCurrentIds: Item does not support multiple ids.");
    }

    Q_D(WAbstractPlaylist);

    unselectTracks();

    setCurrentId(ids.first());

    selectTrack(d->currentIndex);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WAbstractPlaylist::isPlaylistNet() const
{
    Q_D(const WAbstractPlaylist); return (d->type == PlaylistNet);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractPlaylist::isFeed() const
{
    Q_D(const WAbstractPlaylist); return (d->type == PlaylistFeed);
}

bool WAbstractPlaylist::isSearch() const
{
    Q_D(const WAbstractPlaylist); return (d->type == PlaylistSearch);
}

//-------------------------------------------------------------------------------------------------

int WAbstractPlaylist::currentId() const
{
    Q_D(const WAbstractPlaylist);

    if (d->currentTrack) return d->currentTrack->id();
    else                 return -1;
}

void WAbstractPlaylist::setCurrentId(int id)
{
    if (currentId() == id) return;

    Q_D(WAbstractPlaylist);

    const WAbstractTrack * track = itemFromId(id);

    if (d->currentTrack == track) return;

    d->currentTrack = track;
    d->currentIndex = indexOf(track);

    if (track == NULL)
    {
        setCurrentTime(-1);
    }

    d->currentIndexChanged();

    emit currentTrackChanged();
    emit currentIndexChanged();

    emit playlistUpdated();

    save();
}

//-------------------------------------------------------------------------------------------------

int WAbstractPlaylist::currentIndex() const
{
    Q_D(const WAbstractPlaylist); return d->currentIndex;
}

void WAbstractPlaylist::setCurrentIndex(int index)
{
    if (index < 0 || index >= count()) return;

    const WAbstractTrack * track = itemAt(index);

    setCurrentId(track->id());
}

//-------------------------------------------------------------------------------------------------

int WAbstractPlaylist::currentTime() const
{
    Q_D(const WAbstractPlaylist); return d->currentTime;
}

void WAbstractPlaylist::setCurrentTime(int msec)
{
    Q_D(WAbstractPlaylist);

    if (d->currentTime == msec) return;

    d->currentTime = msec;

    emit currentTimeChanged();

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WAbstractPlaylist::firstSelected() const
{
    Q_D(const WAbstractPlaylist);

    if (d->selectedTracks.isEmpty())
    {
         return -1;
    }
    else return indexOf(d->selectedTracks.first());
}

/* Q_INVOKABLE */ int WAbstractPlaylist::lastSelected() const
{
    Q_D(const WAbstractPlaylist);

    if (d->selectedTracks.isEmpty())
    {
         return -1;
    }
    else return indexOf(d->selectedTracks.last());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WAbstractPlaylist::selectedAligned() const
{
    Q_D(const WAbstractPlaylist);

    QList<int> selected = d->getSelected();

    if (selected.count() == 1) return true;

    int lastIndex = -1;

    foreach (int index, selected)
    {
        if (lastIndex != -1)
        {
            if (lastIndex != index) return false;

            lastIndex++;
        }
        else lastIndex = index + 1;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

int WAbstractPlaylist::count() const
{
    return itemCount();
}

//-------------------------------------------------------------------------------------------------

int WAbstractPlaylist::maxCount() const
{
    Q_D(const WAbstractPlaylist); return d->maxCount;
}

void WAbstractPlaylist::setMaxCount(int max)
{
    Q_D(WAbstractPlaylist);

    if (d->maxCount == max) return;

    d->maxCount = max;

    emit maxCountChanged();
}

//-------------------------------------------------------------------------------------------------

bool WAbstractPlaylist::isEmpty() const
{
    return (count() == 0);
}

bool WAbstractPlaylist::isFull() const
{
    Q_D(const WAbstractPlaylist);

    return (count() >= d->maxCount);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractPlaylist::hasPreviousTrack() const
{
    Q_D(const WAbstractPlaylist);

    return hasPreviousIndex(d->currentIndex);
}

bool WAbstractPlaylist::hasNextTrack() const
{
    Q_D(const WAbstractPlaylist);

    return hasNextIndex(d->currentIndex);
}

//-------------------------------------------------------------------------------------------------

QList<int> WAbstractPlaylist::selectedTracks() const
{
    Q_D(const WAbstractPlaylist); return d->getSelected();
}

void WAbstractPlaylist::setSelectedTracks(const QList<int> & indexes)
{
    if (indexes.isEmpty()) return;

    Q_D(WAbstractPlaylist);

    QList<int> selected = d->getSelected();

    QList<int> changed;

    for (int i = 0; i < indexes.count(); i++)
    {
        const WAbstractTrack * track = itemAt(i);

        if (d->insertSelected(selected, track, i))
        {
            changed.append(i);
        }
    }

    if (changed.count())
    {
        qSort(changed.begin(), changed.end());

        d->emitSelectedTracksChanged(changed);
    }
}

//-------------------------------------------------------------------------------------------------

int WAbstractPlaylist::selectedCount() const
{
    Q_D(const WAbstractPlaylist); return d->selectedTracks.count();
}

QString WAbstractPlaylist::selectedSources() const
{
    Q_D(const WAbstractPlaylist);

    QString sources;

    foreach (const WAbstractTrack * track, d->selectedTracks)
    {
        sources.append(track->source().toString() + '\n');
    }

    return sources;
}

//-------------------------------------------------------------------------------------------------

qreal WAbstractPlaylist::scrollValue() const
{
    Q_D(const WAbstractPlaylist); return d->scrollValue;
}

void WAbstractPlaylist::setScrollValue(qreal value)
{
    Q_D(WAbstractPlaylist);

    if (d->scrollValue == value) return;

    d->scrollValue = value;

    emit scrollValueChanged();

    save();
}

//-------------------------------------------------------------------------------------------------

QString WAbstractPlaylist::currentTitle() const
{
    const WAbstractTrack * track = currentTrackPointer();

    if (track) return track->title();
    else       return QString();
}

QUrl WAbstractPlaylist::currentCover() const
{
    const WAbstractTrack * track = currentTrackPointer();

    if (track) return track->cover();
    else       return QUrl();
}

int WAbstractPlaylist::currentDuration() const
{
    const WAbstractTrack * track = currentTrackPointer();

    if (track) return track->duration();
    else       return -1;
}

#endif // SK_NO_ABSTRACTPLAYLIST
