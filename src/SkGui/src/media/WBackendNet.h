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
        //TypeWeb,
        TypeTorrent
    };

    enum Target
    {
        TargetDefault,
        TargetHtml,
        TargetFolder,
        TargetFile,
        TargetItem
    };

public:
    explicit WBackendNetQuery(const QString & url = QString());

    explicit WBackendNetQuery(Type type, const QString & url = QString());

public: // Interface
    bool isValid() const;

private: // Functions
    void init(Type type, const QString & url);

public: // Operators
    WBackendNetQuery & operator=(const WBackendNetQuery & other);

public: // Variables
    Type   type;
    Target target;

    QString backend;

    QString url;
    QString urlRedirect;

    int id;

    int indexNext;
    int indexReload;

    QNetworkRequest::Priority priority;

    WAbstractBackend::SourceMode mode;

    QVariant data;

    QString header;

    bool clearItems;
    bool cookies;
    bool skipError;

    int maxHost;
    int delay;
    int timeout;
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
// WBackendNetBase
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetBase
{
public:
    WBackendNetBase();

public: // Variables
    bool valid;

    QByteArray cache;

    QVariant backup;

    bool reload;

    QList<WBackendNetQuery> nextQueries;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetSource
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetSource : public WBackendNetBase
{
public:
    WBackendNetSource();

public: // Variables
    QHash<WAbstractBackend::Quality, QString> medias;
    QHash<WAbstractBackend::Quality, QString> audios;

    QDateTime expiry;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetTrack
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetTrack : public WBackendNetBase
{
public:
    WBackendNetTrack();

public: // Variables
    WTrack track;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetPlaylist
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetPlaylist : public WBackendNetBase
{
public:
    WBackendNetPlaylist();

public: // Variables
    QString title;
    QString cover;

    QList<WTrack> tracks;

    int currentIndex;

    bool clearDuplicate;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetFolder
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetFolder : public WBackendNetBase
{
public:
    WBackendNetFolder();

public: // Variables
    QString title;
    QString cover;

    QList<WLibraryFolderItem> items;

    int currentIndex;

    bool clearDuplicate;
    bool scanItems;
};

//-------------------------------------------------------------------------------------------------
// WBackendNetItem
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WBackendNetItem : public WBackendNetBase
{
public:
    WBackendNetItem();

public: // Variables
    QByteArray data;

    QString extension;
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
    friend class WBackendNetPrivate;
    friend class WBackendNetInterface;
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
    friend class WBackendNetPrivate;
    friend class WBackendNetInterface;
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
    friend class WBackendNetPrivate;
    friend class WBackendNetInterface;
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
    friend class WBackendNetPrivate;
    friend class WBackendNetInterface;
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
    friend class WBackendNetPrivate;
    friend class WBackendNetInterface;
};

//-------------------------------------------------------------------------------------------------
// WNetReplyItem
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WNetReplyItem : public WBackendNetReply
{
    Q_OBJECT

public:
    WNetReplyItem(QIODevice * device, const WBackendNetQuery & query);

signals:
    void loaded(QIODevice * device, const WBackendNetItem & item);

private:
    friend class WBackendNetPrivate;
    friend class WBackendNetInterface;
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
    Q_INVOKABLE WLibraryItem::Type getPlaylistType(const QString & url) const;
    Q_INVOKABLE QString            getPlaylistId  (const QString & url) const;

    Q_INVOKABLE QString getSourceTrack   (const QString & url) const;
    Q_INVOKABLE QString getSourcePlaylist(const QString & url) const;

    Q_INVOKABLE bool checkQuery(const QString & url) const;

    Q_INVOKABLE WBackendNetQuery extractQuery(const QString & url) const;

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

    Q_INVOKABLE void loadItem(QIODevice              * device,
                              const WBackendNetQuery & query,
                              QObject                * receiver,
                              const char             * method);

    Q_INVOKABLE bool tryDelete();

public: // Abstract interface
    Q_INVOKABLE virtual QString getId   () const = 0;
    Q_INVOKABLE virtual QString getTitle() const = 0;

public: // Virtual interface
    Q_INVOKABLE virtual void reload(); // {}

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE virtual bool hasSearch() const; // {}

    Q_INVOKABLE virtual bool isSearchEngine() const; // {}
    Q_INVOKABLE virtual bool isSearchCover () const; // {}

    Q_INVOKABLE virtual QString validate() const; // {}

    Q_INVOKABLE virtual bool checkCover(const QString & label, const QString & q) const; // {}

    Q_INVOKABLE virtual QString getHost () const; // {}
    Q_INVOKABLE virtual QString getCover() const; // {}

    Q_INVOKABLE virtual QList<WLibraryFolderItem> getLibraryItems() const; // {}

    Q_INVOKABLE virtual QString getTrackId(const QString & url) const; // {}

    Q_INVOKABLE virtual WAbstractBackend::Output getTrackOutput(const QString & url) const; // {}

    Q_INVOKABLE virtual WBackendNetPlaylistInfo getPlaylistInfo(const QString & url) const; // {}

    Q_INVOKABLE virtual QString getUrlTrack(const QString & id) const; // {}

    Q_INVOKABLE virtual QString getUrlPlaylist(const WBackendNetPlaylistInfo & info) const; // {}

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE virtual WBackendNetQuery getQuerySource  (const QString & url) const; // {}
    Q_INVOKABLE virtual WBackendNetQuery getQueryTrack   (const QString & url) const; // {}
    Q_INVOKABLE virtual WBackendNetQuery getQueryPlaylist(const QString & url) const; // {}
    Q_INVOKABLE virtual WBackendNetQuery getQueryFolder  (const QString & url) const; // {}
    Q_INVOKABLE virtual WBackendNetQuery getQueryItem    (const QString & url) const; // {}

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE virtual WBackendNetQuery createQuery(const QString & method,
                                                     const QString & label,
                                                     const QString & q) const; // {}

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE virtual
    WBackendNetSource extractSource(const QByteArray       & data,
                                    const WBackendNetQuery & query) const; // {}

    Q_INVOKABLE virtual
    WBackendNetTrack extractTrack(const QByteArray       & data,
                                  const WBackendNetQuery & query) const; // {}

    Q_INVOKABLE virtual
    WBackendNetPlaylist extractPlaylist(const QByteArray       & data,
                                        const WBackendNetQuery & query) const; // {}

    Q_INVOKABLE virtual
    WBackendNetFolder extractFolder(const QByteArray       & data,
                                    const WBackendNetQuery & query) const; // {}

    Q_INVOKABLE virtual
    WBackendNetItem extractItem(const QByteArray       & data,
                                const WBackendNetQuery & query) const; // {}

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE virtual void queryFailed(const WBackendNetQuery & query); // {}

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE virtual void applySource(const WBackendNetQuery  & query,
                                         const WBackendNetSource & source); // {}

    Q_INVOKABLE virtual void applyTrack(const WBackendNetQuery & query,
                                        const WBackendNetTrack & track); // {}

    Q_INVOKABLE virtual void applyPlaylist(const WBackendNetQuery    & query,
                                           const WBackendNetPlaylist & playlist); // {}

    Q_INVOKABLE virtual void applyFolder(const WBackendNetQuery  & query,
                                         const WBackendNetFolder & folder); // {}

    Q_INVOKABLE virtual void applyItem(const WBackendNetQuery & query,
                                       const WBackendNetItem  & item); // {}

public: // Properties
    QString id   () const;
    QString title() const;

private:
    W_DECLARE_PRIVATE(WBackendNet)

    friend class WBackendLoader;
    friend class WBackendLoaderPrivate;
    friend class WBackendLoaderCache;
};

#include <private/WBackendNet_p>

#endif // SK_NO_BACKENDNET
#endif // WBACKENDNET_H
