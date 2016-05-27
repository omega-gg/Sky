//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkTorrent module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendBencode.h"

#ifndef SK_NO_BACKENDBENCODE

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WControllerPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const QChar BACKENDBENCODE_INTEGER = 'i';
static const QChar BACKENDBENCODE_STRING  = ':';

static const QChar BACKENDBENCODE_END = 'e';

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_TORRENT_EXPORT WBackendBencodePrivate : public WBackendNetPrivate
{
public:
    WBackendBencodePrivate(WBackendBencode * p);

    void init();

public: // Functions
    QString     extractName (const QString & data) const;
    QStringList extractPaths(const QString & data) const;

    QString extractString    (const QString & data, int from) const;
    QString extractNextString(const QString & data, int from) const;

protected:
    W_DECLARE_PUBLIC(WBackendBencode)
};

//-------------------------------------------------------------------------------------------------

WBackendBencodePrivate::WBackendBencodePrivate(WBackendBencode * p) : WBackendNetPrivate(p) {}

void WBackendBencodePrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions

QString WBackendBencodePrivate::extractName(const QString & data) const
{
    int index = data.indexOf("4:name");

    if (index == -1)
    {
         return QString();
    }
    else return extractString(data, index + 6);
}

QStringList WBackendBencodePrivate::extractPaths(const QString & data) const
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

QString WBackendBencodePrivate::extractString(const QString & data, int from) const
{
    int index = data.indexOf(BACKENDBENCODE_STRING, from);

    if (index == -1) return QString();

    int number = data.mid(from, index - from).toInt();

    return data.mid(index + 1, number);
}

QString WBackendBencodePrivate::extractNextString(const QString & data, int from) const
{
    QChar character = data.at(from);

    while (character.isNumber() == false)
    {
        if (character == BACKENDBENCODE_INTEGER)
        {
            from = data.indexOf(BACKENDBENCODE_END, from);

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

WBackendBencode::WBackendBencode() : WBackendNet(new WBackendBencodePrivate(this))
{
    Q_D(WBackendBencode); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendBencode::getId() const
{
    return "bencode";
}

/* Q_INVOKABLE virtual */ QString WBackendBencode::getTitle() const
{
    return "Bencode";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendBencode::checkValidUrl(const QUrl & url) const
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
QUrl WBackendBencode::getUrlPlaylist(const WBackendNetPlaylistInfo & info) const
{
    return info.id;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendBencode::getQueryPlaylist(const QUrl & url) const
{
    return WBackendNetQuery(url);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetPlaylistInfo WBackendBencode::getPlaylistInfo(const QUrl & url) const
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
WBackendNetPlaylist WBackendBencode::extractPlaylist(const QByteArray       & data,
                                                     const WBackendNetQuery &) const
{
    Q_D(const WBackendBencode);

    WBackendNetPlaylist reply;

    QString content = Sk::readUtf8(data);

    QString name = d->extractName(content);

    QStringList paths = d->extractPaths(content);

    if (paths.isEmpty())
    {
        paths.append(name);
    }

    reply.title = name;

    foreach (const QString & path, paths)
    {
        if (WControllerPlaylist::urlIsVideo(path) == false) continue;

        WTrackNet track(path);

        track.setTitle(path);

        reply.tracks.append(track);
    }

    return reply;
}

#endif // SK_NO_BACKENDBENCODE
