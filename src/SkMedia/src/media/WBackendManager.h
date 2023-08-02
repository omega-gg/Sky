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

#ifndef WBACKENDMANAGER_H
#define WBACKENDMANAGER_H

// Sk includes
#include <WAbstractBackend>

#ifndef SK_NO_BACKENDMANAGER

// Forward declarations
class WBackendManagerPrivate;

class SK_MEDIA_EXPORT WBackendManager : public WAbstractBackend
{
    Q_OBJECT

public:
    WBackendManager(QObject * parent = NULL);
protected:
    WBackendManager(WBackendManagerPrivate * p, QObject * parent = NULL);

protected: // Virtual functions
    virtual WAbstractHook * createHook(WAbstractBackend * backend); // {}

protected: // WAbstractBackend implementation
#ifdef QT_NEW
    /* virtual */ WBackendNode * backendCreateNode() const;
#endif

    /* virtual */ bool backendSetSource(const QString & url);

    /* virtual */ bool backendPlay ();
    /* virtual */ bool backendPause();
    /* virtual */ bool backendStop ();

    /* virtual */ void backendSetVolume(qreal volume);

    /* virtual */ bool backendDelete();

protected: // WAbstractBackend reimplementation
    /* virtual */ void backendSeek(int msec);

    /* virtual */ void backendSetSpeed(qreal speed);

    /* virtual */ void backendSetOutput (Output  output);
    /* virtual */ void backendSetQuality(Quality quality);

    /* virtual */ void backendSetFillMode(FillMode fillMode);

    /* virtual */ void backendSetVideo(int index);
    /* virtual */ void backendSetAudio(int index);

    /* virtual */ void backendSetScanOutput(bool enabled);

    /* virtual */ void backendSetCurrentOutput(const WBackendOutput * output);

    /* virtual */ void backendSetSize(const QSizeF & size);

#ifdef QT_NEW
    /* virtual */ void backendSynchronize(WBackendFrame * frame);
#endif

    /* virtual */ void backendDrawFrame(QPainter * painter, const QRect & rect);

    /* virtual */ void   backendUpdateFrame();
    /* virtual */ QImage backendGetFrame   () const;

    /* virtual */ QRectF backendRect() const;

private:
    W_DECLARE_PRIVATE(WBackendManager)

    Q_PRIVATE_SLOT(d_func(), void onLoaded())

    Q_PRIVATE_SLOT(d_func(), void onState     ())
    Q_PRIVATE_SLOT(d_func(), void onStateLoad ())
    Q_PRIVATE_SLOT(d_func(), void onTime      ())
    Q_PRIVATE_SLOT(d_func(), void onVideos    ())
    Q_PRIVATE_SLOT(d_func(), void onAudios    ())
    Q_PRIVATE_SLOT(d_func(), void onTrackVideo())
    Q_PRIVATE_SLOT(d_func(), void onTrackAudio())
};

#include <private/WBackendManager_p>

#endif // SK_NO_BACKENDMANAGER
#endif // WBACKENDMANAGER_H
