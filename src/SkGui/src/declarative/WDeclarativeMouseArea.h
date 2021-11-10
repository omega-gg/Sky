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

#ifndef WDECLARATIVEMOUSEAREA_H
#define WDECLARATIVEMOUSEAREA_H

#ifdef QT_4
// Qt includes
#include <QMouseEvent>
#endif

// Sk includes
#include <WDeclarativeItem>

#ifndef SK_NO_DECLARATIVEMOUSEAREA

// Forward declarations
class WDeclarativeMouseAreaPrivate;
class WDeclarativeDropEvent;

//-------------------------------------------------------------------------------------------------
// WDeclarativeMouseEvent
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeMouseEvent : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

    Q_PROPERTY(int x READ x)
    Q_PROPERTY(int y READ y)

    Q_PROPERTY(int button READ button)
    Q_PROPERTY(int buttons READ buttons)

    Q_PROPERTY(int modifiers READ modifiers)

    Q_PROPERTY(bool isClick READ isClick)
    Q_PROPERTY(bool wasHeld READ wasHeld)

public:
    /* virtual */ WDeclarativeMouseEvent(QMouseEvent::Type type, const QPoint & position,
                                         Qt::MouseButton button, Qt::MouseButtons buttons,
                                         Qt::KeyboardModifiers modifiers,
                                         bool isClick = false, bool wasHeld = false);

public: // Properties
    bool isAccepted() const;
    void setAccepted(bool accepted);

    QMouseEvent::Type type() const;

    int x() const;
    int y() const;

    int button()  const;
    int buttons() const;

    int modifiers() const;

    bool isClick() const;
    bool wasHeld() const;

private:
    QMouseEvent _event;

    bool _isClick;
    bool _wasHeld;

