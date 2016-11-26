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

#ifndef WVIEW_P_H
#define WVIEW_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QTimer>

// Sk includes
#include <private/WAbstractView_p>
#include <WDeclarativeMouseArea>

#ifndef SK_NO_VIEW

// Forward declarations
class QDrag;
class WDeclarativeMouseArea;
class WResizer;

//-------------------------------------------------------------------------------------------------
// WViewDrag
//-------------------------------------------------------------------------------------------------

struct WViewDragData
{
    QString text;

    Qt::DropActions actions;
    Qt::DropAction  action;
};

//-------------------------------------------------------------------------------------------------
// WViewPrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WViewPrivate : public WAbstractViewPrivate
{
public:
    WViewPrivate(WView * p);

    /* virtual */ ~WViewPrivate();

    void init(QDeclarativeItem * item);

public: // Functions
    void startFade(bool visible);

    void fadeIn ();
    void fadeOut();

    void applySize(int width, int height);

    void updateFlags();

    void updateViewport();

    void updateMinimumSize();

    void updateHoverable();

    void updateMouse();

    void updateDrag();
    void clearDrag ();

    void setActive(bool active);

    void setEntered (bool entered);
    void setPressed (bool pressed);
    void setDragging(bool dragging);

    void setDragged (bool dragged);
    void setResizing(bool resizing);

    QRect getGeometryDefault(const QRect & rect) const;
    QRect getGeometry       (const QRect & rect) const;

    void setGeometryNormal(const QRect & rect);

    void setMousePos(const QPoint & pos);

    bool isUnderMouse() const;

    void setCursor  (WDeclarativeMouseArea::CursorShape shape);
    void applyCursor(WDeclarativeMouseArea::CursorShape shape);

    void setKeyShiftPressed  (bool pressed);
    void setKeyControlPressed(bool pressed);
    void setKeyAltPressed    (bool pressed);

    QList<WDeclarativeMouseArea *> getMouseAreas(const QList<QGraphicsItem *> & items) const;
    QList<WDeclarativeMouseArea *> getDropAreas (const QList<QGraphicsItem *> & items) const;

public: // Slots
    void onGeometryChanged();

    void onFadeTimeout();
    void onIdleTimeout();

    void onCursorVisibleChanged();

public: // Variables
    WViewScene * scene;

    QDeclarativeItem * item;

    WResizer * currentResizer;

    qreal zoom;

    int minimumWidth;
    int minimumHeight;

    int maximumWidth;
    int maximumHeight;

    QRect geometryNormal;

    bool active;

    bool minimized;
    bool maximized;
    bool fullScreen;
    bool locked;
    bool closed;

    bool autoSize;

    bool opengl;
    bool antialias;
    bool vsync;

    QTimer timerLeave;

    //---------------------------------------------------------------------------------------------
    // Fade

    bool fade;

    bool fadeVisible;

    int   fadeDuration;
    qreal fadeValue;

    QTimer fadeTimer;

    //---------------------------------------------------------------------------------------------
    // Mouse

    bool entered;
    bool pressed;
    bool dragging;

    bool hover;
    bool hoverable;

    bool dragged;
    bool resizing;

    bool mouseAccepted;

    QPoint mousePos;

    Qt::MouseButton  button;
    Qt::MouseButtons buttons;

    WDeclarativeMouseArea::CursorShape cursor;

    QHash<WDeclarativeMouseArea::CursorShape, QCursor> cursors;

    QList<QGraphicsItem         *> itemsCursor;
    QList<WDeclarativeMouseArea *> itemsHovered;

    bool   idleCheck;
    bool   idle;
    int    idleDelay;
    QTimer idleTimer;

    //---------------------------------------------------------------------------------------------
    // Drag

    QDrag * drag;

    QMimeData * mime;

    WViewDragData dragData;

    QList<QGraphicsItem *> itemsDrop;

    WDeclarativeMouseArea * areaDrop;

    //---------------------------------------------------------------------------------------------
    // Keys

#ifdef Q_OS_WIN
    quint32 keyVirtual;
    quint32 keyScan;
#endif

    bool keyShiftPressed;
    bool keyControlPressed;
    bool keyAltPressed;

    bool keyAccepted;

protected:
    W_DECLARE_PUBLIC(WView)
};

#endif // SK_NO_VIEW
#endif // WVIEW_P_H
