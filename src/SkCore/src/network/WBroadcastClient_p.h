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

#ifndef WBROADCASTCLIENT_P_H
#define WBROADCASTCLIENT_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_BROADCASTCLIENT

// Forward declarations
class WBroadcastClientThread;

class SK_CORE_EXPORT WBroadcastClientPrivate : public WPrivate
{
public: // Enums
    enum EventType
    {
        EventConnected = QEvent::User,
        EventDisconnected,
        EventReply
    };

public:
    WBroadcastClientPrivate(WBroadcastClient * p);

    /* virtual */ ~WBroadcastClientPrivate();

    void init();

public: // Functions
    void createThread();

    void postEvent(QEvent * event);

    void setConnected(bool connected);

    void setSource(const WBroadcastSource & source);

public: // Static functions
    static QString extractName     (QString * data);
    static QString extractParameter(QString * data);

    static QByteArray generateData(const QString & type, const QStringList & parameters);

public: // Variables
    WBroadcastClientThread * thread;

    bool connected;

    WBroadcastSource source;

protected:
    W_DECLARE_PUBLIC(WBroadcastClient)
};

#endif // SK_NO_BROADCASTCLIENT
#endif // WBROADCASTCLIENT_P_H