private:
    Q_DISABLE_COPY(WDeclarativeMouseEvent)
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeDrag
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeDrag : public QObject
{
    Q_OBJECT

    Q_ENUMS(Axis)

#ifdef QT_4
    Q_PROPERTY(QGraphicsObject * target READ target WRITE setTarget RESET resetTarget
               NOTIFY targetChanged)
#else
    Q_PROPERTY(QQuickItem * target READ target WRITE setTarget RESET resetTarget
               NOTIFY targetChanged)
#endif

    Q_PROPERTY(Axis axis READ axis WRITE setAxis NOTIFY axisChanged)

    Q_PROPERTY(qreal threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)

    Q_PROPERTY(qreal minimumX READ minimumX WRITE setMinimumX NOTIFY minimumXChanged)
    Q_PROPERTY(qreal maximumX READ maximumX WRITE setMaximumX NOTIFY maximumXChanged)

    Q_PROPERTY(qreal minimumY READ minimumY WRITE setMinimumY NOTIFY minimumYChanged)
    Q_PROPERTY(qreal maximumY READ maximumY WRITE setMaximumY NOTIFY maximumYChanged)

    Q_PROPERTY(bool active READ active NOTIFY activeChanged)

    Q_PROPERTY(bool filterChildren READ filterChildren WRITE setFilterChildren
               NOTIFY filterChildrenChanged)

public: // Enums
    enum Axis
    {
        XAxis     = 0x1,
        YAxis     = 0x2,
        XandYAxis = 0x3
    };

public:
    explicit WDeclarativeDrag(QObject * parent = NULL);

signals:
    void targetChanged();

    void axisChanged();

    void thresholdChanged();

    void minimumXChanged();
    void maximumXChanged();
    void minimumYChanged();
    void maximumYChanged();

    void activeChanged();

    void filterChildrenChanged();

public: // Properties
#ifdef QT_4
    QGraphicsObject * target() const;
    void              setTarget(QGraphicsObject * object);
#else
    QQuickItem * target() const;
    void         setTarget(QQuickItem * item);
#endif

    void resetTarget();

    Axis axis() const;
    void setAxis(Axis axis);

    qreal threshold() const;
    void  setThreshold(qreal threshold);

    qreal minimumX() const;
    void  setMinimumX(qreal minimumX);

    qreal maximumX() const;
    void  setMaximumX(qreal maximumY);

    qreal minimumY() const;
    void  setMinimumY(qreal minimumY);

    qreal maximumY() const;
    void  setMaximumY(qreal maximumY);

    bool active() const;
    void setActive(bool active);

    bool filterChildren() const;
    void setFilterChildren(bool filter);

private: // Variables
#ifdef QT_4
    QGraphicsObject * _target;
#else
    QQuickItem * _target;
#endif

    Axis _axis;

    qreal _threshold;

    qreal _minimumX;
    qreal _maximumX;

    qreal _minimumY;
    qreal _maximumY;

    bool _active : 1;

    bool _filterChildren : 1;

private:
    Q_DISABLE_COPY(WDeclarativeDrag)
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeMouseArea
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeMouseArea : public WDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(qreal mouseX READ mouseX NOTIFY mousePositionChanged)
    Q_PROPERTY(qreal mouseY READ mouseY NOTIFY mousePositionChanged)

    Q_PROPERTY(bool containsMouse READ hovered NOTIFY hoveredChanged)

    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

    Q_PROPERTY(Qt::MouseButtons pressedButtons READ pressedButtons NOTIFY pressedChanged)

    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons
               NOTIFY acceptedButtonsChanged)

    Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled
               NOTIFY hoverEnabledChanged)

    Q_PROPERTY(bool hoverRetain READ hoverRetain WRITE setHoverRetain NOTIFY hoverRetainChanged)

    Q_PROPERTY(bool wheelEnabled READ wheelEnabled WRITE setWheelEnabled
               NOTIFY wheelEnabledChanged)

    Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled NOTIFY dropEnabledChanged)

    Q_PROPERTY(bool dragAccepted READ dragAccepted NOTIFY dragAcceptedChanged)

    Q_PROPERTY(WDeclarativeDrag * drag READ drag CONSTANT)

    Q_PROPERTY(bool preventStealing READ preventStealing WRITE setPreventStealing
               NOTIFY preventStealingChanged /* REVISION 1 */)

    Q_PROPERTY(Qt::CursorShape cursor READ cursor WRITE setCursor NOTIFY cursorChanged)

    Q_PROPERTY(Qt::CursorShape cursorDrop READ cursorDrop WRITE setCursorDrop
               NOTIFY cursorDropChanged)

public:
#ifdef QT_4
    explicit WDeclarativeMouseArea(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeMouseArea(QQuickItem * parent = NULL);
#endif
protected:
#ifdef QT_4
    WDeclarativeMouseArea(WDeclarativeMouseAreaPrivate * p, QDeclarativeItem * parent = NULL);
#else
    WDeclarativeMouseArea(WDeclarativeMouseAreaPrivate * p, QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE void press  (Qt::MouseButton button = Qt::LeftButton);
    Q_INVOKABLE void release(Qt::MouseButton button = Qt::LeftButton);

    Q_INVOKABLE void click(Qt::MouseButton button = Qt::LeftButton);

    //---------------------------------------------------------------------------------------------
    // QML

    Q_INVOKABLE void press  (int button);
    Q_INVOKABLE void release(int button);

    Q_INVOKABLE void click(int button);

protected: // Functions
    void setHovered(bool hovered);
    bool setPressed(bool pressed);

#ifdef QT_4
    bool sendMouseEvent(QGraphicsSceneMouseEvent * event);
#else
    bool sendMouseEvent(QMouseEvent * event);
#endif

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_OLD
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);
#else
    /* virtual */ void geometryChange(const QRectF & newGeometry, const QRectF & oldGeometry);
#endif

#ifdef QT_4
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);
#else
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & value);
#endif

protected: // Events
#ifdef QT_4
    /* virtual */ void mousePressEvent  (QGraphicsSceneMouseEvent * event);
    /* virtual */ void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

    /* virtual */ void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

    /* virtual */ void mouseMoveEvent(QGraphicsSceneMouseEvent * event);

    /* virtual */ void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    /* virtual */ void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    /* virtual */ void hoverMoveEvent (QGraphicsSceneHoverEvent * event);

    /* virtual */ void wheelEvent(QGraphicsSceneWheelEvent * event);

