//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

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

#ifndef WBACKENDFFMPEG_P_H
#define WBACKENDFFMPEG_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QMutex>
#include <QMetaMethod>

// Sk includes
#include <WFfmpegPlayer>
#ifndef SK_NO_QML
#include <WBackendYuv>
#endif

// Private includes
#include <private/WAbstractBackend_p>

#ifndef SK_NO_BACKENDFFMPEG

// Forward declarations
class WMediaReply;
struct SwsContext;

//-------------------------------------------------------------------------------------------------
// WBackendFfmpegTexture
//-------------------------------------------------------------------------------------------------

struct WBackendFfmpegTexture
{
    WBackendFfmpegTexture()
    {
        width  = 0;
        height = 0;

        length = 0;
        pitch  = 0;

        bitsA = NULL;
        bitsB = NULL;

        bits = NULL;
    }

    int width;
    int height;

    int length;
    int pitch;

    uchar * bitsA;
    uchar * bitsB;

    uchar * bits;
};

//-------------------------------------------------------------------------------------------------
// WBackendFfmpegPrivate
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WBackendFfmpegPrivate : public WAbstractBackendPrivate
{
public: // Enums
    enum EventType
    {
        EventSetup
    };

public:
    WBackendFfmpegPrivate(WBackendFfmpeg * p);

    /* virtual */ ~WBackendFfmpegPrivate();

    void init();

public: // Functions
    void convertFrame();

    void loadSources(bool play);

    void applySources(const WMediaReply * reply, bool play);

    bool applyQuality(WAbstractBackend::Quality quality);

    void playAt(int time);

    void playMedia();

    void restart();
    void reload ();

    void updateTargetRect();

    void updateLoading();

    void clearPlayer();
    void clearActive();

    void clearReply  ();
    void clearMedia  ();
    void clearSources();

    WAbstractBackend::Output getOutput(WAbstractBackend::Output output);

public: // Static functions
    static bool setup(void * data, int * width, int * height, int * pitches, int * lines);

    static void lock(void * data, uchar ** planes);

    static void unlock(void * data);

    static void applyFrames(WBackendFfmpegPrivate * d, QByteArray & frameA, QByteArray & frameB,
                            int cursorU, int cursorV);

public: // Slots
    void onLoaded();

    void onFrameUpdated();

public: // Variables
    QMutex mutex;

    WFfmpegPlayer * player;

    int width;
    int height;

    int frameWidth;
    int frameHeight;

    QByteArray frames[4];

    QImage frameSoftware;

    SwsContext * scale;

    bool frameSwap;
    bool frameIndex;

    QRectF targetRect;

    WBackendFfmpegTexture textures[3];

    bool started;
    bool active;
    bool playing;

    bool frameReset;
    bool frameUpdated;
    bool frameFreeze;

    int length;

    // NOTE: The streams the player plays, from its last tracks event.
    int streamVideo;
    int streamAudio;

    QHash<WAbstractBackend::Quality, QString> medias;
    QHash<WAbstractBackend::Quality, QString> audios;

    QStringList options;

    bool loop;

    QString currentMedia;
    QString currentAudio;

    // NOTE: The source mode the sources were loaded with (see getMode).
    WAbstractBackend::SourceMode currentMode;

    WAbstractBackend::Output  closestOutput;
    WAbstractBackend::Quality closestQuality;

    Qt::AspectRatioMode ratio;

    WMediaReply * reply;

    QMetaMethod method;

protected:
    W_DECLARE_PUBLIC(WBackendFfmpeg)
};

//-------------------------------------------------------------------------------------------------
// WBackendFfmpegEventSetup
//-------------------------------------------------------------------------------------------------

class WBackendFfmpegEventSetup : public QEvent
{
public:
    WBackendFfmpegEventSetup(int width, int height, int pitchY, int pitchU, int pitchV,
                             int cursorU, int cursorV)
        : QEvent(static_cast<QEvent::Type> (WBackendFfmpegPrivate::EventSetup))
    {
        this->width  = width;
        this->height = height;

        this->pitchY = pitchY;
        this->pitchU = pitchU;
        this->pitchV = pitchV;

        this->cursorU = cursorU;
        this->cursorV = cursorV;
    }

public: // Variables
    int width;
    int height;

    int pitchY;
    int pitchU;
    int pitchV;

    int cursorU;
    int cursorV;
};

#endif // SK_NO_BACKENDFFMPEG
#endif // WBACKENDFFMPEG_P_H
