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

#include "WWindow.h"

#ifndef SK_NO_WINDOW

// Qt includes
#include <QApplication>
#include <QKeyEvent>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WView>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WWindow_p.h"

WWindowPrivate::WWindowPrivate(WWindow * p) : WDeclarativeMouseAreaPrivate(p) {}

/* virtual */ WWindowPrivate::~WWindowPrivate()
{
    view->deleteLater();
}

//-------------------------------------------------------------------------------------------------

void WWindowPrivate::init()
{
    Q_Q(WWindow);

    icon = sk->icon();

    visible = true;

    hoverItem = NULL;

    //---------------------------------------------------------------------------------------------
    // View

#if defined(SK_WIN_NATIVE)
    view = new WView(q, NULL);
#elif defined(Q_OS_WIN)
    view = new WView(q, NULL, Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
#else
    view = new WView(q, NULL, Qt::FramelessWindowHint);
#endif

    view->setWindowTitle(sk->name());

    if (icon.isEmpty() == false)
    {
        view->setWindowIcon(QIcon(icon));
    }

    view->setVisible(true);

    //---------------------------------------------------------------------------------------------
    // Signals

    QObject::connect(view, SIGNAL(messageReceived(const QString &)),
                     q,    SIGNAL(messageReceived(const QString &)));

    QObject::connect(view, SIGNAL(stateChanged(Qt::WindowState)),
                     q,    SIGNAL(stateChanged(Qt::WindowState)));

    QObject::connect(view, SIGNAL(fadeIn ()), q, SIGNAL(fadeIn ()));
    QObject::connect(view, SIGNAL(fadeOut()), q, SIGNAL(fadeOut()));

    QObject::connect(view, SIGNAL(dragEnded()), q, SIGNAL(dragEnded()));

    QObject::connect(view, SIGNAL(beforeClose()), q, SIGNAL(beforeClose()));

    QObject::connect(view, SIGNAL(itemWidthChanged ()), q, SIGNAL(itemWidthChanged ()));
    QObject::connect(view, SIGNAL(itemHeightChanged()), q, SIGNAL(itemHeightChanged()));

    QObject::connect(view, SIGNAL(xChanged()), q, SIGNAL(viewXChanged()));
    QObject::connect(view, SIGNAL(yChanged()), q, SIGNAL(viewYChanged()));

    QObject::connect(view, SIGNAL(widthChanged ()), q, SIGNAL(viewWidthChanged ()));
    QObject::connect(view, SIGNAL(heightChanged()), q, SIGNAL(viewHeightChanged()));

    QObject::connect(view, SIGNAL(centerXChanged()), q, SIGNAL(centerXChanged()));
    QObject::connect(view, SIGNAL(centerYChanged()), q, SIGNAL(centerYChanged()));

    QObject::connect(view, SIGNAL(originXChanged()), q, SIGNAL(originXChanged()));
    QObject::connect(view, SIGNAL(originYChanged()), q, SIGNAL(originYChanged()));

    QObject::connect(view, SIGNAL(zoomChanged()), q, SIGNAL(zoomChanged()));

    QObject::connect(view, SIGNAL(minimumWidthChanged ()), q, SIGNAL(minimumWidthChanged ()));
    QObject::connect(view, SIGNAL(minimumHeightChanged()), q, SIGNAL(minimumHeightChanged()));

    QObject::connect(view, SIGNAL(maximumWidthChanged ()), q, SIGNAL(maximumWidthChanged ()));
    QObject::connect(view, SIGNAL(maximumHeightChanged()), q, SIGNAL(maximumHeightChanged()));

    QObject::connect(view, SIGNAL(geometryNormalChanged()), q, SIGNAL(geometryNormalChanged()));

    QObject::connect(view, SIGNAL(minimizedChanged ()), q, SIGNAL(minimizedChanged ()));
    QObject::connect(view, SIGNAL(maximizedChanged ()), q, SIGNAL(maximizedChanged ()));
    QObject::connect(view, SIGNAL(fullScreenChanged()), q, SIGNAL(fullScreenChanged()));
    QObject::connect(view, SIGNAL(lockedChanged    ()), q, SIGNAL(lockedChanged    ()));

    QObject::connect(view, SIGNAL(activeChanged  ()), q, SIGNAL(activeChanged  ()));
    QObject::connect(view, SIGNAL(enteredChanged ()), q, SIGNAL(enteredChanged ()));
    QObject::connect(view, SIGNAL(draggingChanged()), q, SIGNAL(draggingChanged()));
    QObject::connect(view, SIGNAL(draggedChanged ()), q, SIGNAL(draggedChanged ()));
    QObject::connect(view, SIGNAL(resizingChanged()), q, SIGNAL(resizingChanged()));

    QObject::connect(view, SIGNAL(mousePosChanged   ()), q, SIGNAL(mousePosChanged   ()));
    QObject::connect(view, SIGNAL(mouseCursorChanged()), q, SIGNAL(mouseCursorChanged()));

    QObject::connect(view, SIGNAL(openglChanged   ()), q, SIGNAL(openglChanged   ()));
    QObject::connect(view, SIGNAL(antialiasChanged()), q, SIGNAL(antialiasChanged()));
    QObject::connect(view, SIGNAL(vsyncChanged    ()), q, SIGNAL(vsyncChanged    ()));

    QObject::connect(view, SIGNAL(hoverEnabledChanged()), q, SIGNAL(hoverEnabledChanged()));

    QObject::connect(view, SIGNAL(fadeEnabledChanged ()), q, SIGNAL(fadeEnabledChanged ()));
    QObject::connect(view, SIGNAL(fadeDurationChanged()), q, SIGNAL(fadeDurationChanged()));

    QObject::connect(view, SIGNAL(idleCheckChanged()), q, SIGNAL(idleCheckChanged()));
    QObject::connect(view, SIGNAL(idleChanged     ()), q, SIGNAL(idleChanged     ()));
    QObject::connect(view, SIGNAL(idleDelayChanged()), q, SIGNAL(idleDelayChanged()));

    QObject::connect(view, SIGNAL(mousePressed(WDeclarativeMouseEvent *)),
                     q,    SIGNAL(mousePressed(WDeclarativeMouseEvent *)));

    QObject::connect(view, SIGNAL(mouseReleased(WDeclarativeMouseEvent *)),
                     q,    SIGNAL(mouseReleased(WDeclarativeMouseEvent *)));

    QObject::connect(view, SIGNAL(mouseDoubleClicked(WDeclarativeMouseEvent *)),
                     q,    SIGNAL(mouseDoubleClicked(WDeclarativeMouseEvent *)));

    QObject::connect(view, SIGNAL(keyPressed(WDeclarativeKeyEvent *)),
                     q,    SIGNAL(keyPressed(WDeclarativeKeyEvent *)));

    QObject::connect(view, SIGNAL(keyReleased(WDeclarativeKeyEvent *)),
                     q,    SIGNAL(keyReleased(WDeclarativeKeyEvent *)));

    QObject::connect(view, SIGNAL(keyShiftPressedChanged()),
                     q,    SIGNAL(keyShiftPressedChanged()));

    QObject::connect(view, SIGNAL(keyControlPressedChanged()),
                     q,    SIGNAL(keyControlPressedChanged()));

    QObject::connect(view, SIGNAL(keyAltPressedChanged()),
                     q,    SIGNAL(keyAltPressedChanged()));

    QObject::connect(view, SIGNAL(availableGeometryChanged()),
                     q,    SIGNAL(availableGeometryChanged()));
}

//=================================================================================================
// WWindow
//=================================================================================================

/* explicit */ WWindow::WWindow(QDeclarativeItem * parent)
    : WDeclarativeMouseArea(new WWindowPrivate(this), parent)
{
    Q_D(WWindow); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::activate()
{
    Q_D(WWindow); d->view->activate();
}

/* Q_INVOKABLE */ void WWindow::raise()
{
    Q_D(WWindow); d->view->raise();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::close()
{
    Q_D(WWindow); d->view->close();
}

//-------------------------------------------------------------------------------------------------
// Focus

/* Q_INVOKABLE */ bool WWindow::getFocus() const
{
    Q_D(const WWindow);

    return (d->view->d_func()->scene->focusItem() != NULL);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::clearFocus()
{
    setFocus(true);
}

/* Q_INVOKABLE */ void WWindow::clearFocusItem(QDeclarativeItem * item)
{
    Q_ASSERT(item);

    if (item->focusItem())
    {
        setFocus(true);
    }
}

//-------------------------------------------------------------------------------------------------
// Geometry

/* Q_INVOKABLE */ int WWindow::getScreenNumber() const
{
    Q_D(const WWindow); return d->view->getScreenNumber();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::setMinimumSize(int width, int height)
{
    Q_D(WWindow); d->view->setMinimumSize(width, height);
}

/* Q_INVOKABLE */ void WWindow::setMaximumSize(int width, int height)
{
    Q_D(WWindow); d->view->setMaximumSize(width, height);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QRect WWindow::getDefaultGeometry() const
{
    Q_D(const WWindow); return d->view->getDefaultGeometry();
}

/* Q_INVOKABLE */ void WWindow::setDefaultGeometry()
{
    Q_D(WWindow); d->view->setDefaultGeometry();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::saveGeometry()
{
    Q_D(WWindow); d->view->saveGeometry();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::checkPosition()
{
    Q_D(WWindow); return d->view->checkPosition();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::originTo(qreal x, qreal y)
{
    Q_D(WWindow); return d->view->originTo(x, y);
}

//-------------------------------------------------------------------------------------------------
// Drag

/* Q_INVOKABLE */ bool WWindow::testDrag(const QPointF & posA,
                                         const QPointF & posB, qreal distance)
{
    Q_D(WWindow); return d->view->testDrag(posA, posB, distance);
}

/* Q_INVOKABLE */ void WWindow::startDrag(const QString & text, int actions)
{
    Q_D(WWindow); d->view->startDrag(text, actions);
}

//-------------------------------------------------------------------------------------------------
// Hover

/* Q_INVOKABLE */ int WWindow::hoverCount() const
{
    Q_D(const WWindow); return d->view->hoverCount();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::updateHover()
{
    Q_D(WWindow); d->view->updateHover();
}

/* Q_INVOKABLE */ void WWindow::clearHover()
{
    Q_D(WWindow); d->view->clearHover();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::checkLeave(int msec)
{
    Q_D(WWindow); d->view->checkLeave(msec);
}

//-------------------------------------------------------------------------------------------------
// Shot

/* Q_INVOKABLE */ QPixmap WWindow::takeShot(int x, int y, int width, int height) const
{
    Q_D(const WWindow); return d->view->takeShot(x, y, width, height);
}

/* Q_INVOKABLE */ bool WWindow::saveShot(const QString & fileName, int x,     int y,
                                                                   int width, int height) const
{
    Q_D(const WWindow); return d->view->saveShot(fileName, x, y, width, height);
}

//-------------------------------------------------------------------------------------------------
// Cursor

/* Q_INVOKABLE */ void WWindow::registerCursor(WDeclarativeMouseArea::CursorShape shape,
                                               const QCursor                    & cursor)
{
    Q_D(WWindow); d->view->registerCursor(shape, cursor);
}

/* Q_INVOKABLE */ void WWindow::unregisterCursor(WDeclarativeMouseArea::CursorShape shape)
{
    Q_D(WWindow); d->view->unregisterCursor(shape);
}

/* Q_INVOKABLE */ void WWindow::unregisterCursors()
{
    Q_D(WWindow); d->view->unregisterCursors();
}

//-------------------------------------------------------------------------------------------------
// Input

/* Q_INVOKABLE */ void WWindow::mouseMove(int x, int y, Qt::MouseButton button) const
{
    Q_D(const WWindow); d->view->mouseMove(x, y, button);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::mousePress(Qt::MouseButton button) const
{
    Q_D(const WWindow); d->view->mousePress(button);
}

/* Q_INVOKABLE */ void WWindow::mouseRelease(Qt::MouseButton button) const
{
    Q_D(const WWindow); d->view->mouseRelease(button);
}

/* Q_INVOKABLE */ void WWindow::mouseClick(Qt::MouseButton button, int msec) const
{
    Q_D(const WWindow); d->view->mouseClick(button, msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::wheel(Qt::Orientation orientation, int delta) const
{
    Q_D(const WWindow); d->view->wheel(orientation, delta);
}

/* Q_INVOKABLE */ void WWindow::wheelUp(int delta) const
{
    Q_D(const WWindow); d->view->wheelUp(delta);
}

/* Q_INVOKABLE */ void WWindow::wheelDown(int delta) const
{
    Q_D(const WWindow); d->view->wheelDown(delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::keyPress(int key, Qt::KeyboardModifiers modifiers) const
{
    Q_D(const WWindow); d->view->keyPress(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyRelease(int key, Qt::KeyboardModifiers modifiers) const
{
    Q_D(const WWindow); d->view->keyRelease(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyClick(int key, Qt::KeyboardModifiers modifiers, int msec) const
{
    Q_D(const WWindow); d->view->keyClick(key, modifiers, msec);
}

//-------------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE */ void WWindow::mouseMove(int x, int y, int button) const
{
    Q_D(const WWindow); d->view->mouseMove(x, y, button);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::mousePress(int button) const
{
    Q_D(const WWindow); d->view->mousePress(button);
}

/* Q_INVOKABLE */ void WWindow::mouseRelease(int button) const
{
    Q_D(const WWindow); d->view->mouseRelease(button);
}

/* Q_INVOKABLE */ void WWindow::mouseClick(int button, int msec) const
{
    Q_D(const WWindow); d->view->mouseClick(button, msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::wheel(int orientation, int delta) const
{
    Q_D(const WWindow); d->view->wheel(orientation, delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::keyPress(int key, int modifiers) const
{
    Q_D(const WWindow); d->view->keyPress(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyRelease(int key, int modifiers) const
{
    Q_D(const WWindow); d->view->keyRelease(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyClick(int key, int modifiers, int msec) const
{
    Q_D(const WWindow); d->view->keyClick(key, modifiers, msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WWindow::itemUnderMouse(QDeclarativeItem * item)
{
    if (isEntered())
    {
         return item->isUnderMouse();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::registerCursorUrl(int shape, const QUrl & url, const QSize & size)
{
    Q_D(WWindow); d->view->registerCursorUrl(shape, url, size);
}

/* Q_INVOKABLE */ void WWindow::unregisterCursor(int shape)
{
    Q_D(WWindow); d->view->unregisterCursor(shape);
}

//-------------------------------------------------------------------------------------------------

#ifdef Q_OS_WIN

/* Q_INVOKABLE */ void WWindow::setWindowSnap(bool enabled)
{
    Q_D(WWindow); d->view->setWindowSnap(enabled);
}

/* Q_INVOKABLE */ void WWindow::setWindowMaximize(bool enabled)
{
    Q_D(WWindow); d->view->setWindowMaximize(enabled);
}

#endif // Q_OS_WIN

//-------------------------------------------------------------------------------------------------
// Static interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QPixmap WWindow::takeItemShot(QGraphicsObject * item,
                                                       const QColor    & background,
                                                       bool              forceVisible)
{
    return WView::takeItemShot(item, background, forceVisible);
}

/* Q_INVOKABLE static */ bool WWindow::saveItemShot(const QString   & fileName,
                                                    QGraphicsObject * item,
                                                    const QColor    & background,
                                                    bool              forceVisible)
{
    return WView::saveItemShot(fileName, item, background, forceVisible);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WWindow::compressShots(const QString & path, int quality)
{
    return WView::compressShots(path, quality);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WView * WWindow::view() const
{
    Q_D(const WWindow); return d->view;
}

//-------------------------------------------------------------------------------------------------

QString WWindow::icon() const
{
    Q_D(const WWindow); return d->icon;
}

void WWindow::setIcon(const QString & icon)
{
    Q_D(WWindow);

    if (d->icon == icon) return;

    d->icon = icon;

    d->view->setWindowIcon(QIcon(icon));

    emit iconChanged();
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isVisible() const
{
    Q_D(const WWindow); return d->visible;
}

void WWindow::setVisible(bool visible)
{
    Q_D(WWindow);

    if (d->visible == visible) return;

    d->visible = visible;

    d->view->setVisible(visible);

    emit visibleChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::opacity() const
{
    Q_D(const WWindow); return d->view->windowOpacity();
}

void WWindow::setOpacity(qreal opacity)
{
    Q_D(WWindow);

    if (d->view->windowOpacity() == opacity) return;

    d->view->setWindowOpacity(opacity);

    emit opacityChanged();
}

//-------------------------------------------------------------------------------------------------
// WView
//-------------------------------------------------------------------------------------------------

qreal WWindow::itemWidth() const
{
    Q_D(const WWindow); return d->view->itemWidth();
}

qreal WWindow::itemHeight() const
{
    Q_D(const WWindow); return d->view->itemHeight();
}

//-------------------------------------------------------------------------------------------------

int WWindow::x() const
{
    Q_D(const WWindow); return d->view->x();
}

void WWindow::setX(int x)
{
    Q_D(WWindow); d->view->setX(x);
}

int WWindow::y() const
{
    Q_D(const WWindow); return d->view->y();
}

void WWindow::setY(int y)
{
    Q_D(WWindow); d->view->setY(y);
}

//-------------------------------------------------------------------------------------------------

int WWindow::width() const
{
    Q_D(const WWindow); return d->view->width();
}

void WWindow::setWidth(int width)
{
    Q_D(WWindow); d->view->setWidth(width);
}

int WWindow::height() const
{
    Q_D(const WWindow); return d->view->height();
}

void WWindow::setHeight(int height)
{
    Q_D(WWindow); d->view->setHeight(height);
}

//-------------------------------------------------------------------------------------------------

int WWindow::centerX() const
{
    Q_D(const WWindow); return d->view->centerX();
}

int WWindow::centerY() const
{
    Q_D(const WWindow); return d->view->centerY();
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::originX() const
{
    Q_D(const WWindow); return d->view->originX();
}

void WWindow::setOriginX(qreal x)
{
    Q_D(WWindow); d->view->setOriginX(x);
}

qreal WWindow::originY() const
{
    Q_D(const WWindow); return d->view->originY();
}

void WWindow::setOriginY(qreal y)
{
    Q_D(WWindow); d->view->setOriginY(y);
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::zoom() const
{
    Q_D(const WWindow); return d->view->zoom();
}

void WWindow::setZoom(qreal zoom)
{
    Q_D(WWindow); d->view->setZoom(zoom);
}

//-------------------------------------------------------------------------------------------------

int WWindow::minimumWidth() const
{
    Q_D(const WWindow); return d->view->minimumWidth();
}

void WWindow::setMinimumWidth(int width)
{
    Q_D(WWindow); d->view->setMinimumWidth(width);
}

int WWindow::minimumHeight() const
{
    Q_D(const WWindow); return d->view->minimumHeight();
}

void WWindow::setMinimumHeight(int height)
{
    Q_D(WWindow); d->view->setMinimumHeight(height);
}

//-------------------------------------------------------------------------------------------------

int WWindow::maximumWidth() const
{
    Q_D(const WWindow); return d->view->maximumWidth();
}

void WWindow::setMaximumWidth(int width)
{
    Q_D(WWindow); d->view->setMaximumWidth(width);
}

int WWindow::maximumHeight() const
{
    Q_D(const WWindow); return d->view->maximumHeight();
}

void WWindow::setMaximumHeight(int height)
{
    Q_D(WWindow); d->view->setMaximumHeight(height);
}

//-------------------------------------------------------------------------------------------------

QRect WWindow::geometryNormal() const
{
    Q_D(const WWindow); return d->view->geometryNormal();
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isMinimized() const
{
    Q_D(const WWindow); return d->view->isMinimized();
}

void WWindow::setMinimized(bool minimized)
{
    Q_D(WWindow); d->view->setMinimized(minimized);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isMaximized() const
{
    Q_D(const WWindow); return d->view->isMaximized();
}

void WWindow::setMaximized(bool maximized)
{
    Q_D(WWindow); d->view->setMaximized(maximized);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isFullScreen() const
{
    Q_D(const WWindow); return d->view->isFullScreen();
}

void WWindow::setFullScreen(bool fullScreen)
{
    Q_D(WWindow); d->view->setFullScreen(fullScreen);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isLocked() const
{
    Q_D(const WWindow); return d->view->isLocked();
}

void WWindow::setLocked(bool locked)
{
    Q_D(WWindow);

    d->view->setLocked(locked);

    bool visible = d->view->isVisible();

    if (d->visible != visible)
    {
        d->visible = visible;

        emit visibleChanged();
    }
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isActive() const
{
    Q_D(const WWindow); return d->view->isActiveWindow();
}

bool WWindow::isEntered() const
{
    Q_D(const WWindow); return d->view->isEntered();
}

bool WWindow::isDragging() const
{
    Q_D(const WWindow); return d->view->isDragging();
}

bool WWindow::isDragged() const
{
    Q_D(const WWindow); return d->view->isDragged();
}

bool WWindow::isResizing() const
{
    Q_D(const WWindow); return d->view->isResizing();
}

//-------------------------------------------------------------------------------------------------

QPoint WWindow::mousePos() const
{
    Q_D(const WWindow); return d->view->mousePos();
}

//-------------------------------------------------------------------------------------------------

int WWindow::mouseX() const
{
    Q_D(const WWindow); return d->view->mouseX();
}

int WWindow::mouseY() const
{
    Q_D(const WWindow); return d->view->mouseY();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeMouseArea::CursorShape WWindow::mouseCursor() const
{
    Q_D(const WWindow); return d->view->mouseCursor();
}

//-------------------------------------------------------------------------------------------------

bool WWindow::opengl() const
{
    Q_D(const WWindow); return d->view->opengl();
}

void WWindow::setOpengl(bool enabled)
{
    Q_D(WWindow); d->view->setOpengl(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::antialias() const
{
    Q_D(const WWindow); return d->view->antialias();
}

void WWindow::setAntialias(bool enabled)
{
    Q_D(WWindow); d->view->setAntialias(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::vsync() const
{
    Q_D(const WWindow); return d->view->vsync();
}

void WWindow::setVsync(bool enabled)
{
    Q_D(WWindow); d->view->setVsync(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::hoverEnabled() const
{
    Q_D(const WWindow); return d->view->hoverEnabled();
}

void WWindow::setHoverEnabled(bool enabled)
{
    Q_D(WWindow); d->view->setHoverEnabled(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::fadeEnabled() const
{
    Q_D(const WWindow); return d->view->fadeEnabled();
}

void WWindow::setFadeEnabled(bool enabled)
{
    Q_D(WWindow); d->view->setFadeEnabled(enabled);
}

int WWindow::fadeDuration() const
{
    Q_D(const WWindow); return d->view->fadeDuration();
}

void WWindow::setFadeDuration(int msec)
{
    Q_D(WWindow); d->view->setFadeDuration(msec);
}

//-------------------------------------------------------------------------------------------------
// Mouse idle

bool WWindow::idleCheck() const
{
    Q_D(const WWindow); return d->view->idleCheck();
}

void WWindow::setIdleCheck(bool check)
{
    Q_D(WWindow); d->view->setIdleCheck(check);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::idle() const
{
    Q_D(const WWindow); return d->view->idle();
}

void WWindow::setIdle(bool idle)
{
    Q_D(WWindow); d->view->setIdle(idle);
}

//-------------------------------------------------------------------------------------------------

int WWindow::idleDelay() const
{
    Q_D(const WWindow); return d->view->idleDelay();
}

void WWindow::setIdleDelay(int msec)
{
    Q_D(WWindow); d->view->setIdleDelay(msec);
}

//-------------------------------------------------------------------------------------------------
// Keys

bool WWindow::keyShiftPressed() const
{
    Q_D(const WWindow); return d->view->keyShiftPressed();
}

bool WWindow::keyControlPressed() const
{
    Q_D(const WWindow); return d->view->keyControlPressed();
}

bool WWindow::keyAltPressed() const
{
    Q_D(const WWindow); return d->view->keyAltPressed();
}

//-------------------------------------------------------------------------------------------------
// Screen

QRect WWindow::availableGeometry() const
{
    Q_D(const WWindow); return d->view->availableGeometry();
}

QRect WWindow::screenGeometry() const
{
    Q_D(const WWindow); return d->view->screenGeometry();
}

#endif // SK_NO_WINDOW
