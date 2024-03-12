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

// Private includes
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

public: // Static functions
    static int queryIndex(const QString & string, const QString & key, const QChar & character);

    static QString extractQuery(const QString & string,
                                const QString & key, const QChar & character);

    static QString applyQuery(const QString & string,
                              const QString & key, const QString & value, const QChar & character);

    static QString removeQuery(const QString & string,
                               const QString & key, const QChar & character);

    //---------------------------------------------------------------------------------------------
    // Json

    static int indexJsonEndA(const QString & text, int at);
    static int indexJsonEndB(const QString & text, int at, const QChar & charA);

    //---------------------------------------------------------------------------------------------
    // Bencode

    static int listAfter(const QString & text, const QString & string, int * at);

    static int extractString(QString * string, const QString & data, int at);

    static int getIndex(const QString & name);

#ifndef SK_NO_TORRENT
    //---------------------------------------------------------------------------------------------
    // Torrent

    static int extractItem(WTorrentItemData * item, const QString & data, int at);

    static WTorrentItemFolder extractFolder(QList<WTorrentItemData> * items);
#endif

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
