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

#ifndef WBACKENDNET_H
#define WBACKENDNET_H

// Qt includes
#include <QNetworkRequest>
#include <QDateTime>

// Sk includes
#include <WLibraryFolder>
#include <WTrack>

#ifndef SK_NO_BACKENDNET

// Forward declarations
class WBackendNetPrivate;

//-------------------------------------------------------------------------------------------------
// WBackendNetQuery
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetQuery
{
public: // Enums
    enum Type
    {
        TypeDefault,
        TypeWeb,
        TypeTorrent
    };

    enum Target
    {
        TargetDefault,
        TargetHtml,
        TargetDir,
        TargetFile
    };

public:
    explicit WBackendNetQuery(const QUrl & url = QUrl());

    WBackendNetQuery(Type type, const QUrl & url = QUrl());

public: // Interface
    bool isValid() const;

private: // Functions
    void init(Type type, const QUrl & url);

public: // Variables
    Type   type;
    Target target;

    QUrl url;

    int id;

    QNetworkRequest::Priority priority;

    QVariant data;

    bool clearItems;
    bool cookies;

    int maxHost;
    int delay;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetPlaylistInfo
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetPlaylistInfo
{
public:
    explicit WBackendNetPlaylistInfo(WLibraryItem::Type type = WLibraryItem::Playlist,
                                     QString            id   = QString());
public: // Interface
    bool isValid() const;

    bool isFeed() const;

public: // Variables
    WLibraryItem::Type type;

    QString id;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetSource
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetSource
{
public:
    WBackendNetSource();

public: // Variables
    bool valid;

    QHash<WAbstractBackend::Quality, QUrl> medias;
    QHash<WAbstractBackend::Quality, QUrl> audios;

    QDateTime expiry;

    WBackendNetQuery nextQuery;

    QVariant data;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetItem
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetItem
{
public:
    WBackendNetItem();

public: // Variables
    bool valid;

    QByteArray cache;

    WBackendNetQuery nextQuery;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetTrack
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetTrack : public WBackendNetItem
{
public:
    WBackendNetTrack();

public: // Variables
    WTrack track;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetPlaylist
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetPlaylist : public WBackendNetItem
{
public:
    WBackendNetPlaylist();

public: // Variables
    QString title;
    QUrl    cover;

    QList<WTrack> tracks;

    int currentIndex;

    bool clearDuplicate;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetFolder
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetFolder : public WBackendNetItem
{
public:
    WBackendNetFolder();

public: // Variables
    QString title;
    QUrl    cover;

    QList<WLibraryFolderItem> items;

    int currentIndex;

    bool scanItems;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetReply
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetReply : public QObject
{
    Q_OBJECT

protected:
    WBackendNetReply(QIODevice * device, const WBackendNetQuery & query);

public: // Properties
    QIODevice * device() const;

    WBackendNetQuery query() const;

private: // Variables
    QIODevice * _device;

    WBackendNetQuery _query;

private:
    friend class WBackendNet;
    friend class WBackendNetPrivate;
};

//-------------------------------------------------------------------------------------------------
// WNetReplySource
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WNetReplySource : public WBackendNetReply
{
    Q_OBJECT

public:
    WNetReplySource(QIODevice * device, const WBackendNetQuery & query);

signals:
    void loaded(QIODevice * device, const WBackendNetSource & source);

private:
    friend class WBackendNet;
    friend class WBackendNetPrivate;
};

//-------------------------------------------------------------------------------------------------
// WNetReplyTrack
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WNetReplyTrack : public WBackendNetReply
{
    Q_OBJECT

public:
    WNetReplyTrack(QIODevice * device, const WBackendNetQuery & query);

signals:
    void loaded(QIODevice * device, const WBackendNetTrack & track);

private:
    friend class WBackendNet;
    friend class WBackendNetPrivate;
};

//-------------------------------------------------------------------------------------------------
// WNetReplyPlaylist
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WNetReplyPlaylist : public WBackendNetReply
{
    Q_OBJECT

public:
    WNetReplyPlaylist(QIODevice * device, const WBackendNetQuery & query);

signals:
    void loaded(QIODevice * device, const WBackendNetPlaylist & playlist);

private:
    friend class WBackendNet;
    friend class WBackendNetPrivate;
};

//-------------------------------------------------------------------------------------------------
// WNetReplyFolder
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WNetReplyFolder : public WBackendNetReply
{
    Q_OBJECT

public:
    WNetReplyFolder(QIODevice * device, const WBackendNetQuery & query);

signals:
    void loaded(QIODevice * device, const WBackendNetFolder & folder);

private:
    friend class WBackendNet;
    friend class WBackendNetPrivate;
};

//-------------------------------------------------------------------------------------------------
// WBackendNet
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNet : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(QString id    READ id    CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)

protected:
    WBackendNet(WBackendNetPrivate * p);

public: // Interface
    Q_INVOKABLE WLibraryItem::Type getPlaylistType(const QUrl & url) const;
    Q_INVOKABLE QString            getPlaylistId  (const QUrl & url) const;

    Q_INVOKABLE QUrl getSourceTrack   (const QUrl & url) const;
    Q_INVOKABLE QUrl getSourcePlaylist(const QUrl & url) const;

    Q_INVOKABLE bool checkQuery(const QUrl & source) const;

    Q_INVOKABLE WBackendNetQuery extractQuery(const QUrl & source) const;

    Q_INVOKABLE void loadSource(QIODevice              * device,
                                const WBackendNetQuery & query,
                                QObject                * receiver,
                                const char             * method);

    Q_INVOKABLE void loadTrack(QIODevice              * device,
                               const WBackendNetQuery & query,
                               QObject                * receiver,
                               const char             * method);

    Q_INVOKABLE void loadPlaylist(QIODevice              * device,
                                  const WBackendNetQuery & query,
                                  QObject                * receiver,
                                  const char             * method);

    Q_INVOKABLE void loadFolder(QIODevice              * device,
                                const WBackendNetQuery & query,
                                QObject                * receiver,
                                const char             * method);

public: // Abstract interface
    Q_INVOKABLE virtual QString getId   () const = 0;
    Q_INVOKABLE virtual QString getTitle() const = 0;

public: // Virtual interface
    Q_INVOKABLE virtual bool isHub() const; /* {} */

    Q_INVOKABLE virtual bool isSearchEngine() const; /* {} */
    Q_INVOKABLE virtual bool isSearchCover () const; /* {} */

    Q_INVOKABLE virtual bool checkValidUrl(const QUrl & url) const; /* {} */

    Q_INVOKABLE virtual bool checkCover(const QString & label, const QString & q) const; /* {} */

    Q_INVOKABLE virtual QString getHost() const; /* {} */

    Q_INVOKABLE virtual QList<WLibraryFolderItem> getLibraryItems() const; /* {} */

    Q_INVOKABLE virtual QString getTrackId(const QUrl & url) const; /* {} */

    Q_INVOKABLE virtual WAbstractBackend::Output getTrackOutput(const QUrl & url) const; /* {} */

    Q_INVOKABLE virtual WBackendNetPlaylistInfo getPlaylistInfo(const QUrl & url) const; /* {} */

    Q_INVOKABLE virtual QUrl getUrlTrack(const QString & id) const; /* {} */

    Q_INVOKABLE virtual QUrl getUrlPlaylist(const WBackendNetPlaylistInfo & info) const; /* {} */

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE virtual WBackendNetQuery getQuerySource  (const QUrl & url) const;
    Q_INVOKABLE virtual WBackendNetQuery getQueryTrack   (const QUrl & url) const; /* {} */
    Q_INVOKABLE virtual WBackendNetQuery getQueryPlaylist(const QUrl & url) const; /* {} */

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE virtual WBackendNetQuery createQuery(const QString & method,
                                                     const QString & label,
                                                     const QString & q) const; /* {} */

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE virtual
    WBackendNetSource extractSource(const QByteArray       & data,
                                    const WBackendNetQuery & query) const; /* {} */

    Q_INVOKABLE virtual
    WBackendNetTrack extractTrack(const QByteArray       & data,
                                  const WBackendNetQuery & query) const; /* {} */

    Q_INVOKABLE virtual
    WBackendNetPlaylist extractPlaylist(const QByteArray       & data,
                                        const WBackendNetQuery & query) const; /* {} */

    Q_INVOKABLE virtual
    WBackendNetFolder extractFolder(const QByteArray       & data,
                                    const WBackendNetQuery & query) const; /* {} */

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE virtual void queryFailed(const WBackendNetQuery & query); /* {} */

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE virtual void applySource(const WBackendNetQuery  & query,
                                         const WBackendNetSource & source); /* {} */

    Q_INVOKABLE virtual void applyTrack(const WBackendNetQuery & query,
                                        const WBackendNetTrack & track); /* {} */

    Q_INVOKABLE virtual void applyPlaylist(const WBackendNetQuery    & query,
                                           const WBackendNetPlaylist & playlist); /* {} */

    Q_INVOKABLE virtual void applyFolder(const WBackendNetQuery  & query,
                                         const WBackendNetFolder & folder); /* {} */

public: // Properties
    QString id   () const;
    QString title() const;

private:
    W_DECLARE_PRIVATE(WBackendNet)

    Q_PRIVATE_SLOT(d_func(), void onLoadSource  (WNetReplySource   *))
    Q_PRIVATE_SLOT(d_func(), void onLoadTrack   (WNetReplyTrack    *))
    Q_PRIVATE_SLOT(d_func(), void onLoadPlaylist(WNetReplyPlaylist *))
    Q_PRIVATE_SLOT(d_func(), void onLoadFolder  (WNetReplyFolder   *))
};

#include <private/WBackendNet_p>

#endif // SK_NO_BACKENDNET
#endif // WBACKENDNET_H
