//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
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

#include <private/Sk_p>

#ifndef SK_NO_CONTROLLERAPPLICATION

// Forward declarations
class QCoreApplication;
class WFileWatcher;

class SK_CORE_EXPORT WControllerApplicationPrivate : public WPrivate
{
public:
    WControllerApplicationPrivate(WControllerApplication * p);

    /* virtual */ ~WControllerApplicationPrivate();

    void init();

public: // Functions
    void processArguments(int & argc, char ** argv);

    QHash<QString, QString> extractArguments(int & argc, char ** argv);

    void restartScript();

    void declareController  (WController * controller);
    void undeclareController(WController * controller);

public: // WCoreApplication and WApplication interface
    void createApplication(QCoreApplication * application, int & argc, char ** argv, Sk::Type type,
                           bool coreApplication);

public: // Slots
    void onAboutToQuit();

public: // Variables
    QList<WController *> controllers;

    QCoreApplication * application;

    Sk::Type type;

    bool coreApplication;

    QString name;
    QUrl    icon;

    QString version;

    QUrl applicationUrl;

    bool qrc;

    WControllerApplication::Mode defaultMode;

    int defaultScreen;

    int defaultWidth;
    int defaultHeight;

    bool screenDimEnabled;
    bool screenSaverEnabled;

    bool cursorVisible;

    QObject * object;

    WFileWatcher * watcher;

#ifdef Q_OS_WIN
    int timeoutLowPower;
    int timeoutPowerOff;
    int timeoutScreenSave;
#endif

protected:
    W_DECLARE_PUBLIC(WControllerApplication)
};

#endif // SK_NO_CONTROLLERAPPLICATION
#endif // WCONTROLLERAPPLICATION_P_H
