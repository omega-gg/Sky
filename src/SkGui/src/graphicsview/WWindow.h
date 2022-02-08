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

#ifndef WWINDOW_H
#define WWINDOW_H

// Sk includes
#include <WView>

#ifndef SK_NO_WINDOW

// Forward declarations
class WViewportPrivate;
class WWindowPrivate;

//-------------------------------------------------------------------------------------------------
// WViewport
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WViewport : public WDeclarativeMouseArea
{
    Q_OBJECT

public:
#ifdef QT_4
    explicit WViewport(QDeclarativeItem * parent = NULL);
#else
    explicit WViewport(QQuickItem * parent = NULL);
#endif

protected: // Events
    /* virtual */ void keyPressEvent  (QKeyEvent * event);
    /* virtual */ void keyReleaseEvent(QKeyEvent * event);

private:
    W_DECLARE_PRIVATE(WViewport)
};

//-------------------------------------------------------------------------------------------------
// WWindow
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WWindow : public WView
{
    Q_OBJECT

    Q_PROPERTY(WDeclarativeMouseArea * viewport READ viewport CONSTANT)

#ifdef QT_4
    Q_PROPERTY(QDeclarativeListProperty<QObject> children READ children)
#else
    Q_PROPERTY(QQmlListProperty<QObject> children READ children)
#endif

    Q_CLASSINFO("DefaultProperty", "children")

    //---------------------------------------------------------------------------------------------

    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)

    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

    // NOTE: Some properties need to be skipped because they already exist in WView.
    //---------------------------------------------------------------------------------------------
    // WDeclarativeMouseArea

    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)

    //Q_PROPERTY(qreal mouseX READ mouseX NOTIFY mousePositionChanged)
    //Q_PROPERTY(qreal mouseY READ mouseY NOTIFY mousePositionChanged)

    Q_PROPERTY(bool containsMouse READ hovered NOTIFY hoveredChanged)

    Q_PROPERTY(bool pressed READ pressed NOTIFY mousePressedChanged)

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

    Q_PROPERTY(Qt::MouseButtons pressedButtons READ pressedButtons NOTIFY mousePressedChanged)

    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons
               NOTIFY acceptedButtonsChanged)

    //Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled
    //           NOTIFY hoverEnabledChanged)

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
    explicit WWindow(QWidget * parent = NULL);
#else
    explicit WWindow(QWindow * parent = NULL);
#endif

public: // Interface
    //---------------------------------------------------------------------------------------------
    // Focus

    Q_INVOKABLE bool getFocus() const;

    Q_INVOKABLE void setFocus(bool focus);

    Q_INVOKABLE void clearFocus();

#ifdef QT_4
    Q_INVOKABLE void clearFocusItem(QDeclarativeItem * item);
#else
    Q_INVOKABLE void clearFocusItem(QQuickItem * item);
#endif

    //---------------------------------------------------------------------------------------------
    // WDeclarativeMouseArea
    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void press  (Qt::MouseButton button = Qt::LeftButton);
    Q_INVOKABLE void release(Qt::MouseButton button = Qt::LeftButton);

    Q_INVOKABLE void click(Qt::MouseButton button = Qt::LeftButton);

    //---------------------------------------------------------------------------------------------
    // QML

    Q_INVOKABLE void press  (int button);
    Q_INVOKABLE void release(int button);

    Q_INVOKABLE void click(int button);

public: // Static functions
#if defined(QT_NEW) && defined(Q_OS_ANDROID)
    Q_INVOKABLE static void hideSplash(int duration = 0);
#endif

protected: // Events
//#ifdef QT_4
//    /* virtual */ void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
//    /* virtual */ void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
//#else
//    /* virtual */ void hoverEnterEvent(QHoverEvent * event);
//    /* virtual */ void hoverLeaveEvent(QHoverEvent * event);

//    /* virtual */ void dragEnterEvent(QDragEnterEvent * event);
//    /* virtual */ void dragLeaveEvent(QDragLeaveEvent * event);

//    /* virtual */ void dragMoveEvent(QDragMoveEvent * event);

//    /* virtual */ void dropEvent(QDropEvent * event);
//#endif

private: // Declarative
#ifdef QT_4
    static void childrenAppend(QDeclarativeListProperty<QObject> * property, QObject * item);
    static void childrenClear (QDeclarativeListProperty<QObject> * property);

    static int childrenCount(QDeclarativeListProperty<QObject> * property);

    static QObject * childrenAt(QDeclarativeListProperty<QObject> * property, int index);
#else
    static void childrenAppend(QQmlListProperty<QObject> * property, QObject * item);
    static void childrenClear (QQmlListProperty<QObject> * property);

#ifdef QT_5
    static int childrenCount(QQmlListProperty<QObject> * property);

    static QObject * childrenAt(QQmlListProperty<QObject> * property, int index);
#else // QT_6
    static qsizetype childrenCount(QQmlListProperty<QObject> * property);

    static QObject * childrenAt(QQmlListProperty<QObject> * property, qsizetype index);
#endif
#endif

signals:
    void iconChanged();

    void visibleChanged();

    void opacityChanged();

    // NOTE: Some signals need to be renamed because they already exist in WView.
    //---------------------------------------------------------------------------------------------
    // WDeclarativeMouseArea

    void scaleChanged();

    void hoveredChanged();

    void mousePressedChanged();

    void enabledChanged();

    void acceptedButtonsChanged();

    //void hoverEnabledChanged();
    void hoverRetainChanged ();

    void wheelEnabledChanged();

    void dropEnabledChanged();

    void dragAcceptedChanged();

    /* Q_REVISION(1) */ void preventStealingChanged();

    void cursorChanged    ();
    void cursorDropChanged();

    void positionChanged(WDeclarativeMouseEvent * mouse);

    //void mousePositionChanged();

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
    WDeclarativeMouseArea * viewport() const;

#ifdef QT_4
    QDeclarativeListProperty<QObject> children();
#else
    QQmlListProperty<QObject> children();
#endif

    QString icon() const;
    void    setIcon(const QString & icon);

    bool isVisible() const;
    void setVisible(bool visible);

    qreal opacity() const;
    void  setOpacity(qreal opacity);

    void setLocked(bool locked);

    //---------------------------------------------------------------------------------------------
    // WDeclarativeMouseArea

    qreal scale() const;
    void  setScale(qreal scale);

    //qreal mouseX() const;
    //qreal mouseY() const;

    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool hovered() const;
    bool pressed() const;

    Qt::MouseButtons pressedButtons() const;

    Qt::MouseButtons acceptedButtons() const;
    void             setAcceptedButtons(Qt::MouseButtons buttons);

    //bool hoverEnabled() const;
    //void setHoverEnabled(bool enabled);

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

    Qt::CursorShape cursor() const;
    void            setCursor(Qt::CursorShape shape);

    Qt::CursorShape cursorDrop() const;
    void            setCursorDrop(Qt::CursorShape shape);

private:
    W_DECLARE_PRIVATE(WWindow)
};

#endif // SK_NO_WINDOW
#endif // WWINDOW_H
