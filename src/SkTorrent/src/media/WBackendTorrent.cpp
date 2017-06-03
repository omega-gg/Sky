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
static const QChar BACKENDTORRENT_LIST    = 'l';

static const QChar BACKENDTORRENT_END = 'e';

//=================================================================================================
// WBackendTorrentItem
//=================================================================================================

struct WBackendTorrentItem
{
    int id;

    QString path;
    QString name;
};

//-------------------------------------------------------------------------------------------------

inline bool sort(const WBackendTorrentItem & itemA, const WBackendTorrentItem & itemB)
{
    return itemA.name < itemB.name;
}

//=================================================================================================
// WBackendTorrentPrivate
//=================================================================================================

#include <private/WBackendNet_p>

class SK_TORRENT_EXPORT WBackendTorrentPrivate : public WBackendNetPrivate
{
public:
    WBackendTorrentPrivate(WBackendTorrent * p);

    void init();

public: // Functions
    QString                    extractName (const QString & data) const;
    QList<WBackendTorrentItem> extractItems(const QString & data) const;

    QString extractString(const QString & data, int from) const;

    void extractItem(WBackendTorrentItem * item, const QString & data, int from) const;

    QList<WBackendTorrentItem> getFolder(QList<WBackendTorrentItem> * items) const;

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

QList<WBackendTorrentItem> WBackendTorrentPrivate::extractItems(const QString & data) const
{
    QList<WBackendTorrentItem> list;

    int index = data.indexOf("4:path");

    int id = 1;

    while (index != -1)
    {
        index += 6;

        WBackendTorrentItem item;

        item.id = id;

        QChar character = data.at(index);

        if (character == BACKENDTORRENT_LIST)
        {
            index++;

            extractItem(&item, data, index);

            if (item.name.isEmpty() == false)
            {
                list.append(item);
            }
        }
        else
        {
            QString name = extractString(data, index);

            if (name.isEmpty() == false)
            {
                item.name = name;

                list.append(item);
            }
        }

        index = data.indexOf("4:path", index);

        id++;
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

QString WBackendTorrentPrivate::extractString(const QString & data, int from) const
{
    int index = data.indexOf(BACKENDTORRENT_STRING, from);

    if (index == -1) return QString();

    int length = data.mid(from, index - from).toInt();

    if (length == 0) return QString();

    return data.mid(index + 1, length);
}

//-------------------------------------------------------------------------------------------------

void WBackendTorrentPrivate::extractItem(WBackendTorrentItem * item, const QString & data,
                                                                     int             from) const
{
    int index = data.indexOf(BACKENDTORRENT_STRING, from);

    if (index == -1) return;

    int length = data.mid(from, index - from).toInt();

    if (length == 0) return;

    index++;

    QString string = data.mid(index, length);

    from = index + length;

    if (from < data.length())
    {
        if (data.at(from) != 'e')
        {
            item->path.append(string);

            extractItem(item, data, from);
        }
        else item->name = string;
    }
}

//-------------------------------------------------------------------------------------------------

QList<WBackendTorrentItem>
WBackendTorrentPrivate::getFolder(QList<WBackendTorrentItem> * items) const
{
    QList<WBackendTorrentItem> list;

    WBackendTorrentItem item = items->takeAt(0);

    QString path = item.path;

    list.append(item);

    int index = 0;

    while (index < items->length())
    {
        if (items->at(index).path == path)
        {
            item = items->takeAt(index);

            list.append(item);
        }
        else index++;
    }

    return list;
}

//=================================================================================================
// WBackendTorrent
//=================================================================================================

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
WAbstractBackend::Output WBackendTorrent::getTrackOutput(const QUrl & url) const
{
    QString fragment = url.fragment();

    int index = fragment.indexOf('.');

    if (index != -1)
    {
        fragment = fragment.mid(index + 1);

        if (WControllerPlaylist::extensionIsAudio(fragment))
        {
            return WAbstractBackend::OutputAudio;
        }
    }

    return WAbstractBackend::OutputMedia;
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
WBackendNetQuery WBackendTorrent::createQuery(const QString & method,
                                              const QString & label, const QString & q) const
{
    WBackendNetQuery backendQuery;

    if (method == "related" && label == "tracks")
    {
        backendQuery.url = q;
        backendQuery.id  = 1;
    }

    return backendQuery;
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

    QList<WBackendTorrentItem> items = d->extractItems(content);

    if (items.isEmpty())
    {
        WBackendTorrentItem item;

        item.id   = 1;
        item.name = name;

        items.append(item);
    }

    QList<WTrackNet> tracks;

    QString source = WControllerNetwork::removeUrlFragment(query.url);

    while (items.isEmpty() == false)
    {
        QList<WBackendTorrentItem> list = d->getFolder(&items);

        qSort(list.begin(), list.end(), sort);

        foreach (const WBackendTorrentItem & item, list)
        {
            QString title = item.name;

            QString extension = WControllerNetwork::extractUrlExtension(title);

            if (WControllerPlaylist::extensionIsMedia(extension))
            {
                WTrackNet track(source + '#' + QString::number(item.id) + '.' + extension);

                track.setTitle(title);

                track.setAuthor(name);
                track.setFeed  (source);

                tracks.append(track);
            }
        }
    }

    reply.title = name;

    reply.tracks = tracks;

    reply.cache = data;

    if (query.id == 1)
    {
        reply.clearDuplicate = true;
    }

    return reply;
}

#endif // SK_NO_BACKENDTORRENT
