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

#ifndef WTABTRACK_H
#define WTABTRACK_H

// Sk includes
#include <WAbstractTab>
#include <WBookmarkTrack>

#ifndef SK_NO_TABTRACK

// Forward declarations
class WTabTrackPrivate;
class WDeclarativePlayer;

class SK_GUI_EXPORT WTabTrack : public WAbstractTab
{
    Q_OBJECT

    Q_PROPERTY(QList<WBookmarkTrack> bookmarks READ bookmarks NOTIFY countChanged)

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex
               NOTIFY currentBookmarkChanged)

    Q_PROPERTY(const WBookmarkTrack * currentBookmark READ currentBookmark
               NOTIFY currentBookmarkChanged)

    Q_PROPERTY(WPlaylist * playlist READ playlist WRITE setPlaylist NOTIFY playlistChanged)

    Q_PROPERTY(WDeclarativePlayer * player READ player WRITE setPlayer NOTIFY playerChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(int trackIndex READ trackIndex WRITE setTrackIndex NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(bool isDefault READ isDefault NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(bool isLoaded  READ isLoaded  NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(bool isValid READ isValid NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(bool stackEnabled READ stackEnabled WRITE setStackEnabled
               NOTIFY stackEnabledChanged)

    Q_PROPERTY(bool hasPreviousBookmark READ hasPreviousBookmark NOTIFY currentBookmarkChanged)
    Q_PROPERTY(bool hasNextBookmark     READ hasNextBookmark     NOTIFY currentBookmarkChanged)

    Q_PROPERTY(bool hasPreviousTrack READ hasPreviousTrack NOTIFY playlistUpdated)
    Q_PROPERTY(bool hasNextTrack     READ hasNextTrack     NOTIFY playlistUpdated)

    Q_PROPERTY(QUrl coverShot READ coverShot NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QList<int> folderIds READ folderIds NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QList<int> idPlaylist   READ idPlaylist   NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(int        idTrack      READ idTrack      NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(int        idFolderRoot READ idFolderRoot NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QVariantMap trackData READ trackData NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(int state READ state NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QUrl source READ source NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QString title READ title NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(QUrl    cover READ cover NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QString author READ author NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(QString feed   READ feed   NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(int duration READ duration NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QDateTime date READ date NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(WAbstractBackend::Quality quality READ quality NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QUrl videoShot READ videoShot NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(int currentTime READ currentTime WRITE setCurrentTime NOTIFY currentBookmarkUpdated)

public:
    explicit WTabTrack(WAbstractTabs * parent = NULL);

public: // Interface
    Q_INVOKABLE void pushBookmark(const WBookmarkTrack & bookmark);

    Q_INVOKABLE void clearBookmarks();

    Q_INVOKABLE void setPreviousBookmark();
    Q_INVOKABLE void setNextBookmark    ();

    Q_INVOKABLE void setPreviousTrack(bool cycle = false);
    Q_INVOKABLE void setNextTrack    (bool cycle = false);

    Q_INVOKABLE const WTrack * currentTrackPointer() const;
    Q_INVOKABLE void           setCurrentTrackPointer(const WTrack * track);

    Q_INVOKABLE void copyTrackTo(WPlaylist * destination, int to = -1);

    Q_INVOKABLE void updateBookmark();

public: // WLocalObject reimplementation
    /* Q_INVOKABLE virtual */ QString getParentPath() const;

protected: // WLocalObject reimplementation
    /* virtual */ WAbstractThreadAction * onSave(const QString & path);
    /* virtual */ WAbstractThreadAction * onLoad(const QString & path);

    /* virtual */ bool hasFolder() const;

signals:
    void currentBookmarkChanged();
    void currentBookmarkUpdated();

    void playlistChanged();
    void playerChanged  ();

    void countChanged();

    void playlistUpdated();

    void stackEnabledChanged();

    void videoShotUpdated();

public: // Properties
    QList<WBookmarkTrack> bookmarks() const;

    int  currentIndex() const;
    void setCurrentIndex(int index);

    const WBookmarkTrack * currentBookmark() const;

    WPlaylist * playlist() const;
    void        setPlaylist(WPlaylist * playlist);

    WDeclarativePlayer * player() const;
    void                 setPlayer(WDeclarativePlayer * player);

    int count() const;

    int  trackIndex() const;
    void setTrackIndex(int index);

    bool isDefault() const;
    bool isLoading() const;
    bool isLoaded () const;

    bool isValid() const;

    bool stackEnabled() const;
    void setStackEnabled(bool enabled);

    bool hasPreviousBookmark() const;
    bool hasNextBookmark    () const;

    bool hasPreviousTrack() const;
    bool hasNextTrack    () const;

    QUrl coverShot() const;

    QList<int> folderIds() const;

    QList<int> idPlaylist  () const;
    int        idTrack     () const;
    int        idFolderRoot() const;

    QVariantMap trackData() const;

    WTrack::State state() const;

    QUrl source() const;

    QString title() const;
    QUrl    cover() const;

    QString author() const;
    QString feed  () const;

    int  duration() const;
    void setDuration(int msec);

    QDateTime date() const;

    WAbstractBackend::Quality quality() const;

    QUrl videoShot() const;

    int  currentTime() const;
    void setCurrentTime(int msec);

private:
    W_DECLARE_PRIVATE(WTabTrack)

    Q_PRIVATE_SLOT(d_func(), void onFocusChanged())

    Q_PRIVATE_SLOT(d_func(), void onCurrentTrackChanged())

    Q_PRIVATE_SLOT(d_func(), void onFilesUpdated(const QList<int> &))

    Q_PRIVATE_SLOT(d_func(), void onPlaylistDestroyed())

    friend class WControllerPlaylist;
    friend class WControllerPlaylistPrivate;
    friend class WTabsTrack;
    friend class WTabsTrackPrivate;
    friend class WBookmarkTrack;
    friend class WBookmarkTrackPrivate;
};

#include <private/WTabTrack_p>

#endif // SK_NO_TABTRACK
#endif // WTABTRACK_H
