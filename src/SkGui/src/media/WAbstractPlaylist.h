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

#ifndef WABSTRACTPLAYLIST_H
#define WABSTRACTPLAYLIST_H

// Sk includes
#include <WLibraryItem>

#ifndef SK_NO_ABSTRACTPLAYLIST

//-------------------------------------------------------------------------------------------------
// WAbstractPlaylistWatcher
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WAbstractPlaylistWatcher
{
protected:
    virtual void beginTracksInsert(int first, int last);
    virtual void endTracksInsert  ();

    virtual void beginTracksMove(int first, int last, int to);
    virtual void endTracksMove  ();

    virtual void beginTracksRemove(int first, int last);
    virtual void endTracksRemove  ();

    virtual void trackUpdated(int index);

    virtual void beginTracksClear();
    virtual void endTracksClear  ();

    virtual void selectedTracksChanged(const QList<int> & updatedIndexes);

    virtual void currentIndexChanged(int index);

    virtual void playlistDestroyed();

private:
    friend class WAbstractPlaylist;
    friend class WAbstractPlaylistPrivate;
    friend class WPlaylistNet;
    friend class WPlaylistNetPrivate;
};

//-------------------------------------------------------------------------------------------------
// WAbstractPlaylist
//-------------------------------------------------------------------------------------------------

// Forward declarations
class WAbstractPlaylistPrivate;
class WPlaylistNet;
class WAbstractTrack;

class SK_GUI_EXPORT WAbstractPlaylist : public WLibraryItem
{
    Q_OBJECT

    Q_PROPERTY(bool isPlaylistNet READ isPlaylistNet CONSTANT)

    Q_PROPERTY(bool isFeed   READ isFeed   CONSTANT)
    Q_PROPERTY(bool isSearch READ isSearch CONSTANT)

    Q_PROPERTY(int currentId READ currentId WRITE setCurrentId NOTIFY currentTrackChanged)

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex
               NOTIFY currentIndexChanged)

    Q_PROPERTY(int currentTime READ currentTime WRITE setCurrentTime
               NOTIFY currentTimeChanged)

    Q_PROPERTY(int firstSelected READ firstSelected NOTIFY selectedTracksChanged)
    Q_PROPERTY(int lastSelected  READ lastSelected  NOTIFY selectedTracksChanged)

    Q_PROPERTY(bool selectedAligned READ selectedAligned NOTIFY selectedTracksChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount NOTIFY maxCountChanged)

    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY countChanged)
    Q_PROPERTY(bool isFull  READ isFull  NOTIFY countChanged)

    Q_PROPERTY(bool hasPreviousTrack READ hasPreviousTrack NOTIFY playlistUpdated)
    Q_PROPERTY(bool hasNextTrack     READ hasNextTrack     NOTIFY playlistUpdated)

    Q_PROPERTY(QList<int> selectedTracks READ selectedTracks WRITE setSelectedTracks
               NOTIFY selectedTracksChanged)

    Q_PROPERTY(int     selectedCount   READ selectedCount   NOTIFY selectedTracksChanged)
    Q_PROPERTY(QString selectedSources READ selectedSources NOTIFY selectedTracksChanged)

    Q_PROPERTY(qreal scrollValue READ scrollValue WRITE setScrollValue NOTIFY scrollValueChanged)

    Q_PROPERTY(QString currentTitle    READ currentTitle    NOTIFY currentTrackChanged)
    Q_PROPERTY(QUrl    currentCover    READ currentCover    NOTIFY currentTrackChanged)
    Q_PROPERTY(int     currentDuration READ currentDuration NOTIFY currentTrackChanged)

protected:
    WAbstractPlaylist(WAbstractPlaylistPrivate * p, Type type, WLibraryFolder * parent = NULL);

