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

#ifndef WLOADERHUB_P_H
#define WLOADERHUB_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QMetaMethod>
#ifdef QT_4
#include <QStringList>
#endif

// Private includes
#include <private/WLoaderPlaylist_p>

#ifndef SK_NO_LOADERHUB

class SK_GUI_EXPORT WLoaderHubPrivate : public WLoaderPlaylistPrivate
{
public:
    WLoaderHubPrivate(WLoaderHub * p);

    void init();

public: // Functions
    void updateSources();

    void processQueries();
    void clearQueries  ();

    QStringList getSourcesInput () const;
    QStringList getSourcesOutput() const;

    QHash<QString, const WTrack *> getTracks(WPlaylist * playlist, QStringList * urls) const;

public: // Slots
    void onPlaylistUpdated  ();
    void onPlaylistDestroyed();

    void onLoaded(const WLoaderPlaylistData & data);

public: // Variables
    WPlaylist * history;

    WLoaderPlaylistReply * reply;

    QMetaMethod method;

protected:
    W_DECLARE_PUBLIC(WLoaderHub)
};

#endif // SK_NO_LOADERHUB
#endif // WLOADERHUB_P_H
