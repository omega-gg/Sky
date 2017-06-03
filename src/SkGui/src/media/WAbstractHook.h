//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

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

class SK_GUI_EXPORT WAbstractHook : public QObject,
                                    public WBackendInterface, public WBackendFilter,
                                    public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(WAbstractBackend * backend READ backend CONSTANT)

protected:
    WAbstractHook(WAbstractBackend * backend);

    WAbstractHook(WAbstractHookPrivate * p, WAbstractBackend * backend);

public: // Interface
    Q_INVOKABLE bool checkSource(const QUrl & url);

public: // WBackendInterface implementation
    Q_INVOKABLE /* virtual */ QUrl source() const;

    Q_INVOKABLE /* virtual */ bool sourceIsVideo() const;
    Q_INVOKABLE /* virtual */ bool sourceIsAudio() const;

    Q_INVOKABLE /* virtual */ void loadSource(const QUrl & url, int duration    = -1,
                                                                int currentTime = -1);

    Q_INVOKABLE /* virtual */ void play  ();
    Q_INVOKABLE /* virtual */ void replay();

    Q_INVOKABLE /* virtual */ void pause();
    Q_INVOKABLE /* virtual */ void stop ();
    Q_INVOKABLE /* virtual */ void clear();

    Q_INVOKABLE /* virtual */ void seekTo(int msec);

protected: // Functions
    void setFilterActive(bool active);

    //---------------------------------------------------------------------------------------------
    // Backend functions

    void setState    (WAbstractBackend::State     state);
    void setStateLoad(WAbstractBackend::StateLoad stateLoad);

    void setEnded(bool ended);

    void setCurrentTime(int msec);
    void setDuration   (int msec);

    void setOutputActive (WAbstractBackend::Output  output);
    void setQualityActive(WAbstractBackend::Quality quality);

    //---------------------------------------------------------------------------------------------
    // Backend abstract functions

    bool backendSetSource(const QUrl & url);

    bool backendPlay ();
    bool backendPause();
    bool backendStop ();

    void backendSetVolume(qreal volume);

    bool backendDelete();

    //---------------------------------------------------------------------------------------------
    // Backend virtual functions

    void backendSeekTo(int msec);

    void backendSetSpeed(qreal speed);

    void backendSetOutput (WAbstractBackend::Output  output);
    void backendSetQuality(WAbstractBackend::Quality quality);

    void backendSetFillMode(WAbstractBackend::FillMode fillMode);

    void backendSetSize(const QSizeF & size);

    void backendDrawFrame(QPainter * painter, const QStyleOptionGraphicsItem * option);

    void   backendUpdateFrame();
    QImage backendGetFrame   () const;

protected: // Abstract functions
    virtual bool hookCheckSource(const QUrl & url) = 0;

public: // Properties
    WAbstractBackend * backend() const;

private:
    W_DECLARE_PRIVATE(WAbstractHook)

    Q_PRIVATE_SLOT(d_func(), void onBackendDestroyed())
};

#include <private/WAbstractHook_p>

#endif // SK_NO_ABSTRACTHOOK
#endif // WABSTRACTHOOK_H
