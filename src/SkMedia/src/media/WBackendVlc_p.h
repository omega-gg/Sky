//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkMedia module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
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
#include <QGLWidget>
#include <QMutex>
#include <QMetaMethod>

// Sk includes
#include <WVlcPlayer>

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

        pitch       = 0;
        pitchMargin = 0;

        bitsA = NULL;
        bitsB = NULL;

        bits = NULL;
    }

    int width;
    int height;

    int pitch;
    int pitchMargin;

    uchar * bitsA;
    uchar * bitsB;

    uchar * bits;
};

#ifdef QT_LATEST

//-------------------------------------------------------------------------------------------------
// WBackendVlcShader
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WBackendVlcShader : public QSGMaterialShader
{
public: // QSGMaterialShader implementation
    /* virtual */ char const * const * attributeNames() const;

public: // QSGMaterialShader reimplementation
    /* virtual */ void updateState(const RenderState & state, QSGMaterial * newMaterial,
                                                              QSGMaterial * oldMaterial);

protected: // QSGMaterialShader reimplementation
    /* virtual */ void initialize();

    /* virtual */ const char * vertexShader  () const;
    /* virtual */ const char * fragmentShader() const;

public: // Properties
    int idPosition;
    int idOpacity;
    int idColor;

    int idY;
    int idU;
    int idV;
};

//-------------------------------------------------------------------------------------------------
// WBackendVlcMaterial
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WBackendVlcMaterial : public QSGMaterial
{
public:
    WBackendVlcMaterial();

    /* virtual */ ~WBackendVlcMaterial();

public: // Functions
    void updateTextures();

public: // QSGMaterial implementation
    /* virtual */ QSGMaterialType * type() const;

    /* virtual */ QSGMaterialShader * createShader() const;

public: // Properties
    QOpenGLFunctions * gl;

    WBackendTexture * textures;

    GLuint ids[3];

    bool update;
};

//-------------------------------------------------------------------------------------------------
// WBackendVlcNode
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WBackendVlcNode : public WBackendNode
{
public:
    WBackendVlcNode();

public: // WBackendNode reimplementation
    /* virtual */ void setTextures(WBackendTexture * textures);

public: // Properties
    WBackendVlcMaterial material;
};

#endif

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

    void convertFrameSoftware();
    void convertFrameSse     ();

    void loadSources (bool play);
    void applySources(bool play);

    bool applyOutput(WAbstractBackend::Output output);

    void playMedia();

    void updateTargetRect();

    void updateLoading();

    void clearPlayer();
    void clearReply ();
    void clearActive();

    void setOpacity(GLfloat opacity);

    void setMute(bool enabled);

    WAbstractBackend::Output  getClosestOutput (WAbstractBackend::Output  output);
    WAbstractBackend::Quality getClosestQuality(WAbstractBackend::Quality quality);

public: // Slots
    void onLoaded();

    void onFrameUpdated();

public: // Static functions
    static unsigned setup(void     ** data,     char     * chroma,
                          unsigned *  vlcWidth, unsigned * vlcHeight,
                          unsigned *  pitches,  unsigned * lines);

    static void * lock(void * data, void ** buffer);

    static void unlock(void * data, void * id, void * const * pixels);

public: // Variables
    QMutex mutex;

    WVlcPlayer * player;

    uint32_t tableRgb[1935];

    int frameWidth;
    int frameHeight;

    QImage frameA;
    QImage frameB;

    QImage frameSoftware;

    bool frameIndex;

    QRect targetRect;

    GLfloat targetX;
    GLfloat targetY;
    GLfloat targetWidth;
    GLfloat targetHeight;

    GLuint textureIds[3];

    WBackendVlcTexture textures[3];

    GLfloat values[16];

    bool started;
    bool active;
    bool playing;

    bool frameReset;
    bool frameUpdated;
    bool frameFreeze;

    int  volume;
    bool mute;

    QHash<WAbstractBackend::Quality, QUrl> medias;
    QHash<WAbstractBackend::Quality, QUrl> audios;

    QUrl currentMedia;
    QUrl currentAudio;

    WAbstractBackend::Output  closestOutput;
    WAbstractBackend::Quality closestQuality;

    Qt::AspectRatioMode ratio;

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
    WBackendVlcEventSetup(int width, int height, int pitchY, int pitchU, int pitchV)
        : QEvent(static_cast<QEvent::Type> (WBackendVlcPrivate::EventSetup))
    {
        this->width  = width;
        this->height = height;

        this->pitchY = pitchY;
        this->pitchU = pitchU;
        this->pitchV = pitchV;
    }

public: // Variables
    int width;
    int height;

    int pitchY;
    int pitchU;
    int pitchV;
};

#endif // SK_NO_BACKENDVLC
#endif // WBACKENDVLC_P_H
