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

#ifndef WPLAYLISTNET_H
#define WPLAYLISTNET_H

// Sk includes
#include <WAbstractPlaylist>
#include <WTrackNet>

#ifndef SK_NO_PLAYLISTNET

// Forward declarations
class WPlaylistNetPrivate;
class WBackendNetQuery;

//-------------------------------------------------------------------------------------------------
// WPlaylistNet
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WPlaylistNet : public WAbstractPlaylist
{
    Q_OBJECT

    Q_PROPERTY(QList<WTrackNet> tracks READ tracks)

public:
    explicit WPlaylistNet(WLibraryFolder * parent = NULL);
protected:
    WPlaylistNet(WPlaylistNetPrivate * p, Type type, WLibraryFolder * parent = NULL);

public: // Interface
    void addTrack (const WTrackNet        & track);
    void addTracks(const QList<WTrackNet> & tracks);

    void insertTrack (int index, const WTrackNet        & track);
    void insertTracks(int index, const QList<WTrackNet> & tracks);

    Q_INVOKABLE int addSource(const QUrl & url);

    Q_INVOKABLE int insertSource(int index, const QUrl & url);

    Q_INVOKABLE void removeTrack(int index);

    Q_INVOKABLE void removeTracks(const QList<int>      & indexes);
    Q_INVOKABLE void removeTracks(const QList<QVariant> & indexes);

    Q_INVOKABLE void removeSelectedTracks();

    Q_INVOKABLE void updateCover();

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void loadTrack (int at);
    Q_INVOKABLE void loadTracks(int at, int count);

    Q_INVOKABLE void abortTracks();

    //---------------------------------------------------------------------------------------------

    bool contains(const WTrackNet & track) const;

    Q_INVOKABLE bool containsSource(const QUrl & source) const;

    //---------------------------------------------------------------------------------------------

    WTrackNet getTrackAt(int index) const;

    Q_INVOKABLE WPlaylistNet * duplicate() const;

    Q_INVOKABLE void copyTracksTo(const QList<int> & tracks,
                                  WPlaylistNet     * destination, int to = -1) const;

    Q_INVOKABLE void copyTrackTo(int            from,
                                 WPlaylistNet * destination, int to = -1) const;

    Q_INVOKABLE void copyAllTo     (WPlaylistNet * destination, int to = -1) const;
    Q_INVOKABLE void copySelectedTo(WPlaylistNet * destination, int to = -1) const;

    //---------------------------------------------------------------------------------------------
    // Track interface

    Q_INVOKABLE bool trackIsValid(int index) const;

    Q_INVOKABLE QVariantMap trackData(int index) const;

    Q_INVOKABLE WAbstractTrack::State trackState   (int index) const;
    Q_INVOKABLE void                  setTrackState(int index, WAbstractTrack::State state);

    Q_INVOKABLE bool trackIsDefault(int index) const;
    Q_INVOKABLE bool trackIsLoading(int index) const;
    Q_INVOKABLE bool trackIsLoaded (int index) const;

    Q_INVOKABLE QUrl trackSource   (int index) const;
    Q_INVOKABLE void setTrackSource(int index, const QUrl & source);

    Q_INVOKABLE QString trackTitle   (int index) const;
    Q_INVOKABLE void    setTrackTitle(int index, const QString & title);

    Q_INVOKABLE QUrl trackCover   (int index) const;
    Q_INVOKABLE void setTrackCover(int index, const QUrl & cover);

    Q_INVOKABLE QString trackAuthor   (int index) const;
    Q_INVOKABLE void    setTrackAuthor(int index, const QString & author);

    Q_INVOKABLE QString trackFeed   (int index) const;
    Q_INVOKABLE void    setTrackFeed(int index, const QString & feed);

    //---------------------------------------------------------------------------------------------
    // QML

    Q_INVOKABLE void copyTracksTo(const QVariantList & tracks,
                                  WPlaylistNet       * destination, int to = -1) const;

public: // Static interface
    Q_INVOKABLE static WPlaylistNet * create(Type type = PlaylistNet);

    Q_INVOKABLE static WPlaylistNet * fromPlaylist(WAbstractPlaylist * playlist);

public: // WAbstractPlaylist implementation
    /* virtual */ int indexOf(const WAbstractTrack * track) const;

    /* virtual */ int indexFromId(int id) const;

protected:
    bool applyQueryTrack(WTrackNet * track, const WBackendNetQuery & query);

protected: // WLocalObject reimplementation
    /* virtual */ WAbstractThreadAction * onSave(const QString & path);
    /* virtual */ WAbstractThreadAction * onLoad(const QString & path);

protected: // WLibraryItem reimplementation
    /* virtual */ bool applySource(const QUrl             & source);
    /* virtual */ bool applyQuery (const WBackendNetQuery & query);

    /* virtual */ bool stopQuery();

protected: // WAbstractPlaylist implementation
    /* virtual */ int itemCount() const;

    /* virtual */ const WAbstractTrack * itemFromId(int id)    const;
    /* virtual */ const WAbstractTrack * itemAt    (int index) const;

    /* virtual */ void moveItemTo(int from, int to);

    /* virtual */ void clearItems();

public: // Properties
    QList<WTrackNet> tracks() const;

private:
    W_DECLARE_PRIVATE(WPlaylistNet)

    friend class WControllerPlaylist;
    friend class WControllerPlaylistPrivate;
};

//-------------------------------------------------------------------------------------------------
// WPlaylistFeed
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WPlaylistFeed : public WPlaylistNet
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

class SK_GUI_EXPORT WPlaylistSearch : public WPlaylistNet
{
    Q_OBJECT

public:
    explicit WPlaylistSearch(WLibraryFolder * parent = NULL);

private:
    Q_DISABLE_COPY(WPlaylistSearch)
};

#endif // SK_NO_PLAYLISTNET
#endif // WPLAYLISTNET_H