#ifndef QT_NO_CONTEXTMENU
    /* virtual */ void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
#endif

    /* virtual */ bool sceneEvent(QEvent * event);

    /* virtual */ bool sceneEventFilter(QGraphicsItem * item, QEvent * event);
#else
    /* virtual */ void mousePressEvent  (QMouseEvent * event);
    /* virtual */ void mouseReleaseEvent(QMouseEvent * event);

    /* virtual */ void mouseDoubleClickEvent(QMouseEvent * event);

    /* virtual */ void mouseMoveEvent(QMouseEvent * event);

    /* virtual */ void mouseUngrabEvent();

    /* virtual */ void touchEvent(QTouchEvent * event);

    /* virtual */ void touchUngrabEvent();

    /* virtual */ void hoverEnterEvent(QHoverEvent * event);
    /* virtual */ void hoverLeaveEvent(QHoverEvent * event);
    /* virtual */ void hoverMoveEvent (QHoverEvent * event);

    /* virtual */ void wheelEvent(QWheelEvent * event);

    /* virtual */ bool childMouseEventFilter(QQuickItem * item, QEvent * event);

#ifdef QT_5
    /* virtual */ void windowDeactivateEvent();
#endif
#endif

    /* virtual */ void timerEvent(QTimerEvent * event);

signals:
    void hoveredChanged();
    void pressedChanged();

    void enabledChanged();

    void acceptedButtonsChanged();

    void hoverEnabledChanged();
    void hoverRetainChanged ();

    void wheelEnabledChanged();

    void dropEnabledChanged();

    void dragAcceptedChanged();

    /* Q_REVISION(1) */ void preventStealingChanged();

    void cursorChanged    ();
    void cursorDropChanged();

    void positionChanged(WDeclarativeMouseEvent * mouse);

    void mousePositionChanged();

    void pressed     (WDeclarativeMouseEvent * mouse);
    void pressAndHold(WDeclarativeMouseEvent * mouse);

    void released(WDeclarativeMouseEvent * mouse);

    void clicked      (WDeclarativeMouseEvent * mouse);
    void doubleClicked(WDeclarativeMouseEvent * mouse);

    void entered();
    void exited ();

    void dragEntered(WDeclarativeDropEvent * event);
    void dragExited (WDeclarativeDropEvent * event);

    void dragMove(WDeclarativeDropEvent * event);

    void drop(WDeclarativeDropEvent * event);

    void canceled();

    void wheeled(qreal steps);

public: // Properties
    qreal mouseX() const;
    qreal mouseY() const;

    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool hovered() const;
    bool pressed() const;

    Qt::MouseButtons pressedButtons() const;

    Qt::MouseButtons acceptedButtons() const;
    void             setAcceptedButtons(Qt::MouseButtons buttons);

    bool hoverEnabled() const;
    void setHoverEnabled(bool enabled);

    bool hoverRetain() const;
    void setHoverRetain(bool retain);

    bool wheelEnabled() const;
    void setWheelEnabled(bool enabled);

    bool dropEnabled() const;
    void setDropEnabled(bool enabled);

    bool dragAccepted() const;

    WDeclarativeDrag * drag();

    bool preventStealing() const;
    void setPreventStealing(bool prevent);

    Qt::CursorShape cursor();
    void            setCursor(Qt::CursorShape shape);

    Qt::CursorShape cursorDrop();
    void            setCursorDrop(Qt::CursorShape shape);

private: // Functions
    void handlePress  ();
    void handleRelease();

private:
    W_DECLARE_PRIVATE(WDeclarativeMouseArea)

    friend class WView;
    friend class WViewPrivate;
};

#endif // SK_NO_DECLARATIVEMOUSEAREA
#endif // WDECLARATIVEMOUSEAREA_H
