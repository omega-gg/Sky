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

// Qt includes
#ifdef QT_LATEST
#include <QPixmap>
#endif

// Sk includes
#include <WDeclarativeMouseArea>

#ifndef SK_NO_WINDOW

// Forward declarations
class WWindowPrivate;
class WView;
class WDeclarativeKeyEvent;

//-------------------------------------------------------------------------------------------------
// WWindow
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WWindow : public WDeclarativeMouseArea
{
    Q_OBJECT

    Q_PROPERTY(WView * view READ view CONSTANT)

    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)

    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

    //---------------------------------------------------------------------------------------------
    // WView
    //---------------------------------------------------------------------------------------------

    Q_PROPERTY(qreal itemWidth  READ itemWidth  NOTIFY itemWidthChanged)
    Q_PROPERTY(qreal itemHeight READ itemHeight NOTIFY itemHeightChanged)

    Q_PROPERTY(int x READ x WRITE setX NOTIFY viewXChanged)
    Q_PROPERTY(int y READ y WRITE setY NOTIFY viewYChanged)

    Q_PROPERTY(int width  READ width  WRITE setWidth  NOTIFY viewWidthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY viewHeightChanged)

    Q_PROPERTY(int centerX READ centerX NOTIFY centerXChanged)
    Q_PROPERTY(int centerY READ centerY NOTIFY centerYChanged)

    Q_PROPERTY(qreal originX READ originX WRITE setOriginX NOTIFY originXChanged)
    Q_PROPERTY(qreal originY READ originY WRITE setOriginY NOTIFY originYChanged)

    Q_PROPERTY(qreal ratio READ ratio NOTIFY ratioChanged)

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

    Q_PROPERTY(int fadeDuration READ fadeDuration WRITE setFadeDuration NOTIFY fadeDurationChanged)

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
    explicit WWindow(QDeclarativeItem * parent = NULL);
#else
    explicit WWindow(QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE void activate();
    Q_INVOKABLE void raise   ();

    Q_INVOKABLE void centerWindow();

    Q_INVOKABLE bool close();

    //---------------------------------------------------------------------------------------------
    // Focus

    Q_INVOKABLE bool getFocus() const;

    Q_INVOKABLE void clearFocus();

#ifdef QT_4
    Q_INVOKABLE void clearFocusItem(QDeclarativeItem * item);
#else
    Q_INVOKABLE void clearFocusItem(QQuickItem * item);
#endif

    //---------------------------------------------------------------------------------------------
    // Geometry

    Q_INVOKABLE int getScreenNumber() const;

    Q_INVOKABLE void setMinimumSize(int width, int height);
    Q_INVOKABLE void setMaximumSize(int width, int height);

    Q_INVOKABLE QRect getDefaultGeometry() const;
    Q_INVOKABLE void  setDefaultGeometry();

    Q_INVOKABLE void saveGeometry();

    Q_INVOKABLE void checkPosition();

    Q_INVOKABLE void originTo(qreal x, qreal y);

    //---------------------------------------------------------------------------------------------
    // Drag

    Q_INVOKABLE bool testDrag(const QPointF & posA, const QPointF & posB, qreal distance = -1);

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

#ifdef QT_4
    Q_INVOKABLE QPixmap takeShot(int x = 0, int y = 0, int width = -1, int height = -1) const;

    Q_INVOKABLE bool saveShot(const QString & fileName,
                              int x = 0, int y = 0, int width = -1, int height = -1,
                              const QString & format = "png", int quality = -1) const;
#else
    Q_INVOKABLE QPixmap takeShot(int x = 0, int y = 0, int width = -1, int height = -1);

    Q_INVOKABLE bool saveShot(const QString & fileName,
                              int x = 0, int y = 0, int width = -1, int height = -1,
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

#ifdef Q_OS_WIN
    Q_INVOKABLE void setWindowSnap    (bool enabled);
    Q_INVOKABLE void setWindowMaximize(bool enabled);
    Q_INVOKABLE void setWindowClip    (bool enabled);
#endif

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

protected: // Events
#ifdef QT_4
    /* virtual */ void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    /* virtual */ void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
#else
    /* virtual */ void hoverEnterEvent(QHoverEvent * event);
    /* virtual */ void hoverLeaveEvent(QHoverEvent * event);

    /* virtual */ void dragEnterEvent(QDragEnterEvent * event);
    /* virtual */ void dragLeaveEvent(QDragLeaveEvent * event);

    /* virtual */ void dragMoveEvent(QDragMoveEvent * event);

    /* virtual */ void dropEvent(QDropEvent * event);
#endif

signals:
    void iconChanged();

    void visibleChanged();

    void opacityChanged();

    //---------------------------------------------------------------------------------------------
    // WView

    void messageReceived(const QString & message);

    void stateChanged(Qt::WindowState state);

    void fadeIn ();
    void fadeOut();

    void dragEnded();

    void beforeClose();

    //---------------------------------------------------------------------------------------------

    void itemWidthChanged ();
    void itemHeightChanged();

    void viewXChanged();
    void viewYChanged();

    void viewWidthChanged ();
    void viewHeightChanged();

    void centerXChanged();
    void centerYChanged();

    void originXChanged();
    void originYChanged();

    void ratioChanged();

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

    void fadeEnabledChanged ();
    void fadeDurationChanged();

    //---------------------------------------------------------------------------------------------

    void idleCheckChanged();
    void idleChanged     ();
    void idleDelayChanged();

    //---------------------------------------------------------------------------------------------

    void mousePressed      (WDeclarativeMouseEvent * event);
    void mouseReleased     (WDeclarativeMouseEvent * event);
    void mouseDoubleClicked(WDeclarativeMouseEvent * event);

    void keyPressed (WDeclarativeKeyEvent * event);
    void keyReleased(WDeclarativeKeyEvent * event);

    //---------------------------------------------------------------------------------------------

    void keyShiftPressedChanged  ();
    void keyControlPressedChanged();
    void keyAltPressedChanged    ();

    //---------------------------------------------------------------------------------------------

    void availableGeometryChanged();

public: // Properties
    WView * view() const;

    QString icon() const;
    void    setIcon(const QString & icon);

    bool isVisible() const;
    void setVisible(bool visible);

    qreal opacity() const;
    void  setOpacity(qreal opacity);

    //---------------------------------------------------------------------------------------------
    // WView
    //---------------------------------------------------------------------------------------------

    qreal itemWidth () const;
    qreal itemHeight() const;

    int  x() const;
    void setX(int x);

    int  y() const;
    void setY(int y);

    int  width() const;
    void setWidth(int width);

    int  height() const;
    void setHeight(int height);

    int centerX() const;
    int centerY() const;

    qreal originX() const;
    void  setOriginX(qreal x);

    qreal originY() const;
    void  setOriginY(qreal y);

    qreal ratio() const;

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

    int  fadeDuration() const;
    void setFadeDuration(int msec);

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
    W_DECLARE_PRIVATE(WWindow)

    friend class WWindowTopPrivate;
    friend class WWindowPressWatcher;
    friend class WWindowHoverItem;
};

#endif // SK_NO_WINDOW
#endif // WWINDOW_H
