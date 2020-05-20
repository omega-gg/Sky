//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

#ifndef WDECLARATIVEMOUSEWATCHER_H
#define WDECLARATIVEMOUSEWATCHER_H

// Sk includes
#include <WDeclarativeItem>

#ifndef SK_NO_DECLARATIVEMOUSEWATCHER

class WDeclarativeMouseWatcherPrivate;

class SK_GUI_EXPORT WDeclarativeMouseWatcher : public WDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons
               NOTIFY acceptedButtonsChanged)

public:
#ifdef QT_4
    explicit WDeclarativeMouseWatcher(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeMouseWatcher(QQuickItem * parent = NULL);
#endif

protected: // Events
#ifdef QT_4
    /* virtual */ void mousePressEvent(QGraphicsSceneMouseEvent * event);
#else
    /* virtual */ void mousePressEvent(QMouseEvent * event);
#endif

signals:
    void pressed ();
    void released();

    void acceptedButtonsChanged();

public: // Properties
    Qt::MouseButtons acceptedButtons() const;
    void             setAcceptedButtons(Qt::MouseButtons buttons);

private:
    W_DECLARE_PRIVATE(WDeclarativeMouseWatcher)

    Q_PRIVATE_SLOT(d_func(), void onPressedChanged())
};

#include <private/WDeclarativeMouseWatcher_p>

#endif // SK_NO_DECLARATIVEMOUSEWATCHER
#endif // WDECLARATIVESCALEMOUSEWATCHER_H
