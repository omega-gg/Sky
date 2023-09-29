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

#ifndef WCONTROLLERPLAYLIST_H
#define WCONTROLLERPLAYLIST_H

// Sk includes
#include <WController>
#include <WBackendNet>
#ifdef QT_4
#include <QStringList>
#endif

#ifndef SK_NO_CONTROLLERPLAYLIST

// Forward declarations
class WControllerPlaylistPrivate;
class WControllerPlaylistData;
class WControllerFileReply;
class WAbstractLoader;
class WYamlReader;
class WYamlNodeBase;
class WYamlNode;
class WRemoteData;

#ifdef QT_6
Q_MOC_INCLUDE("QThread")
#endif

// Defines
#define wControllerPlaylist WControllerPlaylist::instance()

//-------------------------------------------------------------------------------------------------
// WControllerPlaylistUrl
//-------------------------------------------------------------------------------------------------

struct WControllerPlaylistUrl
{
    WLibraryItem::Type type;

    QString url;
};

//-------------------------------------------------------------------------------------------------
// WControllerPlaylistSource
//-------------------------------------------------------------------------------------------------

struct WControllerPlaylistSource
{
    QString url;
    QString title;
};

//-------------------------------------------------------------------------------------------------
// WControllerPlaylistSource
//-------------------------------------------------------------------------------------------------

struct WControllerPlaylistMedia
{
    QString url;

    QString title;
    QString cover;
};

//-------------------------------------------------------------------------------------------------
// WControllerPlaylistSlice
//-------------------------------------------------------------------------------------------------

struct WControllerPlaylistSlice
{
    WControllerPlaylistSlice(const QString & start, const QString & end)
    {
        this->start = start;
        this->end   = end;
    }

    QString start;
    QString end;
};

//-------------------------------------------------------------------------------------------------
// WControllerPlaylistItem
//-------------------------------------------------------------------------------------------------

struct WControllerPlaylistItem
{
    QByteArray data;

    QString extension;
};

