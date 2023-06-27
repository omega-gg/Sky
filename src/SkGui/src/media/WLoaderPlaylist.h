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

#ifndef WLOADERPLAYLIST_H
#define WLOADERPLAYLIST_H

// Qt includes
#include <QObject>

// Sk includes
#include <WBackendNet>

#ifndef SK_NO_LOADERPLAYLIST

// Forward declarations
class WLoaderPlaylistPrivate;
class WLibraryFolder;

//=================================================================================================
// WLoaderPlaylistNode
//=================================================================================================

struct WLoaderPlaylistNode
{
    QString source;

    WBackendNetQuery query;

    QList<QStringList> urls;
};

//=================================================================================================
// WLoaderPlaylist
//=================================================================================================

class SK_GUI_EXPORT WLoaderPlaylist : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(Action)

    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)

public: // Enums
    enum Action
    {
        Insert,
        Move,
        Remove
    };

public:
    WLoaderPlaylist(WLibraryFolder * folder, int id);
protected:
    WLoaderPlaylist(WLoaderPlaylistPrivate * p, WLibraryFolder * folder, int id);

public: // Interface
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop ();

    Q_INVOKABLE void reload();

protected: // Abstract functions
    virtual void onStart() = 0;
    virtual void onStop () = 0;

    virtual void onClear() = 0;

protected: // Functions
    void clearTracks();

    void setQueryLoading(bool loading);

signals:
    void runningChanged();

public: // Properties
    bool isRunning() const;
    void setRunning(bool running);

private:
    W_DECLARE_PRIVATE(WLoaderPlaylist)

    Q_PRIVATE_SLOT(d_func(), void onCurrentIdChanged())

    Q_PRIVATE_SLOT(d_func(), void onLoaded())
};

//=================================================================================================
// WLoaderPlaylistAction
//=================================================================================================

struct WLoaderPlaylistAction
{
public:
    WLoaderPlaylistAction(WLoaderPlaylist::Action type)
    {
        this->type = type;

        index = -1;
    }

public: // Variables
    WLoaderPlaylist::Action type;

    int index;

    QString url;

    WBackendNetQuery query;
};

//=================================================================================================
// WLoaderPlaylistData
//=================================================================================================

struct WLoaderPlaylistData
{
    QStringList sources;

    QList<WLoaderPlaylistAction> actions;
};

//=================================================================================================
// WLoaderPlaylistReply
//=================================================================================================

// NOTE: This function is supposed to be instantiated as a pointer object and deletes itself after
//       the extract function has been processed.
class WLoaderPlaylistReply : public QObject
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extract(const QStringList & urls, const QStringList & sources, int maximum);

protected: // Functions
    virtual WBackendNetQuery getQuery(const QString & url, int index) const; // {}

signals:
    void loaded(const WLoaderPlaylistData & data);
};

#include <private/WLoaderPlaylist_p>

#endif // SK_NO_LOADERPLAYLIST
#endif // WLOADERPLAYLIST_H
