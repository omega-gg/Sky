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
    enum Type { Track, MultiTrack, Channel };

public:
    WBackendManagerPrivate(WBackendManager * p);

    /* virtual */ ~WBackendManagerPrivate();

    void init();

public: // Functions
    void loadSources ();
    void applySources(bool play);

    void loadSource(const QString & source, const QString & media, int currentTime);

    void applyTime(int currentTime);

    void updateLoading();

    void clearActive();

    void clearReply();
    void clearMedia();

    void stopTimer();

    void connectBackend   ();
    void disconnectBackend();

    //void setBackend(WAbstractBackend * backendNew);

    void setBackendInterface(WBackendInterface * backendNew);

public: // Slots
    void onLoaded();

#ifndef SK_NO_QML
    void onPlayerChanged();
#endif

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

    void onOutputChanged();

public: // Variables
    QList<WBackendManagerItem> items;

    WBackendManagerItem * currentItem;

    WAbstractBackend  * backend;
    WBackendInterface * backendInterface;

    WMediaReply * reply;

    Type type;

    int timeA;
    int timeB;

    int start;
    int end;

    QHash<WAbstractBackend::Quality, QString> medias;

    QString currentMedia;

    bool loaded;
    bool connected;

    int timer;

    QElapsedTimer time;

protected:
    W_DECLARE_PUBLIC(WBackendManager)
};

#endif // SK_NO_BACKENDMANAGER
#endif // WBACKENDMANAGER_P_H
