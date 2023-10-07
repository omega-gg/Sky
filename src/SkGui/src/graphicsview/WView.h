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

#ifndef WVIEW_H
#define WVIEW_H

// Qt includes
#include <QKeyEvent>

// Sk includes
#include <WAbstractView>
#include <WDeclarativeMouseArea>

#ifndef SK_NO_VIEW

// Forward declarations
class QDeclarativeItem;
class WViewPrivate;

//-------------------------------------------------------------------------------------------------
// WDeclarativeKeyEvent
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeKeyEvent : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

    Q_PROPERTY(int key READ key)

    Q_PROPERTY(QString text READ text)

    Q_PROPERTY(int modifiers READ modifiers)

    Q_PROPERTY(bool isAutoRepeat READ isAutoRepeat)

    Q_PROPERTY(int count READ count)

public:
    WDeclarativeKeyEvent(QEvent::Type            type,
                         int                     key,
                         Qt::KeyboardModifiers   modifiers  = Qt::NoModifier,
                         const QString         & text       = QString(),
                         bool                    autoRepeat = false,
                         ushort                  count      = 1);

    WDeclarativeKeyEvent(const QKeyEvent & event);

public: // Properties
    bool isAccepted() const;
    void setAccepted(bool accepted);

    int key() const;

    int modifiers() const;

    QString text() const;

    bool isAutoRepeat() const;

    int count() const;

private:
    bool _isAccepted;

    QEvent::Type _type;

    int _key;

    int _modifiers;

    QString _text;

    bool _autoRepeat;

    int _count;
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeDropEvent
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeDropEvent : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

    Q_PROPERTY(qreal x READ x)
    Q_PROPERTY(qreal y READ y)

    Q_PROPERTY(QString text READ text WRITE setText)

    Q_PROPERTY(Qt::DropActions actions READ actions)
    Q_PROPERTY(Qt::DropAction  action  READ action WRITE setAction)

public:
    WDeclarativeDropEvent(qreal x, qreal y, const QString & text,
                          Qt::DropActions actions = Qt::IgnoreAction,
                          Qt::DropAction  action  = Qt::IgnoreAction);

    WDeclarativeDropEvent(qreal x, qreal y, const WViewDragData & data);

public: // Properties
    bool isAccepted() const;
    void setAccepted(bool accepted);

    qreal x() const;
    qreal y() const;

    QString text() const;
    void    setText(const QString & text);

    Qt::DropActions actions() const;

    Qt::DropAction action() const;
    void           setAction(Qt::DropAction action);

private: // Variables
    bool _accepted;

    qreal _x;
    qreal _y;

    QString _text;

    Qt::DropActions _actions;
    Qt::DropAction  _action;
};

