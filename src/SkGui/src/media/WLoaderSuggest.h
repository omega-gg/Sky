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

#ifndef WLOADERSUGGEST_H
#define WLOADERSUGGEST_H

// Sk includes
#include <WLoaderPlaylist>

#ifndef SK_NO_LOADERSUGGEST

// Forward declarations
class WLoaderSuggestPrivate;
class WPlaylist;

class SK_GUI_EXPORT WLoaderSuggest : public WLoaderPlaylist
{
    Q_OBJECT

    Q_PROPERTY(WPlaylist * history READ history WRITE setHistory NOTIFY historyChanged)

public:
    WLoaderSuggest(WLibraryFolder * folder, int id);

protected: // WLoaderPlaylist implementation
    /* virtual */ void onStart();
    /* virtual */ void onStop ();

    /* virtual */ void onApplyPlaylist(WLoaderPlaylistNode * node, WPlaylist * playlist);

signals:
    void historyChanged();

public: // Properties
    WPlaylist * history() const;
    void        setHistory(WPlaylist * history);

private:
    W_DECLARE_PRIVATE(WLoaderSuggest)

    Q_PRIVATE_SLOT(d_func(), void onPlaylistUpdated  ())
    Q_PRIVATE_SLOT(d_func(), void onPlaylistDestroyed())

    Q_PRIVATE_SLOT(d_func(), void onLoaded(const WLoaderPlaylistData &))
};

#include <private/WLoaderSuggest_p>

#endif // SK_NO_LOADERSUGGEST
#endif // WLOADERSUGGEST_H
