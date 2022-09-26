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

#ifndef WDECLARATIVEMOUSEAREA_P_H
#define WDECLARATIVEMOUSEAREA_P_H

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_DECLARATIVEMOUSEAREA

// Forward declarations
struct WViewDragData;

class SK_GUI_EXPORT WDeclarativeMouseAreaPrivate : public WPrivate
{
public:
    WDeclarativeMouseAreaPrivate(WDeclarativeMouseArea * p);

    /* virtual */ ~WDeclarativeMouseAreaPrivate();

    void init();

public: // Functions
    bool dragEnterEvent(const QPointF & pos, WViewDragData * data);
    void dragLeaveEvent();

    void dragMoveEvent(const QPointF & pos, const WViewDragData & data);

    void dropEvent(const QPointF & pos, const WViewDragData & data);

    void setHoverActive(bool active);

    void setDragAccepted(bool accepted);

    void clearHover();
    void clearDrop ();

    void clearView();

public: // Events
    void onHoverEnabledChanged();

public: // Variables
    WView * view;

#ifdef QT_4
    QDeclarativeItem * viewport;
#endif

    bool hoverActive;
    bool hoverRetain;

    bool wheelEnabled;

#ifdef QT_NEW
    bool dropEnabled;
#endif

    bool dragAccepted;

    Qt::CursorShape cursor;
    Qt::CursorShape cursorDrop;

protected:
    W_DECLARE_PUBLIC(WDeclarativeMouseArea)
};

#endif // SK_NO_DECLARATIVEMOUSEAREA
#endif // WDECLARATIVEMOUSEAREA_P_H