//-------------------------------------------------------------------------------------------------
// WView
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WView : public WAbstractView
{
    Q_OBJECT

#ifdef QT_4
    Q_PROPERTY(QDeclarativeItem * item READ item NOTIFY itemChanged)
#else
    Q_PROPERTY(QQuickItem * item READ item NOTIFY itemChanged)
#endif

    Q_PROPERTY(qreal itemWidth  READ itemWidth  NOTIFY itemWidthChanged)
    Q_PROPERTY(qreal itemHeight READ itemHeight NOTIFY itemHeightChanged)

    Q_PROPERTY(Qt::WindowFlags flags READ flags WRITE setFlags NOTIFY flagsChanged)

    Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int y READ y WRITE setY NOTIFY yChanged)

    Q_PROPERTY(int width  READ width  WRITE setWidth  NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)

    Q_PROPERTY(int centerX READ centerX NOTIFY centerXChanged)
    Q_PROPERTY(int centerY READ centerY NOTIFY centerYChanged)

    Q_PROPERTY(qreal originX READ originX WRITE setOriginX NOTIFY originXChanged)
    Q_PROPERTY(qreal originY READ originY WRITE setOriginY NOTIFY originYChanged)

    Q_PROPERTY(qreal ratio      READ ratio      NOTIFY ratioChanged)
    Q_PROPERTY(qreal ratioPixel READ ratioPixel NOTIFY ratioPixelChanged)

    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

    Q_PROPERTY(int minimumWidth READ minimumWidth WRITE setMinimumWidth
               NOTIFY minimumWidthChanged)

    Q_PROPERTY(int minimumHeight READ minimumHeight WRITE setMinimumHeight
               NOTIFY minimumHeightChanged)

    Q_PROPERTY(int maximumWidth READ maximumWidth WRITE setMaximumWidth
               NOTIFY maximumWidthChanged)

    Q_PROPERTY(int maximumHeight READ maximumHeight WRITE setMaximumHeight
               NOTIFY maximumHeightChanged)

    Q_PROPERTY(QRect geometryNormal READ geometryNormal NOTIFY geometryNormalChanged)

    Q_PROPERTY(bool minimized  READ isMinimized  WRITE setMinimized  NOTIFY minimizedChanged)
    Q_PROPERTY(bool maximized  READ isMaximized  WRITE setMaximized  NOTIFY maximizedChanged)
    Q_PROPERTY(bool fullScreen READ isFullScreen WRITE setFullScreen NOTIFY fullScreenChanged)
    Q_PROPERTY(bool locked     READ isLocked     WRITE setLocked     NOTIFY lockedChanged)
    Q_PROPERTY(bool scaling    READ isScaling    WRITE setScaling    NOTIFY scalingChanged)

    Q_PROPERTY(bool isActive   READ isActive   NOTIFY activeChanged)
    Q_PROPERTY(bool isEntered  READ isEntered  NOTIFY enteredChanged)
    Q_PROPERTY(bool isPressed  READ isPressed  NOTIFY pressedChanged)
    Q_PROPERTY(bool isDragging READ isDragging NOTIFY draggingChanged)
    Q_PROPERTY(bool isDragged  READ isDragged  NOTIFY draggedChanged)
    Q_PROPERTY(bool isResizing READ isResizing NOTIFY resizingChanged)
    Q_PROPERTY(bool isTouching READ isTouching NOTIFY touchingChanged)

    Q_PROPERTY(QPoint mousePos READ mousePos NOTIFY mousePosChanged)

    Q_PROPERTY(int mouseX READ mouseX NOTIFY mousePosChanged)
    Q_PROPERTY(int mouseY READ mouseY NOTIFY mousePosChanged)

    Q_PROPERTY(Qt::CursorShape mouseCursor READ mouseCursor NOTIFY mouseCursorChanged)

#ifdef QT_4
    Q_PROPERTY(bool opengl READ opengl WRITE setOpengl NOTIFY openglChanged)
#else
    Q_PROPERTY(bool opengl READ opengl CONSTANT)
#endif

    Q_PROPERTY(bool antialias READ antialias WRITE setAntialias NOTIFY antialiasChanged)
    Q_PROPERTY(bool vsync     READ vsync     WRITE setVsync     NOTIFY vsyncChanged)

    Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled
               NOTIFY hoverEnabledChanged)

    Q_PROPERTY(bool fadeEnabled READ fadeEnabled WRITE setFadeEnabled NOTIFY fadeEnabledChanged)

    //---------------------------------------------------------------------------------------------
    // Mouse idle

    Q_PROPERTY(bool idleCheck READ idleCheck WRITE setIdleCheck NOTIFY idleCheckChanged)
    Q_PROPERTY(bool idle      READ idle      WRITE setIdle      NOTIFY idleChanged)
    Q_PROPERTY(int  idleDelay READ idleDelay WRITE setIdleDelay NOTIFY idleDelayChanged)

    //---------------------------------------------------------------------------------------------
    // Keys

    Q_PROPERTY(bool keyShiftPressed   READ keyShiftPressed   NOTIFY keyShiftPressedChanged)
    Q_PROPERTY(bool keyControlPressed READ keyControlPressed NOTIFY keyControlPressedChanged)
    Q_PROPERTY(bool keyAltPressed     READ keyAltPressed     NOTIFY keyAltPressedChanged)

    //---------------------------------------------------------------------------------------------
    // Screen

    Q_PROPERTY(QRect availableGeometry READ availableGeometry NOTIFY availableGeometryChanged)
    Q_PROPERTY(QRect screenGeometry    READ screenGeometry    NOTIFY availableGeometryChanged)

