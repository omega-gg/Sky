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

#ifndef WLOADERRECENT_H
#define WLOADERRECENT_H

// Sk includes
#include <WLoaderPlaylist>

#ifndef SK_NO_LOADERRECENT

// Forward declarations
class WLoaderRecentPrivate;

class SK_GUI_EXPORT WLoaderRecent : public WLoaderPlaylist
{
    Q_OBJECT

    Q_PROPERTY(WLibraryFolder * feeds READ feeds WRITE setFeeds NOTIFY feedsChanged)

public:
    WLoaderRecent(WLibraryFolder * folder, int id);

protected: // WLoaderPlaylist implementation
    /* virtual */ void onStart();
    /* virtual */ void onStop ();

    /* virtual */ void onClear();

signals:
    void feedsChanged();

public: // Properties
    WLibraryFolder * feeds() const;
    void             setFeeds(WLibraryFolder * feeds);

private:
    W_DECLARE_PRIVATE(WLoaderRecent)

    Q_PRIVATE_SLOT(d_func(), void onFolderUpdated  ());
    Q_PRIVATE_SLOT(d_func(), void onFolderDestroyed());
};

#include <private/WLoaderRecent_p>

#endif // SK_NO_LOADERRECENT
#endif // WLOADERRECENT_H