//-------------------------------------------------------------------------------------------------
// WControllerPlaylist
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WControllerPlaylist : public WController
{
    Q_OBJECT

    Q_ENUMS(Type)

    Q_PROPERTY(QThread * thread READ thread CONSTANT)

    Q_PROPERTY(QString versionApi READ versionApi CONSTANT)

    Q_PROPERTY(QString pathStorage     READ pathStorage     NOTIFY pathStorageChanged)
    Q_PROPERTY(QString pathStorageTabs READ pathStorageTabs NOTIFY pathStorageChanged)

public: // Enums
    enum Type
    {
        Unknown,
        Redirect,
        Source,
        Related,
        Track,
        Live,
        Hub,
        Channel,
        Playlist,
        Feed,
        Index,
        Backend
    };

private:
    WControllerPlaylist();

protected: // Initialize
    /* virtual */ void init();

public: // Interface
    Q_INVOKABLE WRemoteData * getData(WAbstractLoader        * loader,
                                      const WBackendNetQuery & query,
                                      QObject                * parent = NULL) const;

    Q_INVOKABLE WRemoteData * getData(const WBackendNetQuery & query,
                                      QObject                * parent = NULL) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE WLibraryItem * getLibraryItem(const QList<int> & idFull);

    Q_INVOKABLE void loadLibraryItem(const QList<int> & idFull);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE WBackendNet * backendFromId(const QString & id) const;

    Q_INVOKABLE WBackendNet * backendFromUrl   (const QString & url) const;
    Q_INVOKABLE WBackendNet * backendFromSource(const QString & url) const;

    Q_INVOKABLE WBackendNet * backendFromTrack   (const QString & url) const;
    Q_INVOKABLE WBackendNet * backendFromPlaylist(const QString & url) const;

    Q_INVOKABLE WBackendNet * backendSearch() const;

    Q_INVOKABLE WBackendNet * backendForCover(const QString & label, const QString & q) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE QString backendIdFromSource(const QString & url) const;

    Q_INVOKABLE QString backendCoverFromId (const QString & id)  const;
    Q_INVOKABLE QString backendCoverFromUrl(const QString & url) const;

    Q_INVOKABLE QString backendHubFromId (const QString & id)  const;
    Q_INVOKABLE QString backendHubFromUrl(const QString & url) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE WLibraryItem::Type urlType(const QString & url) const;

    Q_INVOKABLE bool urlIsTrack(const QString & url) const;

    // NOTE: Useful to test if a given url is just a track and nothing else.
    Q_INVOKABLE bool urlIsTrackOnly(const QString & url) const;

    Q_INVOKABLE bool urlIsPlaylist(const QString & url) const;

    Q_INVOKABLE bool sourceIsVideo(const QString & url) const;
    Q_INVOKABLE bool sourceIsAudio(const QString & url) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE QString sourceRelatedTracks(const QString & url, const QString & title) const;

    Q_INVOKABLE WBackendNetQuery queryPlaylist(const QString & url) const;

    // NOTE duckduckgo: It seems calling 'site:' too often ends up with a timeout.
    Q_INVOKABLE WBackendNetQuery queryRelatedTracks(const QString & url,
                                                    const QString & title,
                                                    const QString & prefix = QString()) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void abortFolderItems();

    Q_INVOKABLE void abortQueries();

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void registerLoader   (WBackendNetQuery::Type type, WAbstractLoader * loader);
    Q_INVOKABLE void unregisterLoader (WBackendNetQuery::Type type);
    Q_INVOKABLE void unregisterLoaders();

public: // Static functions
    Q_INVOKABLE static QString generateSource(const QString & url);

    Q_INVOKABLE static QString simpleSource(const QString & url);

    Q_INVOKABLE static QString simpleQuery(const QString & text);

    Q_INVOKABLE static QString createSource(const QString & backend, const QString & method,
                                            const QString & label,   const QString & q = QString());

    // NOTE: Returns the source fragment cleaned up. That's useful for comparison.
    Q_INVOKABLE static QString cleanSource(const QString & url);

    // NOTE: A source match with both fragments cleaned up.
    Q_INVOKABLE static bool cleanMatch(const QString & urlA, const QString & urlB);

    // NOTE: Extract a millisecond fragment value with the '#t=value' format.
    Q_INVOKABLE static int extractTime(const QString & string);

    Q_INVOKABLE static WControllerFileReply * copyBackends(const QString & path,
                                                           const QString & newPath);

    Q_INVOKABLE static WRemoteData * getDataQuery(WAbstractLoader        * loader,
                                                  const WBackendNetQuery & query,
                                                  QObject                * parent = NULL);

    // NOTE: This is useful when the feed is not a URL.
    Q_INVOKABLE static QString getFeed(const QString & feed, const QString & url);

    Q_INVOKABLE static QString getPlayerTime(int msec, int max = -1);

    Q_INVOKABLE static QStringList getLanguages();

    //---------------------------------------------------------------------------------------------
    // Filters

    Q_INVOKABLE static QString getFilterFile    ();
    Q_INVOKABLE static QString getFilterSubtitle();

    //---------------------------------------------------------------------------------------------
    // Texts

    // NOTE: Returns whether the text is a url and should be redirected, depending on the baseUrl.
    Q_INVOKABLE static bool textIsRedirect(const QString & text, const QString & baseUrl);

    //---------------------------------------------------------------------------------------------
    // Urls

    Q_INVOKABLE static bool urlIsMedia(const QString & url);
    Q_INVOKABLE static bool urlIsVideo(const QString & url);
    Q_INVOKABLE static bool urlIsAudio(const QString & url);

    Q_INVOKABLE static bool urlIsAscii   (const QString & url);
    Q_INVOKABLE static bool urlIsMarkup  (const QString & url);
    Q_INVOKABLE static bool urlIsText    (const QString & url);
    Q_INVOKABLE static bool urlIsSubtitle(const QString & url);

    Q_INVOKABLE static bool urlIsM3u(const QString & url);

#ifndef SK_NO_TORRENT
    Q_INVOKABLE static bool urlIsTorrent(const QString & url); // torrent or magnet.
#endif

    Q_INVOKABLE static bool urlIsVbml    (const QString & url); // vbml file or uri.
    Q_INVOKABLE static bool urlIsVbmlFile(const QString & url);
    Q_INVOKABLE static bool urlIsVbmlUri (const QString & url);

    Q_INVOKABLE static bool urlIsVbmlRun    (const QString & url); // vbml run url.
    Q_INVOKABLE static bool urlIsVbmlConnect(const QString & url); // vbml connect url.

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static bool extensionIsMedia(const QString & extension);
    Q_INVOKABLE static bool extensionIsVideo(const QString & extension);
    Q_INVOKABLE static bool extensionIsAudio(const QString & extension);

    Q_INVOKABLE static bool extensionIsAscii   (const QString & extension);
    Q_INVOKABLE static bool extensionIsMarkup  (const QString & extension);
    Q_INVOKABLE static bool extensionIsText    (const QString & extension);
    Q_INVOKABLE static bool extensionIsSubtitle(const QString & extension);

    Q_INVOKABLE static bool extensionIsM3u(const QString & extension);

#ifndef SK_NO_TORRENT
    Q_INVOKABLE static bool extensionIsTorrent(const QString & extension);
#endif

    Q_INVOKABLE static bool extensionIsVbml(const QString & extension);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QStringList extractTracks(const WControllerPlaylistData & data);

    Q_INVOKABLE
    static QList<WControllerPlaylistUrl> extractPlaylists(const WControllerPlaylistData & data);

    //---------------------------------------------------------------------------------------------
    // QML

    Q_INVOKABLE static WLibraryFolder * createFolder  (int type = WLibraryItem::Folder);
    Q_INVOKABLE static WPlaylist      * createPlaylist(int type = WLibraryItem::Playlist);

    //---------------------------------------------------------------------------------------------
    // VBML

    // NOTE: Writes the vbml header with versionApi.
    Q_INVOKABLE static QString vbml(const QString & append = "\n\n");

    // NOTE: Returns whether the data is VBML or not.
    Q_INVOKABLE static bool vbmlCheck(const QString & data);

    // NOTE: Returns the vbml header index.
    Q_INVOKABLE static int vbmlHeader(const QString & vbml);

    Q_INVOKABLE static QString vbmlVersion(const QString & vbml);
    Q_INVOKABLE static Type    vbmlType   (const QString & vbml);

    Q_INVOKABLE static QString vbmlSource(const WYamlNode & node);

    Q_INVOKABLE static int vbmlDuration(const WYamlNodeBase & node, int at           = 0,
                                                                    int defaultValue = 0);

    Q_INVOKABLE static int vbmlDurationSource(const WYamlNode & node, int at = 0);

    Q_INVOKABLE static Type vbmlTypeFromString(const QString & string);

    Q_INVOKABLE static bool vbmlTypeTrack   (Type type);
    Q_INVOKABLE static bool vbmlTypePlaylist(Type type);

    Q_INVOKABLE static void vbmlPatch(QString & data, const QString & api);

    Q_INVOKABLE static void vbmlApplyTrack(WTrack * track, const QString & uri);

signals:
    void filesCleared(const QList<int> & idFull);
    void filesDeleted(const QList<int> & idFull);

    void pathStorageChanged();

    void pathCoverChanged();

public: // Properties
    QThread * thread() const;

    static QString versionApi();

    QString pathStorage    () const;
    QString pathStorageTabs() const;

private:
    W_DECLARE_PRIVATE   (WControllerPlaylist)
    W_DECLARE_CONTROLLER(WControllerPlaylist)

    Q_PRIVATE_SLOT(d_func(), void onLoaded(WRemoteData *))

    Q_PRIVATE_SLOT(d_func(), void onTrackLoaded   (QIODevice *, const WBackendNetTrack    &))
    Q_PRIVATE_SLOT(d_func(), void onPlaylistLoaded(QIODevice *, const WBackendNetPlaylist &))
    Q_PRIVATE_SLOT(d_func(), void onFolderLoaded  (QIODevice *, const WBackendNetFolder   &))
    Q_PRIVATE_SLOT(d_func(), void onItemLoaded    (QIODevice *, const WBackendNetItem     &))

    Q_PRIVATE_SLOT(d_func(), void onUrlTrack   (QIODevice *, const WControllerPlaylistData &))
    Q_PRIVATE_SLOT(d_func(), void onUrlPlaylist(QIODevice *, const WControllerPlaylistData &))
    Q_PRIVATE_SLOT(d_func(), void onUrlFolder  (QIODevice *, const WControllerPlaylistData &))
    Q_PRIVATE_SLOT(d_func(), void onUrlItem    (QIODevice *, const WControllerPlaylistItem &))

    friend class WLibraryItem;
    friend class WLibraryItemPrivate;
    friend class WLibraryFolder;
    friend class WLibraryFolderPrivate;
    friend class WPlaylist;
    friend class WPlaylistPrivate;
    friend class WBackendLoaderPrivate;
    friend class WBackendNetPrivate;
    friend class WBackendNetInterface;
    friend class WTabTrackPrivate;
};