public: // Interface
    Q_INVOKABLE void setPreviousTrack(bool cycle = false);
    Q_INVOKABLE void setNextTrack    (bool cycle = false);

    Q_INVOKABLE bool hasPreviousIndex(int index) const;
    Q_INVOKABLE bool hasNextIndex    (int index) const;

    Q_INVOKABLE void clearTracks();

    Q_INVOKABLE bool checkFull(int count = 1) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void selectTrack (int index);
    Q_INVOKABLE void selectTracks(int from, int to);
    Q_INVOKABLE void selectAll   ();

    Q_INVOKABLE void selectSingleTrack (int index);
    Q_INVOKABLE void selectCurrentTrack();

    Q_INVOKABLE void moveSelectedTo(int index);

    Q_INVOKABLE void unselectTrack (int index);
    Q_INVOKABLE void unselectTracks();

    Q_INVOKABLE bool indexSelected(int index) const;

    Q_INVOKABLE int closestSelected(int index) const;

    //---------------------------------------------------------------------------------------------
    // Pointers

    Q_INVOKABLE const WAbstractTrack * trackPointerFromId(int id)    const;
    Q_INVOKABLE const WAbstractTrack * trackPointerAt    (int index) const;

    Q_INVOKABLE QList<const WAbstractTrack *> trackPointers() const;

    Q_INVOKABLE const WAbstractTrack * currentTrackPointer() const;
    Q_INVOKABLE void                   setCurrentTrackPointer(const WAbstractTrack * track);

    Q_INVOKABLE bool containsTrackPointer(const WAbstractTrack * track) const;

    //---------------------------------------------------------------------------------------------
    // Watchers

    Q_INVOKABLE void registerWatcher  (WAbstractPlaylistWatcher * watcher);
    Q_INVOKABLE void unregisterWatcher(WAbstractPlaylistWatcher * watcher);

public: // Abstract interface
    virtual int indexOf(const WAbstractTrack * track) const = 0;

    virtual int indexFromId(int id) const = 0;

protected: // Functions
    void updateIndex();

    void updateTrack(int index);

    void updateTrack(const WAbstractTrack * track);

    void beginTracksInsert(int first, int last)         const;
    void beginTracksMove  (int first, int last, int to) const;
    void beginTracksRemove(int first, int last)         const;

    void endTracksInsert() const;
    void endTracksMove  () const;
    void endTracksRemove() const;

protected: // Abstract functions
    virtual int itemCount() const = 0;

    virtual const WAbstractTrack * itemFromId(int id)    const = 0;
    virtual const WAbstractTrack * itemAt    (int index) const = 0;

    virtual void moveItemTo(int from, int to) = 0;

    virtual void clearItems() = 0;

protected: // WLibraryItem reimplementation
    /* virtual */ void onApplyCurrentIds(const QList<int> & ids);

signals:
    void tracksInserted(int index, int count);

    void tracksMoved(const QList<int> & indexes, int to);

    void tracksRemoved(const QList<int> & indexes);

    void trackUpdated(int index);

    void tracksCleared();

    void trackQueryEnded();

    void countChanged();

    void maxCountChanged();

    void playlistUpdated();

    void currentTrackChanged();
    void currentTrackUpdated();

    void currentIndexChanged();

    void currentTimeChanged();

    void selectedTracksChanged();

    void scrollValueChanged();

public: // Properties
    bool isPlaylistNet() const;

    bool isFeed  () const;
    bool isSearch() const;

    int  currentId() const;
    void setCurrentId(int id);

    int  currentIndex() const;
    void setCurrentIndex(int index);

    int  currentTime() const;
    void setCurrentTime(int msec);

    int firstSelected() const;
    int lastSelected () const;

    bool selectedAligned() const;

    int count() const;

    int  maxCount() const;
    void setMaxCount(int max);

    bool isEmpty() const;
    bool isFull () const;

    bool hasPreviousTrack() const;
    bool hasNextTrack    () const;

    QList<int> selectedTracks() const;
    void       setSelectedTracks(const QList<int> & indexes);

    int     selectedCount  () const;
    QString selectedSources() const;

    qreal scrollValue() const;
    void  setScrollValue(qreal value);

    QString currentTitle   () const;
    QUrl    currentCover   () const;
    int     currentDuration() const;

private:
    W_DECLARE_PRIVATE(WAbstractPlaylist)

    friend class WControllerPlaylist;
    friend class WControllerPlaylistPrivate;
};

#endif // SK_NO_ABSTRACTPLAYLIST
#endif // WABSTRACTPLAYLIST_H
