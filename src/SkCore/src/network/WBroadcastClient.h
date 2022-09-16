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

#ifndef WBROADCASTCLIENT_H
#define WBROADCASTCLIENT_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

#ifndef SK_NO_BROADCASTCLIENT

class WBroadcastClientPrivate;

//-------------------------------------------------------------------------------------------------
// WBroadcastSource
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WBroadcastSource
{
public:
    WBroadcastSource();

public: // Functions
    bool isValid() const;

public: // Operators
    WBroadcastSource(const WBroadcastSource & other);

    bool operator==(const WBroadcastSource & other) const;

    WBroadcastSource & operator=(const WBroadcastSource & other);

public: // Variables
    QString address;
    int     port;

    QString name;
};

//-------------------------------------------------------------------------------------------------
// WBroadcastClient
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WBroadcastClient : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectedChanged)

    Q_PROPERTY(WBroadcastSource source READ source NOTIFY sourceChanged)

public:
    explicit WBroadcastClient(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE bool connectToHost(const WBroadcastSource & source);
    Q_INVOKABLE bool connectToHost(const QString          & url);

    Q_INVOKABLE void disconnectHost();

public: // Static functions
    Q_INVOKABLE static WBroadcastSource extractSource(const QString & url);

protected: // Events
    /* virtual */ bool event(QEvent * event);

signals:
    void connectedChanged();

    void sourceChanged();

public: // Properties
    bool isConnected() const;

    const WBroadcastSource & source() const;

private:
    W_DECLARE_PRIVATE(WBroadcastClient)
};

#endif // SK_NO_BROADCASTCLIENT
#endif // WBROADCASTCLIENT_H
