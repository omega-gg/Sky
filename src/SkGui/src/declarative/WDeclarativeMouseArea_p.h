//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WDECLARATIVEMOUSEAREA_P_H
#define WDECLARATIVEMOUSEAREA_P_H

// Qt includes
#include <QBasicTimer>

// Sk includes
#include <private/WDeclarativeItem_p>

#ifndef SK_NO_DECLARATIVEMOUSEAREA

class SK_GUI_EXPORT WDeclarativeMouseAreaPrivate : public WDeclarativeItemPrivate
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

    void saveEvent(QGraphicsSceneMouseEvent * event);

    bool isPressAndHoldConnected();
    bool isDoubleClickConnected ();

    void setDragAccepted(bool accepted);

    void clearHover();
    void clearDrop ();

    void clearView();

public: // Variables
    bool absorb : 1;

    bool wheelEnabled : 1;

    bool dragAccepted : 1;

    bool preventStealing : 1;

    bool hoverEnabled: 1;
    bool hoverRetain : 1;

    bool moved : 1;

    bool hovered : 1;

    bool pressed   : 1;
    bool longPress : 1;

    bool doubleClick : 1;

    bool dragX : 1;
    bool dragY : 1;

    bool stealMouse : 1;

    WDeclarativeDrag * drag;

    QPointF startScene;

    qreal startX;
    qreal startY;

    QPointF lastPos;
    QPointF lastScenePos;

    Qt::MouseButton  lastButton;
    Qt::MouseButtons lastButtons;

    Qt::KeyboardModifiers lastModifiers;

    QBasicTimer pressAndHoldTimer;

    WDeclarativeMouseArea::CursorShape cursor;
    WDeclarativeMouseArea::CursorShape cursorDrop;

protected:
    W_DECLARE_PUBLIC(WDeclarativeMouseArea)
};

#endif // SK_NO_DECLARATIVEMOUSEAREA
#endif // WDECLARATIVEMOUSEAREA_P_H
