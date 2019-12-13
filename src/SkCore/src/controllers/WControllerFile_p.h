//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WCONTROLLERFILE_P_H
#define WCONTROLLERFILE_P_H

// Qt includes
#include <QList>
#include <QTimer>

// Private includes
#include <private/WController_p>

#ifndef SK_NO_CONTROLLERFILE

// Forward declarations
namespace QtLP_Private
{
    class QtLockedFile;
};
class WFileWatcher;
class WLocalObject;
class WThreadActions;

class SK_CORE_EXPORT WControllerFilePrivate : public WControllerPrivate
{
public:
    WControllerFilePrivate(WControllerFile * p);

    /* virtual */ ~WControllerFilePrivate();

    void init();

public: // Functions
    void createThreadWrite();
    void createThreadRead ();

    void registerFileWatcher  (WFileWatcher * watcher);
    void unregisterFileWatcher(WFileWatcher * watcher);

    void registerLocalObject  (WLocalObject * object);
    void unregisterLocalObject(WLocalObject * object);

    bool isLoading();

public: // Static functions
    static bool tryOpen(const QtLP_Private::QtLockedFile & file);

    static void deleteDir(QDir & dir, bool recursive);

public: // Slots
    void onCheckWatchers();

public: // Variables
    WThreadActions * threadWrite;
    WThreadActions * threadRead;

    QString pathStorage;

    WCache * cache;

    QList<WLocalObject *> objects;
    QList<WFileWatcher *> watchers;

    QTimer timer;

protected:
    W_DECLARE_PUBLIC(WControllerFile)
};

#endif // SK_NO_CONTROLLERFILE
#endif // WCONTROLLERFILE_P_H
