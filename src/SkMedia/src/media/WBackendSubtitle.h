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

#ifndef WBACKENDSUBTITLE_H
#define WBACKENDSUBTITLE_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

#ifndef SK_NO_BACKENDSUBTITLE

class WBackendSubtitlePrivate;

#ifdef QT_6
Q_MOC_INCLUDE("QThread.h")
#endif

class SK_MEDIA_EXPORT WBackendSubtitle : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(QThread * thread READ thread WRITE setThread NOTIFY threadChanged)

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(bool retry READ retry WRITE setRetry NOTIFY retryChanged)

    Q_PROPERTY(int currentTime READ currentTime WRITE setCurrentTime NOTIFY currentTimeChanged)

    Q_PROPERTY(QString text READ text NOTIFY textChanged)

public:
    explicit WBackendSubtitle(QObject * parent = NULL);

protected: // Events
    /* virtual */ void timerEvent(QTimerEvent * event);

signals:
    void loaded(bool ok);

    void threadChanged();

    void enabledChanged();

    void sourceChanged();

    void retryChanged();

    void currentTimeChanged();

    void textChanged();

public: // Properties
    QThread * thread() const;
    void      setThread(QThread * thread);

    bool isEnabled() const;
    void setEnabled(bool enabled);

    QString source() const;
    void    setSource(const QString & url);

    bool retry() const;
    void setRetry(int count);

    int  currentTime() const;
    void setCurrentTime(int msec);

    QString text() const;

private:
    W_DECLARE_PRIVATE(WBackendSubtitle)

    Q_PRIVATE_SLOT(d_func(), void onQueryData(const QByteArray &, const QString &))

    Q_PRIVATE_SLOT(d_func(), void onQueryCompleted())

    Q_PRIVATE_SLOT(d_func(), void onLoaded(const QList<WBackendSubtitleData> &))
};

#include <private/WBackendSubtitle_p>

#endif // SK_NO_BACKENDSUBTITLE
#endif // WBACKENDSUBTITLE_H
