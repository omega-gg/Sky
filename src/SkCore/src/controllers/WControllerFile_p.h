//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

#ifndef WCONTROLLERFILE_P_H
#define WCONTROLLERFILE_P_H

// Qt includes
#include <QList>
#include <QTimer>
#include <QMetaMethod>

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

    void addLog(const QString & message);

    bool isLoading() const;

public: // Static functions
#ifdef QT_4
    static void messageHandler(QtMsgType type, const char * message);
#else
    static void messageHandler(QtMsgType type, const QMessageLogContext & context,
                                               const QString            & message);
#endif

    static bool tryOpen(const QtLP_Private::QtLockedFile & file);

    static void deleteDir(QDir & dir, bool recursive);

public: // Slots
    void onLog(const QString & message);

    void onWriteLog();

    void onCheckWatchers();

public: // Variables
    WThreadActions * threadWrite;
    WThreadActions * threadRead;

    QString log;
    QString logBuffer;

    QString pathStorage;
    QString pathLog;

    WCache * cache;

    QList<WLocalObject *> objects;
    QList<WFileWatcher *> watchers;

    QMetaMethod method;

    QTimer timerLog;
    QTimer timerWatcher;

protected:
    W_DECLARE_PUBLIC(WControllerFile)
};

#endif // SK_NO_CONTROLLERFILE
#endif // WCONTROLLERFILE_P_H
