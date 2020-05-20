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

#ifndef WDECLARATIVEMOUSEWATCHER_P_H
#define WDECLARATIVEMOUSEWATCHER_P_H

#include <private/WDeclarativeItem_p>

#ifndef SK_NO_DECLARATIVEMOUSEWATCHER

class SK_GUI_EXPORT WDeclarativeMouseWatcherPrivate : public WDeclarativeItemPrivate
{
public:
    WDeclarativeMouseWatcherPrivate(WDeclarativeMouseWatcher * p);

    void init();

public: // Slots
    void onPressedChanged();

protected:
    W_DECLARE_PUBLIC(WDeclarativeMouseWatcher)
};

#endif // SK_NO_DECLARATIVEMOUSEWATCHER
#endif // WDECLARATIVESCALEMOUSEWATCHER_P_H
