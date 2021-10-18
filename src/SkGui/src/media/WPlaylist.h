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

#ifndef WPLAYLIST_H
#define WPLAYLIST_H

// Sk includes
#include <WLibraryItem>
#include <WTrack>

#ifndef SK_NO_PLAYLIST

// Forward declarations
class WPlaylistPrivate;

//-------------------------------------------------------------------------------------------------
// WPlaylistWatcher
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WPlaylistWatcher
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
    friend class WPlaylist;
    friend class WPlaylistPrivate;
};

//-------------------------------------------------------------------------------------------------
// WPlaylist
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WPlaylist : public WLibraryItem
{
    Q_OBJECT

    Q_ENUMS(TrackType)
    Q_ENUMS(TrackState)

    Q_PROPERTY(bool isBase   READ isBase   CONSTANT)
    Q_PROPERTY(bool isFeed   READ isFeed   CONSTANT)
    Q_PROPERTY(bool isSearch READ isSearch CONSTANT)

    Q_PROPERTY(int currentId READ currentId WRITE setCurrentId NOTIFY currentTrackChanged)

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

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
    Q_PROPERTY(QString currentCover    READ currentCover    NOTIFY currentTrackChanged)
    Q_PROPERTY(int     currentDuration READ currentDuration NOTIFY currentTrackChanged)

public: // Enums
    //---------------------------------------------------------------------------------------------
    // NOTE: These enums are useful in QML because WTrack is not a QObject.

    enum TrackType { TrackUnknown, TrackMedia, TrackLive };

    enum TrackState { TrackDefault, TrackLoading, TrackLoaded, TrackCover };

    //---------------------------------------------------------------------------------------------

public:
    explicit WPlaylist(WLibraryFolder * parent = NULL);
protected:
    WPlaylist(WPlaylistPrivate * p, Type type, WLibraryFolder * parent = NULL);

public: // Interface
    Q_INVOKABLE void addTrack (const WTrack        & track);
    Q_INVOKABLE void addTracks(const QList<WTrack> & tracks);

    Q_INVOKABLE void insertTrack (int index, const WTrack        & track);
    Q_INVOKABLE void insertTracks(int index, const QList<WTrack> & tracks);

    Q_INVOKABLE int addSource(const QString & url);

    Q_INVOKABLE int insertSource(int index, const QString & url);

    Q_INVOKABLE void moveTrack(int from, int to);

    Q_INVOKABLE void removeTrack(int index);

    Q_INVOKABLE void removeTracks(const QList<int> & indexes);

    Q_INVOKABLE void removeSelectedTracks();

    Q_INVOKABLE void clearTracks();

    Q_INVOKABLE void updateCover();

    //---------------------------------------------------------------------------------------------

    // NOTE: When reloading you can specify a delay in msec to skip recently reloaded tracks.
    Q_INVOKABLE void loadTrack  (int at);
    Q_INVOKABLE void reloadTrack(int at, int delay = -1);

    Q_INVOKABLE void loadTracks  (int at, int count);
    Q_INVOKABLE void reloadTracks(int at, int count, int delay = -1);

    // NOTE: Loading a 'count' of tracks before and after the 'at' index.
    Q_INVOKABLE void loadTracksBetween  (int at, int count);
    Q_INVOKABLE void reloadTracksBetween(int at, int count, int delay = -1);

    Q_INVOKABLE void abortTracks();

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE bool checkFull(int count = 1) const;

    Q_INVOKABLE bool hasPreviousIndex(int index) const;
    Q_INVOKABLE bool hasNextIndex    (int index) const;

    Q_INVOKABLE void setPreviousTrack(bool cycle = false);
    Q_INVOKABLE void setNextTrack    (bool cycle = false);

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

    Q_INVOKABLE WTrack getTrackAt(int index) const;

    Q_INVOKABLE int indexOf(const WTrack * track) const;

    Q_INVOKABLE int indexFromId(int id) const;

    Q_INVOKABLE int indexFromSource(const QString & source, bool cleanFragment = false) const;

    Q_INVOKABLE bool contains(const WTrack & track) const;

    Q_INVOKABLE bool containsSource(const QString & source) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE WPlaylist * duplicate() const;

    Q_INVOKABLE void copyTracksTo(const QList<int> & tracks,
                                  WPlaylist        * destination, int to = -1) const;

    Q_INVOKABLE void copyTrackTo(int from, WPlaylist * destination, int to = -1) const;

    Q_INVOKABLE void copyAllTo     (WPlaylist * destination, int to = -1) const;
    Q_INVOKABLE void copySelectedTo(WPlaylist * destination, int to = -1) const;

    //---------------------------------------------------------------------------------------------
    // Track interface

    Q_INVOKABLE bool trackIsValid(int index) const;

    Q_INVOKABLE QVariantMap trackData(int index) const;
    Q_INVOKABLE QString     trackVbml(int index) const;

    Q_INVOKABLE WTrack::Type trackType   (int index) const;
    Q_INVOKABLE void         setTrackType(int index, WTrack::Type type);

    Q_INVOKABLE WTrack::State trackState   (int index) const;
    Q_INVOKABLE void          setTrackState(int index, WTrack::State state);

    Q_INVOKABLE bool trackIsDefault(int index) const;
    Q_INVOKABLE bool trackIsLoading(int index) const;
    Q_INVOKABLE bool trackIsLoaded (int index) const;

    Q_INVOKABLE QString trackSource   (int index) const;
    Q_INVOKABLE void    setTrackSource(int index, const QString & source);

    Q_INVOKABLE QString trackTitle   (int index) const;
    Q_INVOKABLE void    setTrackTitle(int index, const QString & title);

    Q_INVOKABLE QString trackCover   (int index) const;
    Q_INVOKABLE void    setTrackCover(int index, const QString & cover);

    Q_INVOKABLE QString trackAuthor   (int index) const;
    Q_INVOKABLE void    setTrackAuthor(int index, const QString & author);

    Q_INVOKABLE QString trackFeed   (int index) const;
    Q_INVOKABLE void    setTrackFeed(int index, const QString & feed);

    Q_INVOKABLE int  trackDuration   (int index) const;
    Q_INVOKABLE void setTrackDuration(int index, int msec);

    //---------------------------------------------------------------------------------------------
    // Pointers

    Q_INVOKABLE const WTrack * trackPointerFromId(int id)    const;
    Q_INVOKABLE const WTrack * trackPointerAt    (int index) const;

    Q_INVOKABLE QList<const WTrack *> trackPointers() const;

    Q_INVOKABLE const WTrack * currentTrackPointer() const;
    Q_INVOKABLE void           setCurrentTrackPointer(const WTrack * track);

    Q_INVOKABLE bool containsTrackPointer(const WTrack * track) const;

    //---------------------------------------------------------------------------------------------
    // Watchers

    Q_INVOKABLE void registerWatcher  (WPlaylistWatcher * watcher);
    Q_INVOKABLE void unregisterWatcher(WPlaylistWatcher * watcher);

#ifdef QT_4
    //---------------------------------------------------------------------------------------------
    // QML

    Q_INVOKABLE void removeTracks(const QVariantList & indexes);

    Q_INVOKABLE void copyTracksTo(const QVariantList & tracks,
                                  WPlaylist          * destination, int to = -1) const;
#endif

public: // Static functions
    Q_INVOKABLE static WPlaylist * create(Type type = Playlist);

public: // WLibraryItem reimplementation
    Q_INVOKABLE /* virtual */ QString toVbml() const;

protected: // Functions
    void updateIndex();

    void updateTrack(int index);

    void updateTrack(const WTrack * track);

    void beginTracksInsert(int first, int last)         const;
    void beginTracksMove  (int first, int last, int to) const;
    void beginTracksRemove(int first, int last)         const;

    void endTracksInsert() const;
    void endTracksMove  () const;
    void endTracksRemove() const;

protected: // WLocalObject reimplementation
    /* virtual */ WAbstractThreadAction * onSave(const QString & path);
    /* virtual */ WAbstractThreadAction * onLoad(const QString & path);

protected: // WLibraryItem reimplementation
    /* virtual */ bool applySource(const QString          & source);
    /* virtual */ bool applyQuery (const WBackendNetQuery & query);

    /* virtual */ bool stopQuery();

    /* virtual */ void onApplyCurrentIds(const QList<int> & ids);

signals:
    void tracksInserted(int index, int count);

    void tracksMoved(const QList<int> & indexes, int to);

    void tracksRemoved(const QList<int> & indexes);

    void trackUpdated(int index);

    void tracksCleared();

    void trackQueryEnded    ();
    void trackQueryCompleted();

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
    bool isBase  () const;
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
    QString currentCover   () const;
    int     currentDuration() const;

private:
    W_DECLARE_PRIVATE(WPlaylist)

    friend class WControllerPlaylist;
    friend class WControllerPlaylistPrivate;
};

//-------------------------------------------------------------------------------------------------
// WPlaylistFeed
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WPlaylistFeed : public WPlaylist
{
    Q_OBJECT

public:
    explicit WPlaylistFeed(WLibraryFolder * parent = NULL);

private:
    Q_DISABLE_COPY(WPlaylistFeed)
};

//-------------------------------------------------------------------------------------------------
// WPlaylistSearch
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WPlaylistSearch : public WPlaylist
{
    Q_OBJECT

public:
    explicit WPlaylistSearch(WLibraryFolder * parent = NULL);

private:
    Q_DISABLE_COPY(WPlaylistSearch)
};

#endif // SK_NO_PLAYLIST
#endif // WPLAYLIST_H
