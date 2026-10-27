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

#ifndef WBACKENDVLC_P_H
#define WBACKENDVLC_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#if defined(QT_6) == false && defined(SK_NO_QML) == false
#include <QGLWidget>
#endif
#include <QMutex>
#include <QMetaMethod>

// Sk includes
#include <WVlcPlayer>
#if defined(QT_NEW) && defined(SK_NO_QML) == false
#include <WBackendYuv>
#endif

// Private includes
#include <private/WAbstractBackend_p>

#ifndef SK_NO_BACKENDVLC

// Forward declarations
class WMediaReply;

//-------------------------------------------------------------------------------------------------
// WBackendVlcTexture
//-------------------------------------------------------------------------------------------------

struct WBackendVlcTexture
{
    WBackendVlcTexture()
    {
        width  = 0;
        height = 0;

#ifdef QT_6
        length = 0;
#endif

        pitch       = 0;
        pitchMargin = 0;

        bitsA = NULL;
        bitsB = NULL;

        bits = NULL;
    }

    int width;
    int height;

#ifdef QT_6
    int length;
#endif

    int pitch;
    int pitchMargin;

    uchar * bitsA;
    uchar * bitsB;

    uchar * bits;
};


//-------------------------------------------------------------------------------------------------
// WBackendVlcPrivate
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WBackendVlcPrivate : public WAbstractBackendPrivate
{
public: // Enums
    enum EventType
    {
        EventSetup
    };

public:
    WBackendVlcPrivate(WBackendVlc * p);

    /* virtual */ ~WBackendVlcPrivate();

    void init();

public: // Functions
    void populateTableRgb();

#if defined(QT_4) && defined(SK_NO_QML) == false
    void initShader  ();
    void deleteShader();
#endif

    void convertFrameSoftware();
#ifdef CAN_COMPILE_SSE2
    void convertFrameSse();
#elif defined(__ARM_NEON)
    void convertFrameNeon();
#endif

    void loadSources(bool play);

    void applySources(const WMediaReply * reply, bool play);

    void applyOutput(WAbstractBackend::Output output);

    bool applyQuality(WAbstractBackend::Quality quality);

    void playAt(int time);

    void playMedia();

    void reload();

    void updateTargetRect();

    void updateLoading();

    void clearPlayer();
    void clearActive();

    void clearReply  ();
    void clearMedia  ();
    void clearSources();

#if defined(QT_4) && defined(SK_NO_QML) == false
    void setOpacity(GLfloat opacity);
#endif

    //void setMute(bool enabled);

    WAbstractBackend::Output getOutput(WAbstractBackend::Output output);

public: // Static functions
    static unsigned setup(void     ** data,     char     * chroma,
                          unsigned *  vlcWidth, unsigned * vlcHeight,
                          unsigned *  pitches,  unsigned * lines);

    static void * lock(void * data, void ** buffer);

    static void unlock(void * data, void * id, void * const * pixels);

    static void applyFrames(WBackendVlcPrivate * d, QImage & frameA, QImage & frameB,
                            int cursorU, int cursorV);

public: // Slots
    void onLoaded();

    void onFrameUpdated();

#ifdef QT_NEW
    void onUpdateState();
#endif

public: // Variables
    // NOTE: Do we really need a mutex on here after all ?
    // NOTE Qt6: The absence of mutex seems to cause random freezes when starting the playback.
    QMutex mutex;

    WVlcPlayer * player;

    uint32_t tableRgb[1910];

    int width;
    int height;

    int frameWidth;
    int frameHeight;

    QImage frames[4];

    QImage frameSoftware;

    bool frameSwap;
    bool frameIndex;

    QRectF targetRect;

#if defined(QT_4) && defined(SK_NO_QML) == false
    GLfloat targetX;
    GLfloat targetY;
    GLfloat targetWidth;
    GLfloat targetHeight;

    bool shader;

    GLuint textureIds[3];
#endif

    WBackendVlcTexture textures[3];

    bool started;
    bool active;
    bool playing;

    bool frameReset;
    bool frameUpdated;
    bool frameFreeze;

    int length;

    //int  volume;
    //bool mute;

    QHash<WAbstractBackend::Quality, QString> medias;
    QHash<WAbstractBackend::Quality, QString> audios;

    QStringList options;

    bool loop;

    QString currentMedia;
    QString currentAudio;

    WAbstractBackend::Output  closestOutput;
    WAbstractBackend::Quality closestQuality;

    Qt::AspectRatioMode ratio;

    QList<const WBackendOutput *> outputs;

    int indexOutput;

    WMediaReply * reply;

    QMetaMethod method;

protected:
    W_DECLARE_PUBLIC(WBackendVlc)
};

//-------------------------------------------------------------------------------------------------
// WBackendVlcEventSetup
//-------------------------------------------------------------------------------------------------

class WBackendVlcEventSetup : public QEvent
{
public:
#ifdef QT_OLD
    WBackendVlcEventSetup(int width, int height, int pitchY, int pitchU, int pitchV)
#else // QT_6
    WBackendVlcEventSetup(int width, int height, int pitchY, int pitchU, int pitchV,
                          int cursorU, int cursorV)
#endif
        : QEvent(static_cast<QEvent::Type> (WBackendVlcPrivate::EventSetup))
    {
        this->width  = width;
        this->height = height;

        this->pitchY = pitchY;
        this->pitchU = pitchU;
        this->pitchV = pitchV;

#ifdef QT_6
        this->cursorU = cursorU;
        this->cursorV = cursorV;
#endif
    }

public: // Variables
    int width;
    int height;

    int pitchY;
    int pitchU;
    int pitchV;

#ifdef QT_6
    int cursorU;
    int cursorV;
#endif
};

#endif // SK_NO_BACKENDVLC
#endif // WBACKENDVLC_P_H
