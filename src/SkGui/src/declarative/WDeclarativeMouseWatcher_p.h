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

#ifndef WDECLARATIVEMOUSEWATCHER_P_H
#define WDECLARATIVEMOUSEWATCHER_P_H

#ifdef QT_NEW
// Qt includes
#include <QTimer>
#endif

// Private includes
#include <private/WDeclarativeItem_p>

#ifndef SK_NO_DECLARATIVEMOUSEWATCHER

class SK_GUI_EXPORT WDeclarativeMouseWatcherPrivate : public WDeclarativeItemPrivate
{
public:
    WDeclarativeMouseWatcherPrivate(WDeclarativeMouseWatcher * p);

    void init();

public: // Slots
    void onPressedChanged();

#ifdef QT_NEW
public: // Variables
#ifdef QT_5
    bool touch;
#endif

    QTimer timer;
#endif

protected:
    W_DECLARE_PUBLIC(WDeclarativeMouseWatcher)
};

#endif // SK_NO_DECLARATIVEMOUSEWATCHER
#endif // WDECLARATIVESCALEMOUSEWATCHER_P_H