//-------------------------------------------------------------------------------------------------
// WControllerPlaylistData
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WControllerPlaylistData
{
public:
    WControllerPlaylistData()
    {
        currentTime = 0;

        type = WControllerPlaylist::Unknown;
    }

public: // Interface
    void applyVbml   (const QByteArray & array, const QString & url, const QString & urlBase);
    void applyRelated(const QByteArray & array, const QString & url, const QString & urlBase);

    void applyHtml(const QByteArray & array, const QString & url);
    void applyM3u (const QByteArray & array, const QString & url);

    void applyFolder(const QString & url);

    void applyFile(const QByteArray & array, const QString & url);

    void addSlice(const QString & start, const QString & end = QString());

public: // Static functions
    static QString extractHtml(const QByteArray & array);

    static QString extractHtmlLink(const QByteArray & array, const QString & url);

    static QString generateUrl  (const QString & url, const QString & baseUrl);
    static QString generateTitle(const QString & url, const QString & urlName);

private: // Functions
    void parseTrack   (WYamlReader & reader, const QString & type, const QString & url);
    void parsePlaylist(WYamlReader & reader);

    void parsePlaylistTrack(const WYamlNode & node, WTrack::Type type);

    void addSource(const QString & url, const QString & title);
    void addMedia (const QString & url, const QString & title);

    void addFile(const QString & path);

    bool addUrl(QStringList * urls, const QString & url) const;

    void extractSource(const QList<WYamlNode> & children);

    void applySource(const WYamlNodeBase & node, const QString & url, int duration);

    QString extractRelated(const WYamlNodeBase & node);

public: // Variables
    int currentTime;

    WControllerPlaylist::Type type;

    QString origin;
    QString source;

    QString title;
    QString cover;

    QList<WTrack> tracks;

    QList<WControllerPlaylistSource> sources;
    QList<WControllerPlaylistSource> files;
    QList<WControllerPlaylistMedia>  medias;

    QList<WControllerPlaylistSlice> slices;
};

#include <private/WControllerPlaylist_p>

#endif // SK_NO_CONTROLLERPLAYLIST
#endif // WCONTROLLERPLAYLIST_H
