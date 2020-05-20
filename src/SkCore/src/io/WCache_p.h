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

#ifndef WCACHE_P_H
#define WCACHE_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QEvent>
#include <QTimer>
#include <QHash>
#ifdef QT_4
#include <QStringList>
#endif

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_CACHE

// Forward declarations
class  WCacheThread;
struct WCacheFiles;

class SK_CORE_EXPORT WCachePrivate : public WPrivate
{
public: // Enums
    enum EventType
    {
        EventLoaded = QEvent::User,
        EventAdded,
        EventFailed,
        EventRemoved,
        EventCleared
    };

public:
    WCachePrivate(WCache * p);

    /* virtual */ ~WCachePrivate();

    void init(const QString & path, qint64 sizeMax);

public: // Functions
    void get(WCacheFile * file, const QString & url);

    void write(WCacheFile * file, const QString & url,
                                  const QString & extension, const QByteArray & array);

    void pop(const QString & url);

    void clearFile(WCacheFile * file);

    void setEmpty(bool empty);

public: // Slots
    void onProgress(qint64 bytesReceived, qint64 bytesTotal);

    void onLoaded(WCacheFile * file);

    void onPop();

public: // Variables
    WCacheThread * thread;

    QString path;

    qint64 sizeMax;

    bool loaded;
    bool empty;

    QHash<QString, WCacheFiles *> urlsPending;
    QHash<QString, WCacheFile  *> urlsLoad;

    QHash<QString, QString> urls;

    QStringList urlsPop;

    QTimer timer;

    int maxJobs;

protected:
    W_DECLARE_PUBLIC(WCache)
};

#endif // SK_NO_CACHE
#endif // WCACHE_P_H
