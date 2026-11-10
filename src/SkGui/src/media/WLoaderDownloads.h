//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

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

#ifndef WLOADERDOWNLOADS_H
#define WLOADERDOWNLOADS_H

// Sk includes
#include <WLoaderPlaylist>

#ifndef SK_NO_LOADERDOWNLOADS

// Forward declarations
class WLoaderDownloadsPrivate;

// NOTE: This lists the VBML files of a folder, the most recent first.
class SK_GUI_EXPORT WLoaderDownloads : public WLoaderPlaylist
{
    Q_OBJECT

    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

public:
    WLoaderDownloads(WLibraryFolder * folder, int id);

protected: // WLoaderPlaylist implementation
    /* virtual */ void onStart();
    /* virtual */ void onStop ();

signals:
    void pathChanged();

public: // Properties
    QString path() const;
    void    setPath(const QString & path);

private:
    W_DECLARE_PRIVATE(WLoaderDownloads)

    Q_PRIVATE_SLOT(d_func(), void onLoaded(const WLoaderPlaylistData &))
};

#include <private/WLoaderDownloads_p>

#endif // SK_NO_LOADERDOWNLOADS
#endif // WLOADERDOWNLOADS_H
