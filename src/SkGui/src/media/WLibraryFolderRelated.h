//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WLIBRARYFOLDERRELATED_H
#define WLIBRARYFOLDERRELATED_H

// Sk includes
#include <QVariantMap>
#include <WLibraryFolder>

#ifndef SK_NO_LIBRARYFOLDERRELATED

// Forward declarations
class WLibraryFolderRelatedPrivate;
class WTrack;

class SK_GUI_EXPORT WLibraryFolderRelated : public WLibraryFolder
{
    Q_OBJECT

    Q_PROPERTY(bool hasPreviousPlaylist READ hasPreviousPlaylist NOTIFY playlistChanged)
    Q_PROPERTY(bool hasNextPlaylist     READ hasNextPlaylist     NOTIFY playlistChanged)

public:
    explicit WLibraryFolderRelated(WLibraryFolder * parent = NULL);

public: // Interface
    Q_INVOKABLE void loadTracks(const WTrack      & track);
    Q_INVOKABLE void loadTracks(const QVariantMap & data);

    Q_INVOKABLE void setPreviousPlaylist();
    Q_INVOKABLE void setNextPlaylist    ();

signals:
    void playlistChanged();

public: // Properties
    bool hasPreviousPlaylist() const;
    bool hasNextPlaylist    () const;

private:
    W_DECLARE_PRIVATE(WLibraryFolderRelated)
};

#include <private/WLibraryFolderRelated_p>

#endif // SK_NO_LIBRARYFOLDERRELATED
#endif // WLIBRARYFOLDERRELATED_H
