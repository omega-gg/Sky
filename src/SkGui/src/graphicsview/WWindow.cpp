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

#include "WWindow.h"

#ifndef SK_NO_WINDOW

// Qt includes
#include <QApplication>
#include <QKeyEvent>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WMainView>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WWindow_p.h"

WWindowPrivate::WWindowPrivate(WWindow * p) : WDeclarativeMouseAreaPrivate(p) {}

/* virtual */ WWindowPrivate::~WWindowPrivate()
{
    mainView->deleteLater();
}

//-------------------------------------------------------------------------------------------------

void WWindowPrivate::init()
{
    Q_Q(WWindow);

    icon = sk->icon();

    visible = true;

    hoverItem = NULL;

    //---------------------------------------------------------------------------------------------
    // MainView

    mainView = new WMainView(q, NULL, Qt::FramelessWindowHint);

    mainView->setWindowTitle(sk->name());

    if (icon.isEmpty() == false)
    {
        mainView->setWindowIcon(QIcon(icon));
    }

    mainView->setVisible(true);

    //---------------------------------------------------------------------------------------------
    // Signals

    QObject::connect(mainView, SIGNAL(messageReceived(const QString &)),
                     q,        SIGNAL(messageReceived(const QString &)));

    QObject::connect(mainView, SIGNAL(fadeIn ()), q, SIGNAL(fadeIn ()));
    QObject::connect(mainView, SIGNAL(fadeOut()), q, SIGNAL(fadeOut()));

    QObject::connect(mainView, SIGNAL(dragEnded()), q, SIGNAL(dragEnded()));

    QObject::connect(mainView, SIGNAL(beforeClose()), q, SIGNAL(beforeClose()));

    QObject::connect(mainView, SIGNAL(itemWidthChanged ()), q, SIGNAL(itemWidthChanged ()));
    QObject::connect(mainView, SIGNAL(itemHeightChanged()), q, SIGNAL(itemHeightChanged()));

    QObject::connect(mainView, SIGNAL(xChanged()), q, SIGNAL(viewXChanged()));
    QObject::connect(mainView, SIGNAL(yChanged()), q, SIGNAL(viewYChanged()));

    QObject::connect(mainView, SIGNAL(widthChanged ()), q, SIGNAL(viewWidthChanged ()));
    QObject::connect(mainView, SIGNAL(heightChanged()), q, SIGNAL(viewHeightChanged()));

    QObject::connect(mainView, SIGNAL(centerXChanged()), q, SIGNAL(centerXChanged()));
    QObject::connect(mainView, SIGNAL(centerYChanged()), q, SIGNAL(centerYChanged()));

    QObject::connect(mainView, SIGNAL(originXChanged()), q, SIGNAL(originXChanged()));
    QObject::connect(mainView, SIGNAL(originYChanged()), q, SIGNAL(originYChanged()));

    QObject::connect(mainView, SIGNAL(zoomChanged()), q, SIGNAL(zoomChanged()));

    QObject::connect(mainView, SIGNAL(minimumWidthChanged ()), q, SIGNAL(minimumWidthChanged ()));
    QObject::connect(mainView, SIGNAL(minimumHeightChanged()), q, SIGNAL(minimumHeightChanged()));

    QObject::connect(mainView, SIGNAL(maximumWidthChanged ()), q, SIGNAL(maximumWidthChanged ()));
    QObject::connect(mainView, SIGNAL(maximumHeightChanged()), q, SIGNAL(maximumHeightChanged()));

    QObject::connect(mainView, SIGNAL(geometryNormalChanged()),
                     q,        SIGNAL(geometryNormalChanged()));

    QObject::connect(mainView, SIGNAL(minimizedChanged ()), q, SIGNAL(minimizedChanged ()));
    QObject::connect(mainView, SIGNAL(maximizedChanged ()), q, SIGNAL(maximizedChanged ()));
    QObject::connect(mainView, SIGNAL(fullScreenChanged()), q, SIGNAL(fullScreenChanged()));
    QObject::connect(mainView, SIGNAL(lockedChanged    ()), q, SIGNAL(lockedChanged    ()));

    QObject::connect(mainView, SIGNAL(activeChanged  ()), q, SIGNAL(activeChanged  ()));
    QObject::connect(mainView, SIGNAL(enteredChanged ()), q, SIGNAL(enteredChanged ()));
    QObject::connect(mainView, SIGNAL(draggingChanged()), q, SIGNAL(draggingChanged()));
    QObject::connect(mainView, SIGNAL(draggedChanged ()), q, SIGNAL(draggedChanged ()));
    QObject::connect(mainView, SIGNAL(resizingChanged()), q, SIGNAL(resizingChanged()));

    QObject::connect(mainView, SIGNAL(mousePosChanged   ()), q, SIGNAL(mousePosChanged   ()));
    QObject::connect(mainView, SIGNAL(mouseCursorChanged()), q, SIGNAL(mouseCursorChanged()));

    QObject::connect(mainView, SIGNAL(autoSizeChanged()), q, SIGNAL(autoSizeChanged()));

    QObject::connect(mainView, SIGNAL(openglChanged   ()), q, SIGNAL(openglChanged   ()));
    QObject::connect(mainView, SIGNAL(antialiasChanged()), q, SIGNAL(antialiasChanged()));
    QObject::connect(mainView, SIGNAL(vsyncChanged    ()), q, SIGNAL(vsyncChanged    ()));

    QObject::connect(mainView, SIGNAL(hoverEnabledChanged()), q, SIGNAL(hoverEnabledChanged()));

    QObject::connect(mainView, SIGNAL(fadeEnabledChanged ()), q, SIGNAL(fadeEnabledChanged ()));
    QObject::connect(mainView, SIGNAL(fadeDurationChanged()), q, SIGNAL(fadeDurationChanged()));

    QObject::connect(mainView, SIGNAL(idleCheckChanged()), q, SIGNAL(idleCheckChanged()));
    QObject::connect(mainView, SIGNAL(idleChanged     ()), q, SIGNAL(idleChanged     ()));
    QObject::connect(mainView, SIGNAL(idleDelayChanged()), q, SIGNAL(idleDelayChanged()));

    QObject::connect(mainView, SIGNAL(mousePressed(WDeclarativeMouseEvent *)),
                     q,        SIGNAL(mousePressed(WDeclarativeMouseEvent *)));

    QObject::connect(mainView, SIGNAL(mouseReleased(WDeclarativeMouseEvent *)),
                     q,        SIGNAL(mouseReleased(WDeclarativeMouseEvent *)));

    QObject::connect(mainView, SIGNAL(mouseDoubleClicked(WDeclarativeMouseEvent *)),
                     q,        SIGNAL(mouseDoubleClicked(WDeclarativeMouseEvent *)));

    QObject::connect(mainView, SIGNAL(keyPressed(WDeclarativeKeyEvent *)),
                     q,        SIGNAL(keyPressed(WDeclarativeKeyEvent *)));

    QObject::connect(mainView, SIGNAL(keyReleased(WDeclarativeKeyEvent *)),
                     q,        SIGNAL(keyReleased(WDeclarativeKeyEvent *)));

    QObject::connect(mainView, SIGNAL(keyShiftPressedChanged()),
                     q,        SIGNAL(keyShiftPressedChanged()));

    QObject::connect(mainView, SIGNAL(keyControlPressedChanged()),
                     q,        SIGNAL(keyControlPressedChanged()));

    QObject::connect(mainView, SIGNAL(keyAltPressedChanged()),
                     q,        SIGNAL(keyAltPressedChanged()));

    QObject::connect(mainView, SIGNAL(availableGeometryChanged()),
                     q,        SIGNAL(availableGeometryChanged()));
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

/* Q_INVOKABLE */ void WWindow::showMinimized()
{
    Q_D(WWindow); d->mainView->setMinimized(true);
}

/* Q_INVOKABLE */ void WWindow::showMaximized()
{
    Q_D(WWindow); d->mainView->setMaximized(true);
}

/* Q_INVOKABLE */ void WWindow::showNormal()
{
    Q_D(WWindow); d->mainView->setMaximized(false);
}

/* Q_INVOKABLE */ void WWindow::showFullScreen()
{
    Q_D(WWindow); d->mainView->setFullScreen(true);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::activate()
{
    Q_D(WWindow); d->mainView->activate();
}

/* Q_INVOKABLE */ void WWindow::raise()
{
    Q_D(WWindow); d->mainView->raise();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::close()
{
    Q_D(WWindow); d->mainView->close();
}

//-------------------------------------------------------------------------------------------------
// Focus

/* Q_INVOKABLE */ bool WWindow::getFocus() const
{
    Q_D(const WWindow);

    return (d->mainView->d_func()->scene->focusItem() != NULL);
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
    Q_D(const WWindow); return d->mainView->getScreenNumber();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::setMinimumSize(int width, int height)
{
    Q_D(WWindow); d->mainView->setMinimumSize(width, height);
}

/* Q_INVOKABLE */ void WWindow::setMaximumSize(int width, int height)
{
    Q_D(WWindow); d->mainView->setMaximumSize(width, height);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QRect WWindow::getDefaultGeometry() const
{
    Q_D(const WWindow); return d->mainView->getDefaultGeometry();
}

/* Q_INVOKABLE */ void WWindow::setDefaultGeometry()
{
    Q_D(WWindow); d->mainView->setDefaultGeometry();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::saveGeometry()
{
    Q_D(WWindow); d->mainView->saveGeometry();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::checkPosition()
{
    Q_D(WWindow); return d->mainView->checkPosition();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::originTo(qreal x, qreal y)
{
    Q_D(WWindow); return d->mainView->originTo(x, y);
}

//-------------------------------------------------------------------------------------------------
// Drag

/* Q_INVOKABLE */ bool WWindow::testDrag(const QPointF & posA,
                                         const QPointF & posB, qreal distance)
{
    Q_D(WWindow); return d->mainView->testDrag(posA, posB, distance);
}

/* Q_INVOKABLE */ void WWindow::startDrag(const QString & text, int actions)
{
    Q_D(WWindow); d->mainView->startDrag(text, actions);
}

//-------------------------------------------------------------------------------------------------
// Hover

/* Q_INVOKABLE */ int WWindow::hoverCount() const
{
    Q_D(const WWindow); return d->mainView->hoverCount();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::updateHover()
{
    Q_D(WWindow); d->mainView->updateHover();
}

/* Q_INVOKABLE */ void WWindow::clearHover()
{
    Q_D(WWindow); d->mainView->clearHover();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::checkLeave(int msec)
{
    Q_D(WWindow); d->mainView->checkLeave(msec);
}

//-------------------------------------------------------------------------------------------------
// Shot

/* Q_INVOKABLE */ QPixmap WWindow::takeShot(int x, int y, int width, int height) const
{
    Q_D(const WWindow); return d->mainView->takeShot(x, y, width, height);
}

/* Q_INVOKABLE */ bool WWindow::saveShot(const QString & fileName, int x,     int y,
                                                                   int width, int height) const
{
    Q_D(const WWindow); return d->mainView->saveShot(fileName, x, y, width, height);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QPixmap WWindow::takeItemShot(QGraphicsObject * item,
                                                bool              recursive,
                                                const QColor    & background,
                                                bool              forceVisible) const
{
    Q_D(const WWindow);

    return d->mainView->takeItemShot(item, recursive, background, forceVisible);
}

/* Q_INVOKABLE */ bool WWindow::saveItemShot(const QString   & fileName,
                                             QGraphicsObject * item,
                                             bool              recursive,
                                             const QColor    & background,
                                             bool              forceVisible) const
{
    Q_D(const WWindow);

    return d->mainView->saveItemShot(fileName, item, recursive, background, forceVisible);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WWindow::compressShots(const QString & path, int quality)
{
    Q_D(const WWindow);

    return d->mainView->compressShots(path, quality);
}

//-------------------------------------------------------------------------------------------------
// Cursor

/* Q_INVOKABLE */ void WWindow::registerCursor(WDeclarativeMouseArea::CursorShape shape,
                                               const QCursor                    & cursor)
{
    Q_D(WWindow); d->mainView->registerCursor(shape, cursor);
}

/* Q_INVOKABLE */ void WWindow::unregisterCursor(WDeclarativeMouseArea::CursorShape shape)
{
    Q_D(WWindow); d->mainView->unregisterCursor(shape);
}

/* Q_INVOKABLE */ void WWindow::unregisterCursors()
{
    Q_D(WWindow); d->mainView->unregisterCursors();
}

//-------------------------------------------------------------------------------------------------
// Input

/* Q_INVOKABLE */ void WWindow::mouseMove(int x, int y, Qt::MouseButton button) const
{
    Q_D(const WWindow); d->mainView->mouseMove(x, y, button);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::mousePress(Qt::MouseButton button) const
{
    Q_D(const WWindow); d->mainView->mousePress(button);
}

/* Q_INVOKABLE */ void WWindow::mouseRelease(Qt::MouseButton button) const
{
    Q_D(const WWindow); d->mainView->mouseRelease(button);
}

/* Q_INVOKABLE */ void WWindow::mouseClick(Qt::MouseButton button, int msec) const
{
    Q_D(const WWindow); d->mainView->mouseClick(button, msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::wheel(Qt::Orientation orientation, int delta) const
{
    Q_D(const WWindow); d->mainView->wheel(orientation, delta);
}

/* Q_INVOKABLE */ void WWindow::wheelUp(int delta) const
{
    Q_D(const WWindow); d->mainView->wheelUp(delta);
}

/* Q_INVOKABLE */ void WWindow::wheelDown(int delta) const
{
    Q_D(const WWindow); d->mainView->wheelDown(delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::keyPress(int key, Qt::KeyboardModifiers modifiers) const
{
    Q_D(const WWindow); d->mainView->keyPress(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyRelease(int key, Qt::KeyboardModifiers modifiers) const
{
    Q_D(const WWindow); d->mainView->keyRelease(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyClick(int key, Qt::KeyboardModifiers modifiers, int msec) const
{
    Q_D(const WWindow); d->mainView->keyClick(key, modifiers, msec);
}

//-------------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE */ void WWindow::mouseMove(int x, int y, int button) const
{
    Q_D(const WWindow); d->mainView->mouseMove(x, y, button);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::mousePress(int button) const
{
    Q_D(const WWindow); d->mainView->mousePress(button);
}

/* Q_INVOKABLE */ void WWindow::mouseRelease(int button) const
{
    Q_D(const WWindow); d->mainView->mouseRelease(button);
}

/* Q_INVOKABLE */ void WWindow::mouseClick(int button, int msec) const
{
    Q_D(const WWindow); d->mainView->mouseClick(button, msec);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::wheel(int orientation, int delta) const
{
    Q_D(const WWindow); d->mainView->wheel(orientation, delta);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::keyPress(int key, int modifiers) const
{
    Q_D(const WWindow); d->mainView->keyPress(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyRelease(int key, int modifiers) const
{
    Q_D(const WWindow); d->mainView->keyRelease(key, modifiers);
}

/* Q_INVOKABLE */ void WWindow::keyClick(int key, int modifiers, int msec) const
{
    Q_D(const WWindow); d->mainView->keyClick(key, modifiers, msec);
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
    Q_D(WWindow); d->mainView->registerCursorUrl(shape, url, size);
}

/* Q_INVOKABLE */ void WWindow::unregisterCursor(int shape)
{
    Q_D(WWindow); d->mainView->unregisterCursor(shape);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WWindow * WWindow::window()
{
    return this;
}

WMainView * WWindow::mainView() const
{
    Q_D(const WWindow); return d->mainView;
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

    d->mainView->setWindowIcon(QIcon(icon));

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

    d->mainView->setVisible(visible);

    emit visibleChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::opacity() const
{
    Q_D(const WWindow); return d->mainView->windowOpacity();
}

void WWindow::setOpacity(qreal opacity)
{
    Q_D(WWindow);

    if (d->mainView->windowOpacity() == opacity) return;

    d->mainView->setWindowOpacity(opacity);

    emit opacityChanged();
}

//-------------------------------------------------------------------------------------------------
// WMainView
//-------------------------------------------------------------------------------------------------

qreal WWindow::itemWidth() const
{
    Q_D(const WWindow); return d->mainView->itemWidth();
}

qreal WWindow::itemHeight() const
{
    Q_D(const WWindow); return d->mainView->itemHeight();
}

//-------------------------------------------------------------------------------------------------

int WWindow::x() const
{
    Q_D(const WWindow); return d->mainView->x();
}

void WWindow::setX(int x)
{
    Q_D(WWindow); d->mainView->setX(x);
}

int WWindow::y() const
{
    Q_D(const WWindow); return d->mainView->y();
}

void WWindow::setY(int y)
{
    Q_D(WWindow); d->mainView->setY(y);
}

//-------------------------------------------------------------------------------------------------

int WWindow::width() const
{
    Q_D(const WWindow); return d->mainView->width();
}

void WWindow::setWidth(int width)
{
    Q_D(WWindow); d->mainView->setWidth(width);
}

int WWindow::height() const
{
    Q_D(const WWindow); return d->mainView->height();
}

void WWindow::setHeight(int height)
{
    Q_D(WWindow); d->mainView->setHeight(height);
}

//-------------------------------------------------------------------------------------------------

int WWindow::centerX() const
{
    Q_D(const WWindow); return d->mainView->centerX();
}

int WWindow::centerY() const
{
    Q_D(const WWindow); return d->mainView->centerY();
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::originX() const
{
    Q_D(const WWindow); return d->mainView->originX();
}

void WWindow::setOriginX(qreal x)
{
    Q_D(WWindow); d->mainView->setOriginX(x);
}

qreal WWindow::originY() const
{
    Q_D(const WWindow); return d->mainView->originY();
}

void WWindow::setOriginY(qreal y)
{
    Q_D(WWindow); d->mainView->setOriginY(y);
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::zoom() const
{
    Q_D(const WWindow); return d->mainView->zoom();
}

void WWindow::setZoom(qreal zoom)
{
    Q_D(WWindow); d->mainView->setZoom(zoom);
}

//-------------------------------------------------------------------------------------------------

int WWindow::minimumWidth() const
{
    Q_D(const WWindow); return d->mainView->minimumWidth();
}

void WWindow::setMinimumWidth(int width)
{
    Q_D(WWindow); d->mainView->setMinimumWidth(width);
}

int WWindow::minimumHeight() const
{
    Q_D(const WWindow); return d->mainView->minimumHeight();
}

void WWindow::setMinimumHeight(int height)
{
    Q_D(WWindow); d->mainView->setMinimumHeight(height);
}

//-------------------------------------------------------------------------------------------------

int WWindow::maximumWidth() const
{
    Q_D(const WWindow); return d->mainView->maximumWidth();
}

void WWindow::setMaximumWidth(int width)
{
    Q_D(WWindow); d->mainView->setMaximumWidth(width);
}

int WWindow::maximumHeight() const
{
    Q_D(const WWindow); return d->mainView->maximumHeight();
}

void WWindow::setMaximumHeight(int height)
{
    Q_D(WWindow); d->mainView->setMaximumHeight(height);
}

//-------------------------------------------------------------------------------------------------

QRect WWindow::geometryNormal() const
{
    Q_D(const WWindow); return d->mainView->geometryNormal();
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isMinimized() const
{
    Q_D(const WWindow); return d->mainView->isMinimized();
}

void WWindow::setMinimized(bool minimized)
{
    Q_D(WWindow); d->mainView->setMinimized(minimized);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isMaximized() const
{
    Q_D(const WWindow); return d->mainView->isMaximized();
}

void WWindow::setMaximized(bool maximized)
{
    Q_D(WWindow); d->mainView->setMaximized(maximized);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isFullScreen() const
{
    Q_D(const WWindow); return d->mainView->isFullScreen();
}

void WWindow::setFullScreen(bool fullScreen)
{
    Q_D(WWindow); d->mainView->setFullScreen(fullScreen);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isLocked() const
{
    Q_D(const WWindow); return d->mainView->isLocked();
}

void WWindow::setLocked(bool locked)
{
    Q_D(WWindow);

    d->mainView->setLocked(locked);

    bool visible = d->mainView->isVisible();

    if (d->visible != visible)
    {
        d->visible = visible;

        emit visibleChanged();
    }
}

//-------------------------------------------------------------------------------------------------

bool WWindow::isActive() const
{
    Q_D(const WWindow); return d->mainView->isActiveWindow();
}

bool WWindow::isEntered() const
{
    Q_D(const WWindow); return d->mainView->isEntered();
}

bool WWindow::isDragging() const
{
    Q_D(const WWindow); return d->mainView->isDragging();
}

bool WWindow::isDragged() const
{
    Q_D(const WWindow); return d->mainView->isDragged();
}

bool WWindow::isResizing() const
{
    Q_D(const WWindow); return d->mainView->isResizing();
}

//-------------------------------------------------------------------------------------------------

QPoint WWindow::mousePos() const
{
    Q_D(const WWindow); return d->mainView->mousePos();
}

//-------------------------------------------------------------------------------------------------

int WWindow::mouseX() const
{
    Q_D(const WWindow); return d->mainView->mouseX();
}

int WWindow::mouseY() const
{
    Q_D(const WWindow); return d->mainView->mouseY();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeMouseArea::CursorShape WWindow::mouseCursor() const
{
    Q_D(const WWindow); return d->mainView->mouseCursor();
}

//-------------------------------------------------------------------------------------------------

bool WWindow::autoSize() const
{
    Q_D(const WWindow); return d->mainView->autoSize();
}

void WWindow::setAutoSize(bool autoSize)
{
    Q_D(WWindow); d->mainView->setAutoSize(autoSize);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::opengl() const
{
    Q_D(const WWindow); return d->mainView->opengl();
}

void WWindow::setOpengl(bool enabled)
{
    Q_D(WWindow); d->mainView->setOpengl(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::antialias() const
{
    Q_D(const WWindow); return d->mainView->antialias();
}

void WWindow::setAntialias(bool enabled)
{
    Q_D(WWindow); d->mainView->setAntialias(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::vsync() const
{
    Q_D(const WWindow); return d->mainView->vsync();
}

void WWindow::setVsync(bool enabled)
{
    Q_D(WWindow); d->mainView->setVsync(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::hoverEnabled() const
{
    Q_D(const WWindow); return d->mainView->hoverEnabled();
}

void WWindow::setHoverEnabled(bool enabled)
{
    Q_D(WWindow); d->mainView->setHoverEnabled(enabled);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::fadeEnabled() const
{
    Q_D(const WWindow); return d->mainView->fadeEnabled();
}

void WWindow::setFadeEnabled(bool enabled)
{
    Q_D(WWindow); d->mainView->setFadeEnabled(enabled);
}

int WWindow::fadeDuration() const
{
    Q_D(const WWindow); return d->mainView->fadeDuration();
}

void WWindow::setFadeDuration(int msec)
{
    Q_D(WWindow); d->mainView->setFadeDuration(msec);
}

//-------------------------------------------------------------------------------------------------
// Mouse idle

bool WWindow::idleCheck() const
{
    Q_D(const WWindow); return d->mainView->idleCheck();
}

void WWindow::setIdleCheck(bool check)
{
    Q_D(WWindow); d->mainView->setIdleCheck(check);
}

//-------------------------------------------------------------------------------------------------

bool WWindow::idle() const
{
    Q_D(const WWindow); return d->mainView->idle();
}

void WWindow::setIdle(bool idle)
{
    Q_D(WWindow); d->mainView->setIdle(idle);
}

//-------------------------------------------------------------------------------------------------

int WWindow::idleDelay() const
{
    Q_D(const WWindow); return d->mainView->idleDelay();
}

void WWindow::setIdleDelay(int msec)
{
    Q_D(WWindow); d->mainView->setIdleDelay(msec);
}

//-------------------------------------------------------------------------------------------------
// Keys

bool WWindow::keyShiftPressed() const
{
    Q_D(const WWindow); return d->mainView->keyShiftPressed();
}

bool WWindow::keyControlPressed() const
{
    Q_D(const WWindow); return d->mainView->keyControlPressed();
}

bool WWindow::keyAltPressed() const
{
    Q_D(const WWindow); return d->mainView->keyAltPressed();
}

//-------------------------------------------------------------------------------------------------
// Screen

QRect WWindow::availableGeometry() const
{
    Q_D(const WWindow); return d->mainView->availableGeometry();
}

QRect WWindow::screenGeometry() const
{
    Q_D(const WWindow); return d->mainView->screenGeometry();
}

#endif // SK_NO_WINDOW
