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

#ifndef WBACKENDFFMPEG_H
#define WBACKENDFFMPEG_H

// Sk includes
#include <WAbstractBackend>

#ifndef SK_NO_BACKENDFFMPEG

class WBackendFfmpegPrivate;

class SK_MEDIA_EXPORT WBackendFfmpeg : public WAbstractBackend
{
    Q_OBJECT

public:
    WBackendFfmpeg(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void setProxy(const QString & host,
                              int             port, const QString & password = QString());

    Q_INVOKABLE void clearProxy();

protected: // WAbstractBackend implementation
#ifndef SK_NO_QML
    /* virtual */ WBackendNode * backendCreateNode() const;
#endif

    /* virtual */ bool backendSetSource(const QString & url, const WMediaReply * reply);

    /* virtual */ bool backendPlay ();
    /* virtual */ bool backendPause();
    /* virtual */ bool backendStop ();

    /* virtual */ void backendSetVolume(qreal volume);

    /* virtual */ bool backendDelete();

protected: // WAbstractBackend reimplementation
    /* virtual */ void backendSeek(int msec);

    /* virtual */ void backendSetSpeed(qreal speed);

    /* virtual */ void backendSetOutput    (Output     output);
    /* virtual */ void backendSetQuality   (Quality    quality);
    /* virtual */ void backendSetSourceMode(SourceMode mode);

    /* virtual */ void backendSetFillMode(FillMode fillMode);

    /* virtual */ void backendSetAdjust(const WBackendAdjust & adjust);

    /* virtual */ void backendSetVideo(int index);
    /* virtual */ void backendSetAudio(int index);

    /* virtual */ void backendSetSize(const QSizeF & size);

#ifndef SK_NO_QML
    /* virtual */ void backendSynchronize(WBackendFrame * frame);
#endif

    /* virtual */ void backendDrawFrame(QPainter * painter, const QRect & rect);

    /* virtual */ void backendUpdateFrame();

    /* virtual */ void backendNextFrame();

    /* virtual */ QImage backendGetFrame    () const;
    /* virtual */ QImage backendGetFrameGray() const;

    /* virtual */ QRectF backendRect() const;

protected: // Events
    /* virtual */ bool event(QEvent * event);

private:
    W_DECLARE_PRIVATE(WBackendFfmpeg)

    Q_PRIVATE_SLOT(d_func(), void onLoaded      ())
    Q_PRIVATE_SLOT(d_func(), void onFrameUpdated())
};

#include <private/WBackendFfmpeg_p>

#endif // SK_NO_BACKENDFFMPEG
#endif // WBACKENDFFMPEG_H
