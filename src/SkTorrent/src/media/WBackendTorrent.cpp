//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkTorrent module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendTorrent.h"

#ifndef SK_NO_BACKENDTORRENT

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WControllerPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const QChar BACKENDTORRENT_INTEGER = 'i';
static const QChar BACKENDTORRENT_STRING  = ':';

static const QChar BACKENDTORRENT_END = 'e';

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_TORRENT_EXPORT WBackendTorrentPrivate : public WBackendNetPrivate
{
public:
    WBackendTorrentPrivate(WBackendTorrent * p);

    void init();

public: // Functions
    QString     extractName (const QString & data) const;
    QStringList extractPaths(const QString & data) const;

    QString extractString    (const QString & data, int from) const;
    QString extractNextString(const QString & data, int from) const;

protected:
    W_DECLARE_PUBLIC(WBackendTorrent)
};

//-------------------------------------------------------------------------------------------------

WBackendTorrentPrivate::WBackendTorrentPrivate(WBackendTorrent * p) : WBackendNetPrivate(p) {}

void WBackendTorrentPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions

QString WBackendTorrentPrivate::extractName(const QString & data) const
{
    int index = data.indexOf("4:name");

    if (index == -1)
    {
         return QString();
    }
    else return extractString(data, index + 6);
}

QStringList WBackendTorrentPrivate::extractPaths(const QString & data) const
{
    QStringList list;

    int index = data.indexOf("4:path");

    while (index != -1)
    {
        index += 6;

        QString path = extractNextString(data, index);

        list.append(path);

        index = data.indexOf("4:path", index);
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

QString WBackendTorrentPrivate::extractString(const QString & data, int from) const
{
    int index = data.indexOf(BACKENDTORRENT_STRING, from);

    if (index == -1) return QString();

    int number = data.mid(from, index - from).toInt();

    return data.mid(index + 1, number);
}

QString WBackendTorrentPrivate::extractNextString(const QString & data, int from) const
{
    QChar character = data.at(from);

    while (character.isNumber() == false)
    {
        if (character == BACKENDTORRENT_INTEGER)
        {
            from = data.indexOf(BACKENDTORRENT_END, from);

            if (from == -1) return QString();
        }

        from++;

        character = data.at(from);
    }

    return extractString(data, from);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendTorrent::WBackendTorrent() : WBackendNet(new WBackendTorrentPrivate(this))
{
    Q_D(WBackendTorrent); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendTorrent::getId() const
{
    return "torrent";
}

/* Q_INVOKABLE virtual */ QString WBackendTorrent::getTitle() const
{
    return "Torrent";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendTorrent::checkValidUrl(const QUrl & url) const
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    if (extension == "torrent")
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QUrl WBackendTorrent::getUrlPlaylist(const WBackendNetPlaylistInfo & info) const
{
    return info.id;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendTorrent::getQuerySource(const QUrl &) const
{
    return WBackendNetQuery();
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendTorrent::getQueryPlaylist(const QUrl & url) const
{
    return WBackendNetQuery(url);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetPlaylistInfo WBackendTorrent::getPlaylistInfo(const QUrl & url) const
{
    QString extension = WControllerNetwork::extractUrlExtension(url);

    if (extension == "torrent")
    {
         return WBackendNetPlaylistInfo(WLibraryItem::PlaylistNet, url.toString());
    }
    else return WBackendNetPlaylistInfo();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetPlaylist WBackendTorrent::extractPlaylist(const QByteArray       & data,
                                                     const WBackendNetQuery & query) const
{
    Q_D(const WBackendTorrent);

    WBackendNetPlaylist reply;

    QString content = Sk::readUtf8(data);

    QString name = d->extractName(content);

    QStringList paths = d->extractPaths(content);

    if (paths.isEmpty())
    {
        paths.append(name);
    }

    reply.title = name;

    QString source = query.url.toString();

    int index = 1;

    foreach (const QString & path, paths)
    {
        if (WControllerPlaylist::urlIsMedia(path) == false) continue;

        WTrackNet track(source + '#' + QString::number(index));

        track.setTitle(path);

        reply.tracks.append(track);

        index++;
    }

    reply.cache = data;

    return reply;
}

#endif // SK_NO_BACKENDTORRENT
