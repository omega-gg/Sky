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

#ifndef WDECLARATIVEANIMATED_H
#define WDECLARATIVEANIMATED_H

// Sk includes
#include <WDeclarativeItem>

#ifndef SK_NO_DECLARATIVEANIMATED

class WDeclarativeAnimatedPrivate;

class SK_GUI_EXPORT WDeclarativeAnimated : public WDeclarativeItem
{
    Q_OBJECT

    Q_ENUMS(StepDirection)
    Q_ENUMS(StepMode)

    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)

    Q_PROPERTY(StepDirection stepDirection READ stepDirection WRITE setStepDirection
               NOTIFY stepDirectionChanged)

    Q_PROPERTY(bool isBackward READ isBackward NOTIFY stepDirectionChanged)
    Q_PROPERTY(bool isForward  READ isForward  NOTIFY stepDirectionChanged)

    Q_PROPERTY(StepMode stepMode READ stepMode WRITE setStepMode NOTIFY stepModeChanged)

    Q_PROPERTY(int steps READ stepCount WRITE setStepCount NOTIFY stepCountChanged)
    Q_PROPERTY(int step  READ step      WRITE setStep      NOTIFY stepChanged)

    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)

    Q_PROPERTY(int loops READ loopCount WRITE setLoopCount NOTIFY loopCountChanged)
    Q_PROPERTY(int loop  READ loop                         NOTIFY loopChanged)

public: // Enums
    enum StepDirection { StepBackward, StepForward };

    enum StepMode { StepAuto, StepManual };

public:
#ifdef QT_4
    explicit WDeclarativeAnimated(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeAnimated(QQuickItem * parent = NULL);
#endif

protected:
#ifdef QT_4
    WDeclarativeAnimated(WDeclarativeAnimatedPrivate * p, QDeclarativeItem * parent = NULL);
#else
    WDeclarativeAnimated(WDeclarativeAnimatedPrivate * p, QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE void start  ();
    Q_INVOKABLE void restart();

    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop ();

    Q_INVOKABLE void stepBackward();
    Q_INVOKABLE void stepForward ();

public: // QDeclarativeItem / QQuickItem reimplementation
    /* virtual */ void componentComplete();

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_4
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);
#else
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & value);
#endif

signals:
    void runningChanged();

    void stepDirectionChanged();
    void stepModeChanged     ();

    void stepCountChanged();
    void stepChanged     ();

    void intervalChanged();

    void loopCountChanged();
    void loopChanged     ();

public: // Properties
    bool running() const;
    void setRunning(bool running);

    StepDirection stepDirection() const;
    void          setStepDirection(StepDirection direction);

    bool isBackward() const;
    bool isForward () const;

    StepMode stepMode() const;
    void     setStepMode(StepMode mode);

    int  stepCount() const;
    void setStepCount(int count);

    int  step() const;
    void setStep(int step);

    int  interval() const;
    void setInterval(int interval);

    int  loopCount() const;
    void setLoopCount(int count);

    int loop() const;

private:
    W_DECLARE_PRIVATE(WDeclarativeAnimated)

    Q_PRIVATE_SLOT(d_func(), void onFinished())
};

QML_DECLARE_TYPE(WDeclarativeAnimated)

#include <private/WDeclarativeAnimated_p>

#endif // SK_NO_DECLARATIVEANIMATED
#endif // WDECLARATIVEANIMATED_H
