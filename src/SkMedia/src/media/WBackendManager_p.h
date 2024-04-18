//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMedia.

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

#ifndef WBACKENDMANAGER_P_H
#define WBACKENDMANAGER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QElapsedTimer>
#include <QTimer>

// Sk includes
#include <WTrack>

// Private includes
#include <private/WAbstractBackend_p>

#ifndef SK_NO_BACKENDMANAGER

// Forward declarations
class WMediaReply;

//=================================================================================================
// WBackendManagerItem
//=================================================================================================

struct WBackendManagerItem
{
    WAbstractBackend * backend;
    WAbstractHook    * hook;
};

//=================================================================================================
// WBackendManager
//=================================================================================================

class SK_MEDIA_EXPORT WBackendManagerPrivate : public WAbstractBackendPrivate
{
public: // Enums
    enum Type { Track, MultiTrack, Channel, Interactive };

public:
    WBackendManagerPrivate(WBackendManager * p);

    /* virtual */ ~WBackendManagerPrivate();

    void init();

public: // Functions
    void loadSources  (bool play);
    void reloadSources(bool play);

    void loadMedia();

    void applySources(bool play);

    void applyEmpty();

    void loadSource(const QString & source, const QString & media, int currentTime);

    void pauseBackend();
    void stopBackend ();

    void applyDefault();

    bool applyNext(int currentTime);

    void updateLoading();

    void startClock();
    void stopClock ();

    void connectBackend   ();
    void disconnectBackend();

    void clearActive();

    void clearReply();
    void clearMedia();

    //void setBackend(WAbstractBackend * backendNew);

    void setBackendInterface(WBackendInterface * backendNew);

public: // Slots
    void onLoaded  ();
    void onReloaded();

    void onNext();

#ifndef SK_NO_QML
    void onPlayerChanged();
#endif

    void onRepeatChanged();

    void onState      ();
    void onStateLoad  ();
    void onLive       ();
    void onStarted    ();
    void onEnded      ();
    void onCurrentTime();
    void onDuration   ();
    void onProgress   ();
    void onOutput     ();
    void onQuality    ();
    void onVideos     ();
    void onAudios     ();
    void onTrackVideo ();
    void onTrackAudio ();

    void onError(const QString & message);

    void onOutputAdded(const WBackendOutput &);

    void onOutputRemoved(int);

public: // Variables
    QList<WBackendManagerItem> items;

    WBackendManagerItem * currentItem;

    WAbstractBackend  * backend;
    WBackendInterface * backendInterface;

    WMediaReply * reply;

    bool loaded;
    bool connected;
    bool hub;
    bool loop;

    bool freeze;
    bool freezeLoop;

    QString urlSource;

    Type type;

    QString timeZone;

    int timeA;
    int timeB;

    int start;

    QHash<WAbstractBackend::Quality, QString> medias;

    QString currentMedia;

    int timerClock;
    int timerSynchronize;
    int timerReload;

    QElapsedTimer time;

    QTimer timer;

    QList<const WBackendOutput *> outputs;

protected:
    W_DECLARE_PUBLIC(WBackendManager)
};

#endif // SK_NO_BACKENDMANAGER
#endif // WBACKENDMANAGER_P_H
