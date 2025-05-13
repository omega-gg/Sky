//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#ifndef WHOOKOUTPUT_H
#define WHOOKOUTPUT_H

// Sk includes
#include <WAbstractHook>

#ifndef SK_NO_HOOKOUTPUT

class WHookOutputPrivate;

class SK_GUI_EXPORT WHookOutput : public WAbstractHook
{
    Q_OBJECT

    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectedChanged)

    Q_PROPERTY(int countSettings READ countSettings NOTIFY settingsChanged)

    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)

    Q_PROPERTY(int screenCount READ screenCount NOTIFY screenCountChanged)

    Q_PROPERTY(int screen READ screen WRITE setScreen NOTIFY screenChanged)

    Q_PROPERTY(bool fullScreen READ fullScreen WRITE setFullScreen NOTIFY fullScreenChanged)

    Q_PROPERTY(bool videoTag READ videoTag WRITE setVideoTag NOTIFY videoTagChanged)

    Q_PROPERTY(bool startup READ startup WRITE setStartup NOTIFY startupChanged)

public:
    WHookOutput(WAbstractBackend * backend);
protected:
    WHookOutput(WHookOutputPrivate * p, WAbstractBackend * backend);

public: // Interface
    Q_INVOKABLE void connectToHost(const QString & url);

    Q_INVOKABLE void disconnectHost();

    Q_INVOKABLE bool hasSetting(const QString & name) const;

    Q_INVOKABLE void clearCache();
    Q_INVOKABLE void shutdown  ();

public: // WAbstractHook reimplementation
    /* Q_INVOKABLE virtual */ void loadSource(const QString     & url,
                                              int                 duration    = -1,
                                              int                 currentTime = -1,
                                              const WMediaReply * reply       = NULL);

    /* Q_INVOKABLE virtual */ void play  ();
    /* Q_INVOKABLE virtual */ void replay();

    /* Q_INVOKABLE virtual */ void pause();
    /* Q_INVOKABLE virtual */ void stop ();
    /* Q_INVOKABLE virtual */ void clear();

    /* Q_INVOKABLE virtual */ void seek(int msec);

protected: // Virtual functions
    virtual QString getSource(const QString & url) const;

protected: // WAbstractHook reimplementation
    /* virtual */ bool hookCheck(const QString & url);

signals:
    void connectedChanged();

    void settingsChanged();

    void volumeChanged();

    void screenCountChanged();
    void screenChanged     ();

    void fullScreenChanged();

    void videoTagChanged();

    void startupChanged();

public: // Properties
    bool isConnected() const;

    int countSettings() const;

    qreal volume() const;
    void  setVolume(qreal volume);

    int screenCount() const;

    int  screen() const;
    void setScreen(int index);

    bool fullScreen() const;
    void setFullScreen(bool fullScreen);

    bool videoTag() const;
    void setVideoTag(bool enabled);

    bool startup() const;
    void setStartup(bool enabled);

private:
    W_DECLARE_PRIVATE(WHookOutput)

    Q_PRIVATE_SLOT(d_func(), void onOutputChanged  ())
    Q_PRIVATE_SLOT(d_func(), void onQualityChanged ())
    Q_PRIVATE_SLOT(d_func(), void onModeChanged    ())
    Q_PRIVATE_SLOT(d_func(), void onFillModeChanged())
    Q_PRIVATE_SLOT(d_func(), void onSpeedChanged   ())
    Q_PRIVATE_SLOT(d_func(), void onVideoChanged   ())
    Q_PRIVATE_SLOT(d_func(), void onAudioChanged   ())
    Q_PRIVATE_SLOT(d_func(), void onAdjustChanged  ())
    Q_PRIVATE_SLOT(d_func(), void onSubtitleChanged())

    Q_PRIVATE_SLOT(d_func(), void onCurrentOutputChanged())

    Q_PRIVATE_SLOT(d_func(), void onConnectedChanged())

    Q_PRIVATE_SLOT(d_func(), void onReply(const WBroadcastReply &))
};

#include <private/WHookOutput_p>

#endif // SK_NO_HOOKOUTPUT
#endif // WHOOKOUTPUT_H
