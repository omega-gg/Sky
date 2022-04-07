//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#ifndef WCONTROLLERAPPLICATION_P_H
#define WCONTROLLERAPPLICATION_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QUrl>
#include <QPoint>

#ifdef Q_OS_MACX
// macOS includes
#include <IOKit/pwr_mgt/IOPMLib.h>
#endif

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_CONTROLLERAPPLICATION

// Forward declarations
class QCoreApplication;

class SK_CORE_EXPORT WControllerApplicationPrivate : public WPrivate
{
public:
    WControllerApplicationPrivate(WControllerApplication * p);

    /* virtual */ ~WControllerApplicationPrivate();

    void init();

public: // Functions
    QHash<QString, QString> extractArguments(int & argc, char ** argv);

    void declareController  (WController * controller);
    void undeclareController(WController * controller);

#ifdef Q_OS_IOS
    // NOTE iOS: This function is defined in WControllerApplication.mm
    void setScreenSaverEnabled(bool enabled);
#endif

public: // WCoreApplication and WApplication interface
    void initApplication(QCoreApplication * application, Sk::Type type, bool gui);

public: // Slots
    void onAboutToQuit();

public: // Variables
    QList<WController *> controllers;

    QCoreApplication * application;

    Sk::Type type;
    bool     gui;

    QString name;
    QString icon;

    QString version;

    QString applicationUrl;

    bool qrc;

#ifndef SK_CONSOLE
    WControllerApplication::Mode defaultMode;

    int defaultScreen;

    int defaultWidth;
    int defaultHeight;

    int defaultMargins;

    bool screenDimEnabled;
    bool screenSaverEnabled;

    QPoint cursorPosition;
    bool   cursorVisible;
#endif

    QObject * object;

#ifdef Q_OS_WIN
    int timeoutLowPower;
    int timeoutPowerOff;
    int timeoutScreenSave;
#elif defined(Q_OS_MACX)
    IOPMAssertionID assertion;
#endif

protected:
    W_DECLARE_PUBLIC(WControllerApplication)
};

#endif // SK_NO_CONTROLLERAPPLICATION
#endif // WCONTROLLERAPPLICATION_P_H
