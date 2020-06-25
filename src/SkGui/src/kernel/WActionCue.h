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

#ifndef WACTIONCUE_H
#define WACTIONCUE_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

#ifndef SK_NO_ACTIONCUE

class WActionCuePrivate;

class SK_GUI_EXPORT WActionCue : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

    Q_PROPERTY(bool isActive READ isActive NOTIFY activeChanged)

    Q_PROPERTY(int maxCued READ maxCued WRITE setMaxCued NOTIFY maxCuedChanged)

public:
    explicit WActionCue(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE bool tryPush(int id);

    Q_INVOKABLE bool start(int msec);

    Q_INVOKABLE void clear();

signals:
    void processAction(int id);

    void enabledChanged();

    void activeChanged();

    void maxCuedChanged();

public: // Properties
    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool isActive() const;

    int  maxCued() const;
    void setMaxCued(int max);

private:
    W_DECLARE_PRIVATE(WActionCue)

    Q_PRIVATE_SLOT(d_func(), void onFinished())
};

#include <private/WActionCue_p>

#endif // SK_NO_ACTIONCUE
#endif // WACTIONCUE_H
