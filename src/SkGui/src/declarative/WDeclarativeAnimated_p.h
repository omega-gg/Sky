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

#ifndef WDECLARATIVEANIMATED_P_H
#define WDECLARATIVEANIMATED_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QPauseAnimation>

// Private includes
#include <private/WDeclarativeItem_p>

#ifndef SK_NO_DECLARATIVEANIMATED

class SK_GUI_EXPORT WDeclarativeAnimatedPrivate : public WDeclarativeItemPrivate
{
protected:
    WDeclarativeAnimatedPrivate(WDeclarativeAnimated * p);

    void init();

public: // Functions
    void start();
    void stop ();

    void update();

    void setStep(int step);
    void setLoop(int loop);

public: // Slots
    void onFinished();

public: // Variables
    QPauseAnimation pause;

    bool running;

    WDeclarativeAnimated::StepDirection stepDirection;
    WDeclarativeAnimated::StepMode      stepMode;

    int stepCount;
    int step;

    int loopCount;
    int loop;

protected:
    W_DECLARE_PUBLIC(WDeclarativeAnimated)
};

#endif // SK_NO_DECLARATIVEANIMATED
#endif // WDECLARATIVEANIMATED_P_H
