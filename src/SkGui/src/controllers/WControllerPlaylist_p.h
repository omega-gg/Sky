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

#ifndef WCONTROLLERPLAYLIST_P_H
#define WCONTROLLERPLAYLIST_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/WController_p>

#ifndef SK_NO_CONTROLLERPLAYLIST

//-------------------------------------------------------------------------------------------------
// WControllerPlaylistQuery
//-------------------------------------------------------------------------------------------------

class WControllerPlaylistQuery
{
public: // Enums
    enum Type
    {
        TypeTrack,
        TypePlaylist,
        TypeFolder
    };

public:
    WControllerPlaylistQuery(const WBackendNetQuery & backendQuery, Type type);

public: // Variables
    WBackendNetQuery backendQuery;
    Type             type;

    WBackendNet * backend;

    WRemoteData * data;
    QIODevice   * reply;

    WLibraryItem * item;

    WTrackNet * track;
};

//-------------------------------------------------------------------------------------------------
// WControllerPlaylistSource
//-------------------------------------------------------------------------------------------------

struct WControllerPlaylistSource
{
    QUrl    url;
    QString title;
};

//-------------------------------------------------------------------------------------------------
// WControllerPlaylistMedia
//-------------------------------------------------------------------------------------------------

struct WControllerPlaylistMedia
{
    QUrl    url;
    QString title;

    bool local;
};

//-------------------------------------------------------------------------------------------------
// WControllerPlaylistData
//-------------------------------------------------------------------------------------------------

class WControllerPlaylistData
{
public: // Interface
    void addSource(const QString & url, const QString & title);
    void addFolder(const QString & url, const QString & title);

    void addFile(const QString & path);

public: // Variables
    QString title;
    QUrl    cover;

    QList<WControllerPlaylistSource> sources;
    QList<WControllerPlaylistSource> folders;
    QList<WControllerPlaylistSource> files;

    QList<WControllerPlaylistMedia> medias;
};

//-------------------------------------------------------------------------------------------------
// Classes
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WControllerPlaylistPrivate : public WControllerPrivate
{
public:
    WControllerPlaylistPrivate(WControllerPlaylist * p);

    /* virtual */ ~WControllerPlaylistPrivate();

    void init();

public: // Interface
    bool applyQueryTrack(WPlaylistNet * playlist,
                         WTrackNet    * track, const WBackendNetQuery & query);

    bool applyQueryPlaylist(WPlaylistNet   * playlist, const WBackendNetQuery & query);
    bool applyQueryFolder  (WLibraryFolder * folder,   const WBackendNetQuery & query);

    //---------------------------------------------------------------------------------------------

    bool applySourceTrack(WPlaylistNet * playlist, WTrackNet * track, const QUrl & url);

    bool applySourcePlaylist(WPlaylistNet   * playlist, const QUrl & url);
    bool applySourceFolder  (WLibraryFolder * folder,   const QUrl & url);

    //---------------------------------------------------------------------------------------------

    bool abortQueryTrack   (WTrackNet    * track);
    bool abortQueriesTracks(WPlaylistNet * playlist);

    bool abortQueriesPlaylist(WPlaylistNet   * playlist);
    bool abortQueriesFolder  (WLibraryFolder * folder);

    bool abortQueriesItem(WLibraryItem * item);

public: // Functions
    void registerBackend  (WBackendNet * backend);
    void unregisterBackend(WBackendNet * backend);

    void registerFolder  (WLibraryFolder * folder);
    void unregisterFolder(WLibraryFolder * folder);

    void registerTab  (WTabTrack * tab);
    void unregisterTab(WTabTrack * tab);

    void registerItemId(WLibraryItem * item);

    //---------------------------------------------------------------------------------------------

    WLibraryFolder * getFolderRoot(int id) const;

    WLibraryFolder * getFolder(WLibraryFolder * folder, QList<int> * idFull) const;

    //---------------------------------------------------------------------------------------------

    WControllerPlaylistQuery * getQuery(WRemoteData * data) const;

    void removeQuery(WRemoteData * data, const WControllerPlaylistQuery * query);

    //---------------------------------------------------------------------------------------------

    QString generateSource(const QUrl & url) const;

    void addFolderSearch(WLibraryFolder * folder, const QString & source,
                                                  const QString & title) const;

    bool applyUrl(WLibraryFolder * folder, WBackendNet * backend, const QUrl & url) const;

    void applyPlaylist(WLibraryFolder * folder, WBackendNet * backend, const QUrl  & url,
                                                                       QList<QUrl> * urls) const;

    void applySources(WLibraryFolder                         * folder,
                      const QList<WControllerPlaylistSource> & sources,
                      QList<QUrl>                            * urls) const;

    void applyCurrentIndex(WPlaylistNet * playlist) const;

    void loadUrls(QIODevice * device, const WBackendNetQuery & query, const char * slot) const;

    void scanItems(QList<WLibraryFolderItem> * items) const;

    void addToCache(const QUrl & url, const QByteArray & array) const;

    //---------------------------------------------------------------------------------------------

    bool getDataTrack(WPlaylistNet * playlist, WTrackNet * track, const WBackendNetQuery & query);

    bool getDataPlaylist(WPlaylistNet   * playlist, const WBackendNetQuery & query);
    bool getDataFolder  (WLibraryFolder * folder,   const WBackendNetQuery & query);

    bool getDataRelated(WBackendNet * backend, WPlaylistNet * playlist, const QString & id);

    //---------------------------------------------------------------------------------------------

    void abortTrack   (WTrackNet      * track);
    void abortPlaylist(WPlaylistNet   * playlist);
    void abortFolder  (WLibraryFolder * folder);

public: // Slots
    void onLoaded(WRemoteData * data);

    void onTrackLoaded   (QIODevice * device, const WBackendNetTrack    & reply);
    void onPlaylistLoaded(QIODevice * device, const WBackendNetPlaylist & reply);
    void onFolderLoaded  (QIODevice * device, const WBackendNetFolder   & reply);

    void onUrlPlaylist(QIODevice * device, const WControllerPlaylistData & data);
    void onUrlFolder  (QIODevice * device, const WControllerPlaylistData & data);

public: // Variables
    QThread * thread;

    QList<WBackendNet    *> backends;
    QList<WLibraryFolder *> folders;
    QList<WTabTrack      *> tabs;

    QList<int> ids;

    QHash<WBackendNetQuery::Type, WAbstractLoader *> loaders;

    QHash<WRemoteData *, WControllerPlaylistQuery> queries;

    QHash<QIODevice *, WControllerPlaylistQuery *> replies;

    QMetaMethod methodHtml;
    QMetaMethod methodDir;
    QMetaMethod methodFile;

    QString pathCover;

protected:
    W_DECLARE_PUBLIC(WControllerPlaylist)
};

#endif // SK_NO_CONTROLLERPLAYLIST
#endif // WCONTROLLERPLAYLIST_P_H
