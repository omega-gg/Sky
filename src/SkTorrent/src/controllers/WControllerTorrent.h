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

#ifndef WCONTROLLERTORRENT_H
#define WCONTROLLERTORRENT_H

// Qt includes
#include <QUrl>

// Sk includes
#include <WController>

#ifndef SK_NO_CONTROLLERTORRENT

// Forward declarations
class WControllerTorrentPrivate;
class WTorrentEngine;

// Defines
#define wControllerTorrent WControllerTorrent::instance()

//-------------------------------------------------------------------------------------------------
// WTorrentReply
//-------------------------------------------------------------------------------------------------

class SK_TORRENT_EXPORT WTorrentReply : public QObject
{
    Q_OBJECT

private:
    WTorrentReply(const QUrl & url, QObject * parent);

private: // Functions
    int extractIndex(const QUrl & url);

signals:
    void loaded(WTorrentReply * reply);

public: // Properties
    QUrl url() const;

    int index() const;

private: // Variables
    QUrl _url;

    int _index;

private:
    friend class WControllerTorrent;
    friend class WControllerTorrentPrivate;
};

//-------------------------------------------------------------------------------------------------
// WControllerTorrent
//-------------------------------------------------------------------------------------------------

class SK_TORRENT_EXPORT WControllerTorrent : public WController
{
    Q_OBJECT

    Q_PROPERTY(WTorrentEngine * engine READ engine CONSTANT)

    Q_PROPERTY(QString pathStorage READ pathStorage NOTIFY pathStorageChanged)

private:
    WControllerTorrent();

public: // Interface
    Q_INVOKABLE WTorrentReply * getTorrent(const QUrl & url, QObject * parent = NULL);

protected: // Initialize
    /* virtual */ void init();

signals:
    void pathStorageChanged();

public: // Properties
    WTorrentEngine * engine() const;

    QString pathStorage() const;

private:
    W_DECLARE_PRIVATE   (WControllerTorrent)
    W_DECLARE_CONTROLLER(WControllerTorrent)
};

#endif // SK_NO_CONTROLLERTORRENT
#endif // WCONTROLLERTORRENT_H
