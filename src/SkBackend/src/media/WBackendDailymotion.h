//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WBACKENDDAILYMOTION_H
#define WBACKENDDAILYMOTION_H

// Sk includes
#include <WBackendNet>

#ifndef SK_NO_BACKENDDAILYMOTION

// Forward declarations
class WBackendDailymotionPrivate;

class SK_BACKEND_EXPORT WBackendDailymotion : public WBackendNet
{
    Q_OBJECT

public:
    WBackendDailymotion();

public: // WBackendNet implementation
    /* Q_INVOKABLE virtual */ QList<WLibraryFolderItem> getLibraryItems() const;

    /* Q_INVOKABLE virtual */ QString getId   () const;
    /* Q_INVOKABLE virtual */ QString getTitle() const;

    /* Q_INVOKABLE virtual */ bool checkValidUrl(const QUrl & url) const;

public: // WBackendNet reimplementation
    /* Q_INVOKABLE virtual */ QString getHost() const;

    /* Q_INVOKABLE virtual */ QString getTrackId(const QUrl & url) const;

    /* Q_INVOKABLE virtual */ WBackendNetPlaylistInfo getPlaylistInfo(const QUrl & url) const;

    /* Q_INVOKABLE virtual */ QUrl getUrlTrack(const QString & id) const;

    /* Q_INVOKABLE virtual */ QUrl getUrlPlaylist(const WBackendNetPlaylistInfo & info) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */ WBackendNetQuery getQuerySource  (const QUrl & url) const;
    /* Q_INVOKABLE virtual */ WBackendNetQuery getQueryTrack   (const QUrl & url) const;
    /* Q_INVOKABLE virtual */ WBackendNetQuery getQueryPlaylist(const QUrl & url) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */
    WBackendNetQuery createQuery(const QString & method,
                                 const QString & label, const QString & q) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */
    WBackendNetSource extractSource(const QByteArray       & data,
                                    const WBackendNetQuery & query) const;

    /* Q_INVOKABLE virtual */
    WBackendNetTrack extractTrack(const QByteArray       & data,
                                  const WBackendNetQuery & query) const;

    /* Q_INVOKABLE virtual */
    WBackendNetPlaylist extractPlaylist(const QByteArray       & data,
                                        const WBackendNetQuery & query) const;

    /* Q_INVOKABLE virtual */
    WBackendNetFolder extractFolder(const QByteArray       & data,
                                    const WBackendNetQuery & query) const;

private:
    W_DECLARE_PRIVATE(WBackendDailymotion)
};

#endif // SK_NO_BACKENDDAILYMOTION
#endif // WBACKENDDAILYMOTION_H
