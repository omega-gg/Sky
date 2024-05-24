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

#ifndef WTABTRACK_H
#define WTABTRACK_H

// Sk includes
#include <WAbstractTab>
#include <WBookmarkTrack>

#ifndef SK_NO_TABTRACK

// Forward declarations
class WTabTrackPrivate;
#ifndef SK_NO_PLAYER
class WPlayer;
#endif

class SK_GUI_EXPORT WTabTrack : public WAbstractTab
{
    Q_OBJECT

    Q_PROPERTY(QList<WBookmarkTrack> bookmarks READ bookmarks NOTIFY countChanged)

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex
               NOTIFY currentBookmarkChanged)

    Q_PROPERTY(const WBookmarkTrack * currentBookmark READ currentBookmark
               NOTIFY currentBookmarkChanged)

    Q_PROPERTY(WPlaylist * playlist READ playlist WRITE setPlaylist NOTIFY playlistChanged)

#ifndef SK_NO_PLAYER
    Q_PROPERTY(WPlayer * player READ player WRITE setPlayer NOTIFY playerChanged)
#endif

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

    Q_PROPERTY(QList<int> folderIds READ folderIds NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QList<int> idPlaylist   READ idPlaylist   NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(int        idTrack      READ idTrack      NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(int        idFolderRoot READ idFolderRoot NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QVariantMap trackData READ trackData NOTIFY currentBookmarkUpdated)

    //---------------------------------------------------------------------------------------------
    // NOTE: We cannot use WTrack::Type and WTrack::State because WTrack is not a QObject.

    Q_PROPERTY(int type READ type NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(bool isLive        READ isLive        NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(bool isHub         READ isHub         NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(bool isChannel     READ isChannel     NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(bool isInteractive READ isInteractive NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(bool isLite        READ isLite        NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(int state READ state NOTIFY currentBookmarkUpdated)

    //---------------------------------------------------------------------------------------------

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QString title READ title NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(QString cover READ cover NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QString author READ author NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(QString feed   READ feed   NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(int duration READ duration NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QDateTime date READ date NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QString videoShot READ videoShot NOTIFY currentBookmarkUpdated)
    Q_PROPERTY(QString coverShot READ coverShot NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(int currentTime READ currentTime WRITE setCurrentTime NOTIFY currentBookmarkUpdated)

    Q_PROPERTY(QString subtitle READ subtitle WRITE setSubtitle NOTIFY currentBookmarkUpdated)

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

    Q_INVOKABLE void loadTrack  ();
    Q_INVOKABLE void reloadTrack();

public: // Virtual interface
    // NOTE: You can override the source and the currentTime. When passing -2 we skip the
    //       currentTime entirely.
    Q_INVOKABLE virtual QString toVbml(const QString & source      = QString(),
                                       int             currentTime = -1) const;

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

#ifndef SK_NO_PLAYER
    void playerChanged();
#endif

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

#ifndef SK_NO_PLAYER
    WPlayer * player() const;
    void      setPlayer(WPlayer * player);
#endif

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

    QList<int> folderIds() const;

    QList<int> idPlaylist  () const;
    int        idTrack     () const;
    int        idFolderRoot() const;

    QVariantMap trackData() const;

    WTrack::Type type() const;

    bool isLive       () const;
    bool isHub        () const;
    bool isChannel    () const;
    bool isInteractive() const;
    bool isLite       () const;

    WTrack::State state() const;

    QString source() const;

    // NOTE: When a playlist is available we update its track source, otherwise we update the
    //       current bookmark.
    void setSource(const QString & source);

    QString title() const;
    QString cover() const;

    QString author() const;
    QString feed  () const;

    int  duration() const;
    void setDuration(int msec);

    QDateTime date() const;

    QString videoShot() const;
    QString coverShot() const;

    int  currentTime() const;
    void setCurrentTime(int msec);

    QString subtitle() const;
    void    setSubtitle(const QString & subtitle);

private:
    W_DECLARE_PRIVATE(WTabTrack)

    Q_PRIVATE_SLOT(d_func(), void onFocusChanged())

    Q_PRIVATE_SLOT(d_func(), void onCurrentTrackChanged())

    Q_PRIVATE_SLOT(d_func(), void onTrackLoaded())

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
