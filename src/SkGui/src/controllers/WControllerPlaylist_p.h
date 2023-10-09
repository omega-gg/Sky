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

// Forward declarations
class WBackendLoader;

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
    bool applyQueryTrack(const QString & backendId,
                         WPlaylist * playlist, WTrack * track, WBackendNetQuery * query);

    bool applyQueryPlaylist(WPlaylist      * playlist, const WBackendNetQuery & query);
    bool applyQueryFolder  (WLibraryFolder * folder,   const WBackendNetQuery & query);
    bool applyQueryItem    (WLibraryItem   * item,     const WBackendNetQuery & query);

    //-------------------------------------------------------------------------------------------------

    bool applySourceTrack(WPlaylist * playlist, WTrack * track, const QString & url, int index);

    bool applySourcePlaylist(WPlaylist * playlist, const QString & url,
                                                   const QString & urlBase, int index);

    bool applySourceFolder(WLibraryFolder * folder, const QString & url,
                                                    const QString & urlBase, int index);

    bool applySourceItem(WLibraryItem * item, const QString & url, int index);

    //---------------------------------------------------------------------------------------------

    bool applyNextTrack(WPlaylist * playlist, WTrack * track, const QString & url, int index);

    bool applyNextPlaylist(WPlaylist * playlist, const QString & url,
                                                 const QString & urlBase, int index);

    bool applyNextFolder(WLibraryFolder * folder, const QString & url,
                                                  const QString & urlBase, int index);

    bool applyNextItem(WLibraryItem * item, const QString & url, int index);

    //---------------------------------------------------------------------------------------------

    bool abortQueryTrack   (WTrack    * track);
    bool abortQueriesTracks(WPlaylist * playlist);

    bool abortQueriesPlaylist(WPlaylist      * playlist);
    bool abortQueriesFolder  (WLibraryFolder * folder);
    bool abortQueriesItem    (WLibraryItem * item);

public: // Functions
    void registerLoader  (WBackendLoader * loader);
    void unregisterLoader(WBackendLoader * loader);

    void registerFolder  (WLibraryFolder * folder);
    void unregisterFolder(WLibraryFolder * folder);

    void registerTab  (WTabTrack * tab);
    void unregisterTab(WTabTrack * tab);

    void registerItemId(WLibraryItem * item);

#if defined(SK_CONSOLE) == false && defined(Q_OS_MACX)
    bool compareBundle(const CFStringRef bundle, const CFStringRef handler) const;
