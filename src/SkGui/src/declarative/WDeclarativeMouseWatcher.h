//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
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
    explicit WDeclarativeMouseWatcher(QDeclarativeItem * parent = NULL);

signals:
    void pressed ();
    void released();

    void acceptedButtonsChanged();

protected: // Events
    /* virtual */ void mousePressEvent(QGraphicsSceneMouseEvent * event);

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
