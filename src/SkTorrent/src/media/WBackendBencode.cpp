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

//-------------------------------------------------------------------------------------------------
// Static variables

static const QChar BACKENDBENCODE_INTEGER    = 'i';
static const QChar BACKENDBENCODE_STRING     = ':';
static const QChar BACKENDBENCODE_LIST       = 'l';
static const QChar BACKENDBENCODE_DICTIONARY = 'd';

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
    int extractValue(const QString & data, int from) const;

    int extractInteger   (const QString & data, int from) const;
    int extractString    (const QString & data, int from) const;
    int extractList      (const QString & data, int from) const;
    int extractDictionary(const QString & data, int from) const;

protected:
    W_DECLARE_PUBLIC(WBackendBencode)
};

//-------------------------------------------------------------------------------------------------

WBackendBencodePrivate::WBackendBencodePrivate(WBackendBencode * p) : WBackendNetPrivate(p) {}

void WBackendBencodePrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions

int WBackendBencodePrivate::extractValue(const QString & data, int from) const
{
    QChar character = data.at(from);

    if (character == BACKENDBENCODE_INTEGER)
    {
        return extractInteger(data, from + 1);
    }
    else if (character == BACKENDBENCODE_LIST)
    {
        return extractList(data, from + 1);
    }
    else if (character == BACKENDBENCODE_DICTIONARY)
    {
        return extractDictionary(data, from + 1);
    }
    else if (character.isNumber())
    {
        return extractString(data, from);
    }
    else return data.length();
}

//-------------------------------------------------------------------------------------------------

int WBackendBencodePrivate::extractInteger(const QString & data, int from) const
{
    int index = data.indexOf(BACKENDBENCODE_END, from);

    if (index == -1)
    {
        return from + 1;
    }

    int number = data.mid(from, index - from).toInt();

    qDebug("Integer: %d", number);

    return index + 1;
}

int WBackendBencodePrivate::extractString(const QString & data, int from) const
{
    int index = data.indexOf(BACKENDBENCODE_STRING, from);

    if (index == -1)
    {
        return from + 1;
    }

    int number = data.mid(from, index - from).toInt();

    index++;

    QString string = data.mid(index, number);

    qDebug("String: %s", string.C_STR);

    return index + number;
}

int WBackendBencodePrivate::extractList(const QString & data, int from) const
{
    qDebug("List");

    while (from < data.length() && data.at(from) != BACKENDBENCODE_END)
    {
        from = extractValue(data, from);
    }

    return from + 1;
}

int WBackendBencodePrivate::extractDictionary(const QString & data, int from) const
{
    qDebug("Dictionnary");

    while (from < data.length() && data.at(from) != BACKENDBENCODE_END)
    {
        from = extractValue(data, from);
    }

    return from + 1;
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

    int index = 0;

    while (index < content.length())
    {
        index = d->extractValue(data, index);
    }

    return reply;
}

#endif // SK_NO_BACKENDBENCODE
