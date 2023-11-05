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

#ifndef WLOADERPLAYLIST_P_H
#define WLOADERPLAYLIST_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Sk includes
#ifdef QT_NEW
#include <WList>
#endif

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_LOADERPLAYLIST

// Forward declarations
class WLibraryItem;

class SK_GUI_EXPORT WLoaderPlaylistPrivate : public WPrivate
{
public:
    WLoaderPlaylistPrivate(WLoaderPlaylist * p);

    void init(WLibraryFolder * folder, int id);

public: // Functions
    void clearNodes();

    WPlaylist * getPlaylist();

    void setItem(WLibraryItem * item);

public: // Slots
    void onCurrentIdChanged();

    void onLoaded();

    void onQueryCompleted();

public: // Variables
    WLibraryFolder * folder;

    int id;

    WLibraryItem * item;

    bool running;
    bool active;

    QStringList sources;

#ifdef QT_OLD
    QList<WLoaderPlaylistNode> nodes;
#else
    // NOTE: We want to ensure that data pointers are updated on a move operation.
    WList<WLoaderPlaylistNode> nodes;
#endif

    QList<WPlaylist *> playlists;

    QHash<WPlaylist *, WLoaderPlaylistNode *> jobs;

protected:
    W_DECLARE_PUBLIC(WLoaderPlaylist)
};

#endif // SK_NO_LOADERPLAYLIST
#endif // WLOADERPLAYLIST_P_H
