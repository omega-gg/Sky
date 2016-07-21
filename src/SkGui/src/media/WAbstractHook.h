//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WABSTRACTHOOK_H
#define WABSTRACTHOOK_H

// Sk includes
#include <WAbstractBackend>

#ifndef SK_NO_ABSTRACTHOOK

class WAbstractHookPrivate;

class SK_GUI_EXPORT WAbstractHook : public WAbstractBackend
{
    Q_OBJECT

    Q_PROPERTY(WAbstractBackend * backend READ backend CONSTANT)

public:
    WAbstractHook(WAbstractBackend * backend);
protected:
    WAbstractHook(WAbstractHookPrivate * p, WAbstractBackend * backend);

public: // Interface
    Q_INVOKABLE bool checkSource(const QUrl & url);

protected: // Abstract functions
    virtual bool hookCheckSource(const QUrl & url) = 0;

protected: // WAbstractBackend implementation
    /* virtual */ bool backendSetSource(const QUrl & url);

    /* virtual */ bool backendPlay  ();
    /* virtual */ bool backendReplay();

    /* virtual */ bool backendPause();
    /* virtual */ bool backendStop ();

    /* virtual */ void backendSetVolume(int percent);

    /* virtual */ bool backendDelete();

protected: // WAbstractBackend reimplementation
    /* virtual */ void backendSeekTo(int msec);

    /* virtual */ void backendSetSpeed(qreal speed);

    /* virtual */ void backendSetRepeat(bool repeat);

    /* virtual */ void backendSetQuality(Quality quality);

    /* virtual */ void backendSetFillMode(FillMode fillMode);

    /* virtual */ void backendSetSize(const QSizeF & size);

    /* virtual */ void backendDrawFrame(QPainter                       * painter,
                                        const QStyleOptionGraphicsItem * option);

    /* virtual */ void   backendUpdateFrame();
    /* virtual */ QImage backendGetFrame   () const;

public: // Properties
    WAbstractBackend * backend() const;

private:
    W_DECLARE_PRIVATE(WAbstractHook)
};

#endif // SK_NO_ABSTRACTHOOK
#endif // WABSTRACTHOOK_H