#endif

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

    void applyTrack(WPlaylist * playlist, WTrack * track, WTrack::State state, int index) const;

    void applyPlaylist(WLibraryFolder * folder, WBackendNet * backend, const QString & url,
                                                                       QStringList   * urls) const;

    void applyPlaylistVbml(WLibraryFolder * folder, const QString & url, QStringList * urls) const;

    void applySources(WLibraryFolder                         * folder,
                      const QList<WControllerPlaylistSource> & sources,
                      QStringList                            * urls) const;

    void applyCurrentIndex(WPlaylist * playlist) const;

    void loadUrls(QIODevice * device, const WBackendNetQuery & query,
                                      const QString          & url,
                                      const char             * signal, const char * slot) const;

    void scanItems(QList<WLibraryFolderItem> * items) const;

    void addToCache(const QString & url, const QByteArray & array,
                                         const QString    & extension = QString()) const;

    WBackendNet * backendTrack(const QString & source, WPlaylist * playlist, int index) const;

    //---------------------------------------------------------------------------------------------

    WBackendNetQuery extractQuery(WBackendNet * backend, const QUrl & url, QString & id) const;

    WBackendNetQuery extractRelated(const QUrl & url) const;

    bool resolveTrack   (const QString & backendId, WBackendNetQuery & query) const;
    bool resolvePlaylist(const QString & backendId, WBackendNetQuery & query) const;
    bool resolveFolder  (const QString & backendId, WBackendNetQuery & query) const;
    bool resolveItem    (const QString & backendId, WBackendNetQuery & query) const;

    bool resolveQuery(WBackendNetQuery & query) const;

    void getDataTrack(WPlaylist * playlist, WTrack * track, const WBackendNetQuery & query);

    void getDataPlaylist(WPlaylist      * playlist, const WBackendNetQuery & query);
    void getDataFolder  (WLibraryFolder * folder,   const WBackendNetQuery & query);
    void getDataItem    (WLibraryItem   * item,     const WBackendNetQuery & query);

    bool getDataRelated(WBackendNet * backend, WPlaylist * playlist, const QString & id);

    void getDataLibraryItem(WLibraryItem           * item,
                            const WBackendNetQuery & query, WControllerPlaylistQuery::Type type);

    //---------------------------------------------------------------------------------------------

    bool getNextTracks(const QString                 & backendId,
                       WPlaylist                     * playlist,
                       WTrack                        * track,
                       const QList<WBackendNetQuery> & queries, int index);

    bool getNextPlaylists(const QString                 & backendId,
                          WPlaylist                     * playlist,
                          const QList<WBackendNetQuery> & queries, int index);

    bool getNextFolders(const QString                 & backendId,
                        WLibraryFolder                * folder,
                        const QList<WBackendNetQuery> & queries, int index);

    bool getNextItems(const QString                 & backendId,
                      WLibraryItem                  * item,
                      const QList<WBackendNetQuery> & queries, int index);

    bool getNextTrack(const QString          & backendId,
                      WPlaylist              * playlist,
                      WTrack                 * track,
                      const WBackendNetQuery & query, int index);

    bool getNextPlaylist(const QString          & backendId,
                         WPlaylist              * playlist,
                         const WBackendNetQuery & query, int index);

    bool getNextFolder(const QString          & backendId,
                       WLibraryFolder         * folder,
                       const WBackendNetQuery & query, int index);

    bool getNextItem(const QString          & backendId,
                     WLibraryItem           * item,
                     const WBackendNetQuery & query, int index);

    //---------------------------------------------------------------------------------------------

    bool checkTrack   (WTrack         * track)    const;
    bool checkPlaylist(WPlaylist      * playlist) const;
    bool checkFolder  (WLibraryFolder * folder)   const;
    bool checkItem    (WLibraryItem   * item)     const;

    void abortTrack   (WTrack         * track);
    void abortPlaylist(WPlaylist      * playlist);
    void abortFolder  (WLibraryFolder * folder);
    void abortItem    (WLibraryItem   * item);

public: // Slots
    void onLoaded(WRemoteData * data);

    void onTrackLoaded   (QIODevice * device, const WBackendNetTrack    & reply);
    void onPlaylistLoaded(QIODevice * device, const WBackendNetPlaylist & reply);
    void onFolderLoaded  (QIODevice * device, const WBackendNetFolder   & reply);
    void onItemLoaded    (QIODevice * device, const WBackendNetItem     & reply);

    void onUrlTrack   (QIODevice * device, const WControllerPlaylistData & data);
    void onUrlPlaylist(QIODevice * device, const WControllerPlaylistData & data);
    void onUrlFolder  (QIODevice * device, const WControllerPlaylistData & data);
    void onUrlItem    (QIODevice * device, const WControllerPlaylistItem & data);

public: // Variables
    QThread * thread;

    QList<WBackendLoader *> backendLoaders;
    QList<WLibraryFolder *> folders;
    QList<WTabTrack      *> tabs;

    QList<int> ids;

    QHash<WBackendNetQuery::Type, WAbstractLoader *> loaders;

    QList<WControllerPlaylistQuery *> queries;

    QHash<WRemoteData *, WControllerPlaylistQuery *> jobs;
    QHash<QIODevice   *, WControllerPlaylistQuery *> replies;

    QMetaMethod methodVbml;
    QMetaMethod methodRelated;
    QMetaMethod methodHtml;
    QMetaMethod methodM3u;
    QMetaMethod methodFolder;
    QMetaMethod methodFile;
    QMetaMethod methodItem;

protected:
    W_DECLARE_PUBLIC(WControllerPlaylist)
};

#endif // SK_NO_CONTROLLERPLAYLIST
#endif // WCONTROLLERPLAYLIST_P_H
