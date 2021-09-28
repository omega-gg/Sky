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
//#include <QMutex>
#include <QMetaMethod>
#ifdef QT_LATEST
#include <QSGMaterial>
#endif

// Sk includes
#include <WVlcPlayer>

// Private includes
#include <private/WAbstractBackend_p>

#ifndef SK_NO_BACKENDVLC

// Forward declarations
#ifdef QT_LATEST
class QOpenGLFunctions;
#endif
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

#ifdef QT_4
    void initShader  ();
    void deleteShader();
#endif

    void convertFrameSoftware();
    void convertFrameSse     ();

    void loadSources (bool play);
    void applySources(bool play);

    bool applyOutput(WAbstractBackend::Output output);

    void playMedia();

    void updateTargetRect();

    void updateLoading();

    void clearPlayer();
    void clearActive();

    void clearReply  ();
    void clearMedia  ();
    void clearSources();

#ifdef QT_4
    void setOpacity(GLfloat opacity);
#endif

    void setMute(bool enabled);

    WAbstractBackend::Output getOutput(WAbstractBackend::Output output);

    bool applyQuality(WAbstractBackend::Quality quality);

public: // Slots
    void onLoaded();

    void onFrameUpdated();

#ifdef QT_LATEST
    void onUpdateState();
#endif

public: // Static functions
    static unsigned setup(void     ** data,     char     * chroma,
                          unsigned *  vlcWidth, unsigned * vlcHeight,
                          unsigned *  pitches,  unsigned * lines);

    static void * lock(void * data, void ** buffer);

    static void unlock(void * data, void * id, void * const * pixels);

    static void applyFrames(WBackendVlcPrivate * d, QImage & frameA, QImage & frameB,
                            int cursorU, int cursorV);

public: // Variables
    // NOTE: Do we really need a mutex on here after all ?
    //QMutex mutex;

    WVlcPlayer * player;

    uint32_t tableRgb[1935];

    int width;
    int height;

    int frameWidth;
    int frameHeight;

    QImage frames[4];

    QImage frameSoftware;

    bool frameSwap;
    bool frameIndex;

    QRectF targetRect;

#ifdef QT_4
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

    int  volume;
    bool mute;

    QHash<WAbstractBackend::Quality, QString> medias;
    QHash<WAbstractBackend::Quality, QString> audios;

    QString currentMedia;
    QString currentAudio;

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
