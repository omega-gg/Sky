//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkBackend.

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

#ifndef WBACKENDUNIVERSAL_H
#define WBACKENDUNIVERSAL_H

// Sk includes
#include <WBackendNet>
#include <WControllerPlaylist>

#ifndef SK_NO_BACKENDUNIVERSAL

// Forward declarations
class WBackendUniversalPrivate;

class SK_BACKEND_EXPORT WBackendUniversal : public WBackendNet
{
    Q_OBJECT

    Q_PROPERTY(bool isLoaded READ isLoaded NOTIFY loaded)

protected:
    WBackendUniversal(const QString & id, const QString & source);

public: // WBackendNet implementation
    /* Q_INVOKABLE virtual */ QString getId   () const;
    /* Q_INVOKABLE virtual */ QString getTitle() const;

public: // WBackendNet reimplementation
    /* Q_INVOKABLE virtual */ void update();
    /* Q_INVOKABLE virtual */ void reload();

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */ bool hasSearch() const;

    /* Q_INVOKABLE virtual */ bool isSearchEngine() const;
    /* Q_INVOKABLE virtual */ bool isSearchCover () const;

    /* Q_INVOKABLE virtual */ QString validate() const;

    /* Q_INVOKABLE virtual */ bool checkCover(const QString & label, const QString & q) const;

    /* Q_INVOKABLE virtual */ QString getHost () const;
    /* Q_INVOKABLE virtual */ QString getCover() const;

    /* Q_INVOKABLE virtual */ WListFolderItem getLibraryItems() const;

    /* Q_INVOKABLE virtual */ WLibraryFolderItem getLibraryItem(const QString & id) const;

    /* Q_INVOKABLE virtual */ QString getTrackId(const QString & url) const;

    /* Q_INVOKABLE virtual */ WAbstractBackend::Output getTrackOutput(const QString & url) const;

    /* Q_INVOKABLE virtual */ WBackendNetPlaylistInfo getPlaylistInfo(const QString & url) const;

    /* Q_INVOKABLE virtual */ QString getUrlTrack(const QString & id) const;

    /* Q_INVOKABLE virtual */ QString getUrlPlaylist(const WBackendNetPlaylistInfo & info) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */ WBackendNetQuery getQuerySource  (const QString & url) const;
    /* Q_INVOKABLE virtual */ WBackendNetQuery getQueryTrack   (const QString & url) const;
    /* Q_INVOKABLE virtual */ WBackendNetQuery getQueryPlaylist(const QString & url) const;
    /* Q_INVOKABLE virtual */ WBackendNetQuery getQueryFolder  (const QString & url) const;
    /* Q_INVOKABLE virtual */ WBackendNetQuery getQueryItem    (const QString & url) const;

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

    /* Q_INVOKABLE virtual */
    WBackendNetItem extractItem(const QByteArray       & data,
                                const WBackendNetQuery & query) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */ void queryFailed(const WBackendNetQuery & query);

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */ void applySource(const WBackendNetQuery  & query,
                                               const WBackendNetSource & source);

    /* Q_INVOKABLE virtual */ void applyTrack(const WBackendNetQuery & query,
                                              const WBackendNetTrack & track);

    /* Q_INVOKABLE virtual */ void applyPlaylist(const WBackendNetQuery    & query,
                                                 const WBackendNetPlaylist & playlist);

    /* Q_INVOKABLE virtual */ void applyFolder(const WBackendNetQuery  & query,
                                               const WBackendNetFolder & folder);

    /* Q_INVOKABLE virtual */ void applyItem(const WBackendNetQuery & query,
                                             const WBackendNetItem  & item);

signals:
    void loaded ();
    void updated();

    void backendUpdated(const QString & id);

public: // Properties
    bool isLoaded() const;

private:
    W_DECLARE_PRIVATE(WBackendUniversal)

    Q_PRIVATE_SLOT(d_func(), void onLoad  ())
    Q_PRIVATE_SLOT(d_func(), void onUpdate())

    Q_PRIVATE_SLOT(d_func(), void onData(const WBackendUniversalData &))

    friend class WBackendIndex;
};

#include <private/WBackendUniversal_p>

#endif // SK_NO_BACKENDUNIVERSAL
#endif // WBACKENDUNIVERSAL_H
