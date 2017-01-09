//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WCONTROLLERNETWORK_P_H
#define WCONTROLLERNETWORK_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QTcpSocket>
#include <QTimer>

// Sk includes
#include <private/WController_p>

#ifndef SK_NO_CONTROLLERNETWORK

class SK_CORE_EXPORT WControllerNetworkPrivate : public WControllerPrivate
{
public:
    WControllerNetworkPrivate(WControllerNetwork * p);

    /* virtual */ ~WControllerNetworkPrivate();

    void init();

public: // Functions
    void checkConnection();

public: // Slots
    void onStateChanged(QAbstractSocket::SocketState socketState);

    void onSocketTimeout();
    void onRetryTimeout ();

public: // Variables
    bool connected;

    bool checkConnected;

    QTcpSocket socket;
    QTimer     socketTimer;
    QTimer     socketRetryTimer;

protected:
    W_DECLARE_PUBLIC(WControllerNetwork)
};

#endif // SK_NO_CONTROLLERNETWORK
#endif // WCONTROLLERNETWORK_P_H
