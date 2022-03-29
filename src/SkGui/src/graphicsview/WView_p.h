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
#include <WDeclarativeMouseArea>

// Private includes
#include <private/WAbstractView_p>

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

#ifdef QT_4
    void init(QDeclarativeItem * item);
#else
    void init(QQuickItem * item);
#endif

public: // Functions
    void startFade(bool visible);

    void fadeIn ();
    void fadeOut();

    void applySize(int width, int height);

#ifdef QT_NEW
    void updateRatio();
#endif

    void applyDrop();

    void updateFlags();

#ifdef QT_4
    void updateViewport();
#endif

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

#ifdef QT_NEW
    void setTouch(int id);
#endif

#ifdef SK_DESKTOP
    QRect getGeometryDefault(const QRect & rect) const;
    QRect getGeometry       (const QRect & rect) const;
#endif

    void setGeometryNormal(const QRect & rect);

    void setMousePos(const QPoint & pos);

    bool isUnderMouse() const;

#ifdef QT_NEW
    bool itemUnderMouse(QQuickItem * item) const;
#endif

    void setCursor  (Qt::CursorShape shape);
    void applyCursor(Qt::CursorShape shape);

    void setKeyShiftPressed  (bool pressed);
    void setKeyControlPressed(bool pressed);
    void setKeyAltPressed    (bool pressed);

#ifdef QT_4
    QList<WDeclarativeMouseArea *> getMouseAreas(const QList<QGraphicsItem *> & items) const;
    QList<WDeclarativeMouseArea *> getDropAreas (const QList<QGraphicsItem *> & items) const;
#else
    void getItems(QList<QQuickItem *> * items, QQuickItem * item, const QPoint & pos) const;

    QList<WDeclarativeMouseArea *> getMouseAreas(const QList<QQuickItem *> & items) const;
    QList<WDeclarativeMouseArea *> getDropAreas (const QList<QQuickItem *> & items) const;
#endif

public: // Slots
#ifdef QT_NEW
    void onStateChanged(Qt::ApplicationState state);
#endif

    void onGeometryChanged();

#ifdef QT_NEW
    void onScreenChanged();
#endif

    void onFadeTimeout();
    void onIdleTimeout();

#ifdef SK_MOBILE
    void onMessageUpdated();
#endif

    void onClose();

    void onCursorVisibleChanged();

public: // Variables
#ifdef QT_4
    WViewScene * scene;

    QDeclarativeItem * item;
#else
    QQuickItem * item;
#endif

    WResizer * currentResizer;

    qreal ratio;
    qreal ratioPixel;

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
    bool scaling;
    bool closed;

    bool opengl;
    bool antialias;
    bool vsync;

#ifdef QT_4
    //QTimer timerLeave;
#endif

    //---------------------------------------------------------------------------------------------
    // Fade

    bool fade;

    bool fadeVisible;

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
    bool touching;

    bool mouseAccepted;
    bool dragAccepted;

    bool resetHover;

    QPoint mousePos;

    Qt::MouseButton  button;
    Qt::MouseButtons buttons;

    Qt::CursorShape cursor;

    QHash<Qt::CursorShape, QCursor> cursors;

#ifdef QT_4
    QList<QGraphicsItem *> itemsCursor;
#else
    QList<QQuickItem *> itemsCursor;
#endif

    QList<WDeclarativeMouseArea *> itemsHovered;

    bool   idleCheck;
    bool   idle;
    int    idleDelay;
    QTimer idleTimer;

#ifdef QT_NEW
    //---------------------------------------------------------------------------------------------
    // Touch

    int touchId;

    WDeclarativeMouseArea * touchItem;
    QTimer                  touchTimer;
#endif

    //---------------------------------------------------------------------------------------------
    // Drag

    QDrag * drag;

    QMimeData * mime;

    WViewDragData dragData;

#ifdef QT_4
    QList<QGraphicsItem *> itemsDrop;
#else
    QList<QQuickItem *> itemsDrop;
#endif

    WDeclarativeMouseArea * areaDrop;

#ifdef QT_NEW
    bool dragSkip;
#endif

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
