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
        TypeFolder,
        TypeItem
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

    WTrack * track;
};

//-------------------------------------------------------------------------------------------------
// WControllerPlaylistPrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WControllerPlaylistPrivate : public WControllerPrivate
{
public:
    WControllerPlaylistPrivate(WControllerPlaylist * p);

    /* virtual */ ~WControllerPlaylistPrivate();

    void init();

public: // Interface
    bool applyQueryTrack(WPlaylist * playlist, WTrack * track, const WBackendNetQuery & query);

    bool applyQueryPlaylist(WPlaylist      * playlist, const WBackendNetQuery & query);
    bool applyQueryFolder  (WLibraryFolder * folder,   const WBackendNetQuery & query);
    bool applyQueryItem    (WLibraryItem   * item,     const WBackendNetQuery & query);

    //---------------------------------------------------------------------------------------------

    bool applySourceTrack(WPlaylist * playlist, WTrack * track, const QString & url);

    bool applySourcePlaylist(WPlaylist      * playlist, const QString & url);
    bool applySourceFolder  (WLibraryFolder * folder,   const QString & url);
    bool applySourceItem    (WLibraryItem   * item,     const QString & url);

    //---------------------------------------------------------------------------------------------

    bool abortQueryTrack   (WTrack    * track);
    bool abortQueriesTracks(WPlaylist * playlist);

    bool abortQueriesPlaylist(WPlaylist      * playlist);
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

    void removeQuery(WControllerPlaylistQuery * query);
    void deleteQuery(WControllerPlaylistQuery * query);

    //---------------------------------------------------------------------------------------------

    void addFolderSearch(WLibraryFolder * folder, const QString & source,
                                                  const QString & title) const;

    bool applyUrl(WLibraryFolder * folder, WBackendNet * backend, const QString & url) const;

    void applyPlaylist(WLibraryFolder * folder, WBackendNet * backend, const QString & url,
                                                                       QStringList   * urls) const;

    void applySources(WLibraryFolder                         * folder,
                      const QList<WControllerPlaylistSource> & sources,
                      QStringList                            * urls) const;

    void applyCurrentIndex(WPlaylist * playlist) const;

    void loadUrls(QIODevice * device, const WBackendNetQuery & query, const char * signal,
                                                                      const char * slot) const;

    void scanItems(QList<WLibraryFolderItem> * items) const;

    void addToCache(const QString & url, const QByteArray & array,
                                         const QString    & extension = QString()) const;

    //---------------------------------------------------------------------------------------------

    void getDataTrack(WPlaylist * playlist, WTrack * track, const WBackendNetQuery & query);

    void getDataPlaylist(WPlaylist      * playlist, const WBackendNetQuery & query);
    void getDataFolder  (WLibraryFolder * folder,   const WBackendNetQuery & query);
    void getDataItem    (WLibraryItem   * item,     const WBackendNetQuery & query);

    bool getDataRelated(WBackendNet * backend, WPlaylist * playlist, const QString & id);

    void getDataLibraryItem(WLibraryItem           * item,
                            const WBackendNetQuery & query, WControllerPlaylistQuery::Type type);

    //---------------------------------------------------------------------------------------------

    void abortTrack   (WTrack         * track);
    void abortPlaylist(WPlaylist      * playlist);
    void abortFolder  (WLibraryFolder * folder);

public: // Slots
    void onLoaded(WRemoteData * data);

    void onTrackLoaded   (QIODevice * device, const WBackendNetTrack    & reply);
    void onPlaylistLoaded(QIODevice * device, const WBackendNetPlaylist & reply);
    void onFolderLoaded  (QIODevice * device, const WBackendNetFolder   & reply);
    void onItemLoaded    (QIODevice * device, const WBackendNetItem     & reply);

    void onUrlPlaylist(QIODevice * device, const WControllerPlaylistData & data);
    void onUrlFolder  (QIODevice * device, const WControllerPlaylistData & data);
    void onUrlItem    (QIODevice * device, const WControllerPlaylistItem & data);

public: // Variables
    QThread * thread;

    QList<WBackendNet    *> backends;
    QList<WLibraryFolder *> folders;
    QList<WTabTrack      *> tabs;

    QList<int> ids;

    QHash<WBackendNetQuery::Type, WAbstractLoader *> loaders;

    QList<WControllerPlaylistQuery *> queries;

    QHash<WRemoteData *, WControllerPlaylistQuery *> jobs;
    QHash<QIODevice   *, WControllerPlaylistQuery *> replies;

    QMetaMethod methodHtml;
    QMetaMethod methodFolder;
    QMetaMethod methodFile;
    QMetaMethod methodItem;

    QString pathCover;

protected:
    W_DECLARE_PUBLIC(WControllerPlaylist)
};

#endif // SK_NO_CONTROLLERPLAYLIST
#endif // WCONTROLLERPLAYLIST_P_H
