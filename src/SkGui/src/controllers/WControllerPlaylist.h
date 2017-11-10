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

#ifndef WCONTROLLERPLAYLIST_H
#define WCONTROLLERPLAYLIST_H

// Sk includes
#include <WController>
#include <WBackendNet>

#ifndef SK_NO_CONTROLLERPLAYLIST

// Forward declarations
class WControllerPlaylistPrivate;
class WAbstractLoader;
class WRemoteData;

// Defines
#define wControllerPlaylist WControllerPlaylist::instance()

class SK_GUI_EXPORT WControllerPlaylist : public WController
{
    Q_OBJECT

    Q_PROPERTY(QThread * thread READ thread CONSTANT)

    Q_PROPERTY(QList<WBackendNet *> backends READ backends)

    Q_PROPERTY(QString pathStorage     READ pathStorage     NOTIFY pathStorageChanged)
    Q_PROPERTY(QString pathStorageTabs READ pathStorageTabs NOTIFY pathStorageChanged)

    Q_PROPERTY(QString pathCover READ pathCover WRITE setPathCover NOTIFY pathCoverChanged)

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

    Q_INVOKABLE WBackendNet * backendFromUrl(const QUrl & url) const;

    Q_INVOKABLE WBackendNet * backendFromTrack   (const QUrl & url) const;
    Q_INVOKABLE WBackendNet * backendFromPlaylist(const QUrl & url) const;

    Q_INVOKABLE WBackendNet * backendForCover(const QString & label, const QString & q) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE QUrl backendCover(WBackendNet * backend) const;

    Q_INVOKABLE QUrl backendCoverFromId (const QString & id)  const;
    Q_INVOKABLE QUrl backendCoverFromUrl(const QUrl    & url) const;

    Q_INVOKABLE QUrl backendCoverFromHub(const QUrl & url) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE WLibraryItem::Type urlType(const QUrl & url) const;

    Q_INVOKABLE bool urlIsTrack   (const QUrl & url) const;
    Q_INVOKABLE bool urlIsPlaylist(const QUrl & url) const;

    Q_INVOKABLE bool sourceIsVideo(const QUrl & url) const;
    Q_INVOKABLE bool sourceIsAudio(const QUrl & url) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void createBackendItems (WLibraryFolder * folder) const;
    Q_INVOKABLE void restoreBackendItems(WLibraryFolder * folder) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void abortFolderItems();

    Q_INVOKABLE void abortQueries();

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void registerLoader   (WBackendNetQuery::Type type, WAbstractLoader * loader);
    Q_INVOKABLE void unregisterLoader (WBackendNetQuery::Type type);
    Q_INVOKABLE void unregisterLoaders();

public: // Static functions
    Q_INVOKABLE static QUrl createSource(const QString & backend, const QString & method,
                                         const QString & label,   const QString & q = QString());

    Q_INVOKABLE static WRemoteData * getDataQuery(WAbstractLoader        * loader,
                                                  const WBackendNetQuery & query,
                                                  QObject                * parent = NULL);

    Q_INVOKABLE static QString getPlayerTime(int msec, int max = -1);

    Q_INVOKABLE static QString getFileFilter();

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static bool urlIsMedia(const QUrl & url);
    Q_INVOKABLE static bool urlIsVideo(const QUrl & url);
    Q_INVOKABLE static bool urlIsAudio(const QUrl & url);

    Q_INVOKABLE static bool urlIsAscii (const QUrl & url);
    Q_INVOKABLE static bool urlIsMarkup(const QUrl & url);
    Q_INVOKABLE static bool urlIsText  (const QUrl & url);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static bool extensionIsMedia(const QString & extension);
    Q_INVOKABLE static bool extensionIsVideo(const QString & extension);
    Q_INVOKABLE static bool extensionIsAudio(const QString & extension);

    Q_INVOKABLE static bool extensionIsAscii (const QString & extension);
    Q_INVOKABLE static bool extensionIsMarkup(const QString & extension);
    Q_INVOKABLE static bool extensionIsText  (const QString & extension);

signals:
    void filesCleared(const QList<int> & idFull);
    void filesDeleted(const QList<int> & idFull);

    void pathStorageChanged();

    void pathCoverChanged();

public: // Properties
    QThread * thread() const;

    QList<WBackendNet *> backends() const;

    QString pathStorage    () const;
    QString pathStorageTabs() const;

    QString pathCover() const;
    void    setPathCover(const QString & path);

private:
    W_DECLARE_PRIVATE   (WControllerPlaylist)
    W_DECLARE_CONTROLLER(WControllerPlaylist)

    Q_PRIVATE_SLOT(d_func(), void onLoaded(WRemoteData *))

    Q_PRIVATE_SLOT(d_func(), void onTrackLoaded   (QIODevice *, const WBackendNetTrack    &))
    Q_PRIVATE_SLOT(d_func(), void onPlaylistLoaded(QIODevice *, const WBackendNetPlaylist &))
    Q_PRIVATE_SLOT(d_func(), void onFolderLoaded  (QIODevice *, const WBackendNetFolder   &))

    Q_PRIVATE_SLOT(d_func(), void onUrlPlaylist(QIODevice *, const WControllerPlaylistData &))
    Q_PRIVATE_SLOT(d_func(), void onUrlFolder  (QIODevice *, const WControllerPlaylistData &))

    friend class WLibraryItem;
    friend class WLibraryItemPrivate;
    friend class WLibraryFolder;
    friend class WLibraryFolderPrivate;
    friend class WPlaylist;
    friend class WPlaylistPrivate;
    friend class WBackendNet;
    friend class WBackendNetPrivate;
    friend class WTabTrack;
    friend class WTabTrackPrivate;
};

#include <private/WControllerPlaylist_p>

#endif // SK_NO_CONTROLLERPLAYLIST
#endif // WCONTROLLERPLAYLIST_H
