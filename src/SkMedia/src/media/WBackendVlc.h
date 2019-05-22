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

#ifndef WBACKENDVLC_H
#define WBACKENDVLC_H

// Sk includes
#include <WAbstractBackend>

#ifndef SK_NO_BACKENDVLC

class WBackendVlcPrivate;

class SK_MEDIA_EXPORT WBackendVlc : public WAbstractBackend
{
    Q_OBJECT

    Q_PROPERTY(QStringList options READ options WRITE setOptions NOTIFY optionsChanged)

    Q_PROPERTY(int networkCache READ networkCache WRITE setNetworkCache NOTIFY networkCacheChanged)

public:
    WBackendVlc();

public: // Interface
    Q_INVOKABLE void setProxy(const QString & host,
                              int             port, const QString & password = QString());

    Q_INVOKABLE void clearProxy();

protected: // WAbstractBackend implementation
#ifdef QT_LATEST
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

    /* virtual */ void backendSetSize(const QSizeF & size);

#ifdef QT_LATEST
    /* virtual */ void backendSynchronize(WBackendFrame * frame);
#endif

    /* virtual */ void backendDrawFrame(QPainter * painter, const QRect & rect);

    /* virtual */ void   backendUpdateFrame();
    /* virtual */ QImage backendGetFrame   () const;

    /* virtual */ QRectF backendRect() const;

protected: // Events
    /* virtual */ bool event(QEvent * event);

signals:
    void optionsChanged();

    void networkCacheChanged();

public: // Properties
    QStringList options();
    void        setOptions(const QStringList & options);

    int  networkCache();
    void setNetworkCache(int msec);

private:
    W_DECLARE_PRIVATE(WBackendVlc)

    Q_PRIVATE_SLOT(d_func(), void onLoaded      ())
    Q_PRIVATE_SLOT(d_func(), void onFrameUpdated())

#ifdef QT_LATEST
    Q_PRIVATE_SLOT(d_func(), void onUpdateState())
#endif
};

#include <private/WBackendVlc_p>

#endif // SK_NO_BACKENDVLC
#endif // WBACKENDVLC_H