public:
#ifdef QT_4
    WView(QDeclarativeItem * item, QWidget * parent = NULL, Qt::WindowFlags flags = Qt::Widget);

    WView(QWidget * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#else
    WView(QQuickItem * item, QWindow * parent = NULL, Qt::WindowFlags flags = Qt::Widget);

    WView(QWindow * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#endif
protected:
#ifdef QT_4
    WView(WViewPrivate     * p,
          QDeclarativeItem * item, QWidget * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#else
    WView(WViewPrivate * p,
          QQuickItem   * item, QWindow * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#endif

public: // Interface
    Q_INVOKABLE void activate();
    Q_INVOKABLE void raise   ();

    Q_INVOKABLE void centerWindow();

    Q_INVOKABLE void moveToScreen(int number);

    Q_INVOKABLE bool close();

    //---------------------------------------------------------------------------------------------
    // Geometry

    Q_INVOKABLE int screenCount () const;
    Q_INVOKABLE int screenNumber() const;

    Q_INVOKABLE void setMinimumSize(int width, int height);
    Q_INVOKABLE void setMaximumSize(int width, int height);

    Q_INVOKABLE QRect getDefaultGeometry() const;
    Q_INVOKABLE void  setDefaultGeometry();

    Q_INVOKABLE void saveGeometry();

    Q_INVOKABLE void checkPosition();

    Q_INVOKABLE void originTo(qreal x, qreal y);

    //---------------------------------------------------------------------------------------------
    // Drag

    Q_INVOKABLE bool testDrag(const QPointF & posA, const QPointF & posB, qreal distance);

    Q_INVOKABLE void startDrag(const QString & text, int actions = Qt::CopyAction);

    //---------------------------------------------------------------------------------------------
    // Hover

    Q_INVOKABLE int hoverCount() const;

    Q_INVOKABLE void updateHover();
    Q_INVOKABLE void clearHover ();

#ifdef QT_4
    //Q_INVOKABLE void checkLeave(int msec);
#endif

    //---------------------------------------------------------------------------------------------
    // Shot

    // NOTE: writeShot is a convenient function that creates the 'path' folder and writes a
    //       threaded screenshot of the entire view with a time stamped file name.

#ifdef QT_4
    Q_INVOKABLE QImage takeShot(int x = 0, int y = 0, int width = -1, int height = -1) const;

    Q_INVOKABLE bool saveShot(const QString & fileName,
                              int x = 0, int y = 0, int width = -1, int height = -1,
                              const QString & format = "png", int quality = -1) const;

    Q_INVOKABLE void writeShot(const QString & path,
                               const QString & format = "png", int quality = -1) const;
#else
    Q_INVOKABLE QImage takeShot(int x = 0, int y = 0, int width = -1, int height = -1);

    Q_INVOKABLE bool saveShot(const QString & fileName,
                              int x = 0, int y = 0, int width = -1, int height = -1,
                              const QString & format = "png", int quality = -1);

    Q_INVOKABLE void writeShot(const QString & path,
                               const QString & format = "png", int quality = -1);
#endif

    //---------------------------------------------------------------------------------------------
    // Cursor

    Q_INVOKABLE void registerCursor(Qt::CursorShape shape, const QCursor & cursor);

    Q_INVOKABLE void unregisterCursor (Qt::CursorShape shape);
    Q_INVOKABLE void unregisterCursors();

    //---------------------------------------------------------------------------------------------
    // Input

    Q_INVOKABLE void mouseMove(int x, int y, Qt::MouseButton button = Qt::NoButton);

    Q_INVOKABLE void mousePress  (Qt::MouseButton button = Qt::LeftButton);
    Q_INVOKABLE void mouseRelease(Qt::MouseButton button = Qt::LeftButton);

    Q_INVOKABLE void mouseClick(Qt::MouseButton button = Qt::LeftButton, int msec = 100);

    Q_INVOKABLE void wheel(Qt::Orientation orientation = Qt::Vertical, int delta = -120);

    Q_INVOKABLE void wheelUp  (int delta =  120);
    Q_INVOKABLE void wheelDown(int delta = -120);

    Q_INVOKABLE void keyPress  (int key, Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    Q_INVOKABLE void keyRelease(int key, Qt::KeyboardModifiers modifiers = Qt::NoModifier);

    Q_INVOKABLE void keyClick(int key, Qt::KeyboardModifiers modifiers = Qt::NoModifier,
                                       int                   msec      = 100);

    //---------------------------------------------------------------------------------------------
    // QML

    Q_INVOKABLE void mouseMove(int x, int y, int button);

    Q_INVOKABLE void mousePress  (int button);
    Q_INVOKABLE void mouseRelease(int button);

    Q_INVOKABLE void mouseClick(int button, int msec = 100);

    Q_INVOKABLE void wheel(int orientation, int delta = -120);

    Q_INVOKABLE void keyPress  (int key, int modifiers);
    Q_INVOKABLE void keyRelease(int key, int modifiers);

    Q_INVOKABLE void keyClick(int key, int modifiers, int msec = 100);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void registerCursorUrl(int shape, const QString & url,
                                                  const QSize   & size = QSize());

    Q_INVOKABLE void unregisterCursor(int shape);

public: // Static functions
#ifdef QT_4
    Q_INVOKABLE static QPixmap takeItemShot(QGraphicsObject * item,
                                            const QColor    & background = Qt::transparent);

    Q_INVOKABLE static bool saveItemShot(const QString   & fileName,
                                         QGraphicsObject * item,
                                         const QString   & format = "png", int quality = -1,
                                         const QColor    & background = Qt::transparent);
#else
    Q_INVOKABLE static QPixmap takeItemShot(QQuickItem   * item,
                                            const QColor & background = Qt::transparent);

    Q_INVOKABLE static bool saveItemShot(const QString & fileName,
                                         QQuickItem    * item,
                                         const QString & format = "png", int quality = -1,
                                         const QColor  & background = Qt::transparent);
#endif

    Q_INVOKABLE static bool compressShot (const QString & fileName, int quality = 0);
    Q_INVOKABLE static bool compressShots(const QString & path,     int quality = 0);

#ifdef QT_6
    Q_INVOKABLE static QString graphicsApiName();
#endif

protected: // Functions
    void hoverEnter();
    void hoverLeave();

#ifdef QT_4
    /* virtual */ void drawBackground(QPainter * painter, const QRectF & rect); // {}
    /* virtual */ void drawForeground(QPainter * painter, const QRectF & rect); // {}
#endif

protected: // Events
#ifdef QT_NEW
    /* virtual */ bool event(QEvent * event);
#endif

    /* virtual */ void showEvent(QShowEvent * event);

    /* virtual */ void moveEvent  (QMoveEvent   * event);
    /* virtual */ void resizeEvent(QResizeEvent * event);

//#ifdef QT_4
    ///* virtual */ void enterEvent(QEvent * event);
    ///* virtual */ void leaveEvent(QEvent * event);
//#endif

    /* virtual */ void mousePressEvent  (QMouseEvent * event);
    /* virtual */ void mouseReleaseEvent(QMouseEvent * event);

    /* virtual */ void mouseDoubleClickEvent(QMouseEvent * event);

    /* virtual */ void mouseMoveEvent(QMouseEvent * event);

    /* virtual */ void dragEnterEvent(QDragEnterEvent * event);
    /* virtual */ void dragLeaveEvent(QDragLeaveEvent * event);

    /* virtual */ void dragMoveEvent(QDragMoveEvent * event);

    /* virtual */ void dropEvent(QDropEvent * event);

    /* virtual */ void keyPressEvent  (QKeyEvent * event);
    /* virtual */ void keyReleaseEvent(QKeyEvent * event);

    /* virtual */ void focusInEvent (QFocusEvent * event);
    /* virtual */ void focusOutEvent(QFocusEvent * event);

    /* virtual */ void timerEvent(QTimerEvent * event);

    /* virtual */ void closeEvent(QCloseEvent * event);

//#ifdef SK_WIN_NATIVE
// NOTE: We need these functions for binary compatibility.
protected: // WAbstractView reimplementation
    /* virtual */ void onStateChanged(Qt::WindowState state);

    /* virtual */ void onClose();
//#endif

signals:
    void messageReceived(const QString & message);

    void stateChanged(Qt::WindowState state);

    void fadeIn ();
    void fadeOut();

    void dragEnded();

    void beforeClose();

    void itemChanged();

    void itemWidthChanged ();
    void itemHeightChanged();

    void flagsChanged();

    void xChanged();
    void yChanged();

    void widthChanged ();
    void heightChanged();

    void centerXChanged();
    void centerYChanged();

    void originXChanged();
    void originYChanged();

    void ratioChanged     ();
    void ratioPixelChanged();

    void zoomChanged();

    void minimumWidthChanged ();
    void minimumHeightChanged();

    void maximumWidthChanged ();
    void maximumHeightChanged();

    void geometryNormalChanged();

    void minimizedChanged ();
    void maximizedChanged ();
    void fullScreenChanged();
    void lockedChanged    ();
    void scalingChanged   ();

    void activeChanged  ();
    void enteredChanged ();
    void pressedChanged ();
    void draggingChanged();
    void draggedChanged ();
    void resizingChanged();
    void touchingChanged();

    void mousePosChanged   ();
    void mouseCursorChanged();

#ifdef QT_4
    void openglChanged();
#endif

    void antialiasChanged();
    void vsyncChanged    ();

    void hoverEnabledChanged();

    void fadeEnabledChanged();

    void idleCheckChanged();
    void idleChanged     ();
    void idleDelayChanged();

#ifdef QT_4
    void mousePressed      (QDeclarativeMouseEvent * event);
    void mouseReleased     (QDeclarativeMouseEvent * event);
    void mouseDoubleClicked(QDeclarativeMouseEvent * event);
#else
    void mousePressed      (QQuickMouseEvent * event);
    void mouseReleased     (QQuickMouseEvent * event);
    void mouseDoubleClicked(QQuickMouseEvent * event);
#endif

    void keyPressed (WDeclarativeKeyEvent * event);
    void keyReleased(WDeclarativeKeyEvent * event);

    void viewportKeyPressed (WDeclarativeKeyEvent * event);
    void viewportKeyReleased(WDeclarativeKeyEvent * event);

    void keyShiftPressedChanged  ();
    void keyControlPressedChanged();
    void keyAltPressedChanged    ();

    void availableGeometryChanged();

    void backwardClicked();
    void forwardClicked ();

public: // Properties
#ifdef QT_4
    QDeclarativeItem * item() const;
    void               setItem(QDeclarativeItem * item);
#else
    QQuickItem * item() const;
    void         setItem(QQuickItem * item);
#endif

    qreal itemWidth () const;
    qreal itemHeight() const;

    Qt::WindowFlags flags() const;
    void            setFlags(Qt::WindowFlags flags);

    void setX(int x);
    void setY(int y);

    void setWidth (int width);
    void setHeight(int height);

    int centerX() const;
    int centerY() const;

    qreal originX() const;
    void  setOriginX(qreal x);

    qreal originY() const;
    void  setOriginY(qreal y);

    qreal ratio     () const;
    qreal ratioPixel() const;

    qreal zoom() const;
    void  setZoom(qreal zoom);

    int  minimumWidth() const;
    void setMinimumWidth(int width);

    int  minimumHeight() const;
    void setMinimumHeight(int height);

    int  maximumWidth() const;
    void setMaximumWidth(int width);

    int  maximumHeight() const;
    void setMaximumHeight(int height);

    QRect geometryNormal() const;

    bool isMinimized() const;
    void setMinimized(bool minimized);

    bool isMaximized() const;
    void setMaximized(bool maximized);

    bool isFullScreen() const;
    void setFullScreen(bool fullScreen);

    bool isLocked() const;
    void setLocked(bool locked);

    bool isScaling() const;
    void setScaling(bool scaling);

    bool isActive  () const;
    bool isEntered () const;
    bool isPressed () const;
    bool isDragging() const;
    bool isDragged () const;
    bool isResizing() const;
    bool isTouching() const;

    QPoint mousePos() const;

    int mouseX() const;
    int mouseY() const;

    Qt::CursorShape mouseCursor() const;

    bool opengl() const;
#ifdef QT_4
    void setOpengl(bool enabled);
#endif

    bool antialias() const;
    void setAntialias(bool enabled);

    bool vsync() const;
    void setVsync(bool enabled);

    bool hoverEnabled() const;
    void setHoverEnabled(bool enabled);

    bool fadeEnabled() const;
    void setFadeEnabled(bool enabled);

    //---------------------------------------------------------------------------------------------
    // Mouse idle

    bool idleCheck() const;
    void setIdleCheck(bool check);

    bool idle() const;
    void setIdle(bool idle);

    int  idleDelay() const;
    void setIdleDelay(int msec);

    //---------------------------------------------------------------------------------------------
    // Keys

    bool keyShiftPressed  () const;
    bool keyControlPressed() const;
    bool keyAltPressed    () const;

    //---------------------------------------------------------------------------------------------
    // Screen

    QRect availableGeometry() const;
    QRect screenGeometry   () const;

private:
    W_DECLARE_PRIVATE(WView)

#ifdef QT_NEW
    Q_PRIVATE_SLOT(d_func(), void onStateChanged(Qt::ApplicationState))
#endif

    Q_PRIVATE_SLOT(d_func(), void onGeometryChanged())

#ifdef QT_NEW
    Q_PRIVATE_SLOT(d_func(), void onScreenChanged())
#endif

    Q_PRIVATE_SLOT(d_func(), void onFadeTimeout())
    Q_PRIVATE_SLOT(d_func(), void onIdleTimeout())

#ifdef SK_MOBILE
    Q_PRIVATE_SLOT(d_func(), void onMessageChanged())
#endif

    Q_PRIVATE_SLOT(d_func(), void onBeforeClose())

    Q_PRIVATE_SLOT(d_func(), void onCursorVisibleChanged())

    friend class WControllerView;
    friend class WControllerViewPrivate;
    friend class WViewDrag;
    friend class WViewDragPrivate;
    friend class WDeclarativeItemPrivate;
    friend class WDeclarativeMouseArea;
    friend class WDeclarativeMouseAreaPrivate;
    friend class WResizer;
    friend class WResizerPrivate;
    friend class WWindow;
    friend class WWindowPrivate;
#ifdef QT_4
    friend class WViewport;
#endif
};

#ifdef QT_4

//-------------------------------------------------------------------------------------------------
// WViewScene
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WViewScene : public QGraphicsScene
{
    Q_OBJECT

public:
    WViewScene(WView * parent);

private: // Variables
    WView * view;

private:
    Q_DISABLE_COPY(WViewScene)

    friend class WDeclarativeItem;
    friend class WDeclarativeItemPrivate;
    friend class WDeclarativeMouseArea;
    friend class WDeclarativeMouseAreaPrivate;
};

#endif

#include <private/WView_p>

#endif // SK_NO_VIEW
#endif // WVIEW_H
