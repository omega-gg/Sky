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

#include "WAbstractView.h"

#ifndef SK_NO_ABSTRACTVIEW

#ifdef SK_WIN_NATIVE
// Qt includes
#include <QApplication>
#include <QDesktopWidget>
#include <QFocusEvent>
#include <QIcon>
#endif

#if defined(SK_WIN_NATIVE) && defined(QT_LATEST)
// Windows includes
#ifdef _MSC_VER
// FIXME MSVC: This fixes FORMATETC declaration.
#include <objidl.h>
#endif
#include <QtWinExtras>
#endif

//-------------------------------------------------------------------------------------------------
// Static variables

#ifdef SK_WIN_NATIVE
static const DWORD ABSTRACTVIEW_FLAGS = WS_OVERLAPPED  | WS_THICKFRAME | WS_MINIMIZEBOX |
                                        WS_MAXIMIZEBOX | WS_CLIPCHILDREN;

#ifdef QT_LATEST
static const int ABSTRACTVIEW_INTERVAL = 400;

static const int ABSTRACTVIEW_DELAY = 5000;
#endif
#endif

#ifdef QT_LATEST
//-------------------------------------------------------------------------------------------------
// Global variables

QWindow * view = NULL;

int count = 0;
#endif

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WAbstractViewPrivate::WAbstractViewPrivate(WAbstractView * p) : WPrivate(p) {}

#ifdef SK_WIN_NATIVE
/* virtual */ WAbstractViewPrivate::~WAbstractViewPrivate()
{
    DestroyWindow(handle);

#ifdef QT_LATEST
    count--;

    if (count == 0 && view)
    {
        delete view;

        view = NULL;
    }
#endif
}
#endif

//-------------------------------------------------------------------------------------------------

void WAbstractViewPrivate::init(Qt::WindowFlags flags)
{
    Q_Q(WAbstractView);

    this->flags = flags;

#ifndef SK_WIN_NATIVE
#ifdef QT_4
    q->setWindowFlags(flags);
#else
    q->setFlags(Qt::Window | flags);
#endif
#else
#ifdef QT_4
    q->setWindowFlags(Qt::FramelessWindowHint);

    id = q->QDeclarativeView::winId();
#else
    q->setFlags(Qt::Window | Qt::FramelessWindowHint);

    WId winId = q->QQuickWindow::winId();

    id = (HWND) winId;

    viewport = q->fromWinId(winId);
#endif

    x = 0;
    y = 0;

    width  = 0;
    height = 0;

    minimumWidth  = 0;
    minimumHeight = 0;

    maximumWidth  = QWIDGETSIZE_MAX;
    maximumHeight = QWIDGETSIZE_MAX;

    visible = false;
    opacity = 1.0;

    maximized  = false;
    fullScreen = false;

    windowSnap     = true;
    windowMaximize = true;
    windowClip     = false;

    version = 0;

    const QMetaObject * meta = q->metaObject();

    method = meta->method(meta->indexOfMethod("onFocus()"));

#ifdef QT_LATEST
    timer.setInterval(ABSTRACTVIEW_INTERVAL);

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(onMove()));
#endif

    //---------------------------------------------------------------------------------------------

    WNDCLASSEX wcx;

    wcx.cbSize = sizeof(WNDCLASSEX);

    wcx.style = CS_HREDRAW | CS_VREDRAW;

    wcx.lpfnWndProc = events;

    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = 0;

    wcx.hInstance = 0;

    wcx.hCursor = 0;

    wcx.hbrBackground = 0;

    wcx.lpszMenuName  = 0;
    wcx.lpszClassName = L"Window";

    HINSTANCE application = GetModuleHandle(0);

    wcx.hIcon = (HICON) LoadImage(application, L"IDI_ICON1", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);

    if (wcx.hIcon)
    {
        int width  = GetSystemMetrics(SM_CXSMICON);
        int height = GetSystemMetrics(SM_CYSMICON);

        wcx.hIconSm = (HICON) LoadImage(application, L"IDI_ICON1", IMAGE_ICON, width, height, 0);
    }
    else wcx.hIconSm = 0;

    RegisterClassEx(&wcx);

    handle = CreateWindow(L"Window", 0, ABSTRACTVIEW_FLAGS, 0, 0, 0, 0, 0, 0, 0, NULL);

    SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (q));

#ifdef QT_4
    SetWindowLong(id, GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN);
#else
    SetWindowLong(id, GWL_STYLE, WS_CHILD);

    q->setProperty("_q_embedded_native_parent_handle", (WId) handle);
#endif

    SetParent(id, handle);

#ifdef QT_LATEST
    // FIXME Qt5 Windows: We need to create a QWindow to receive QScreen events.
    if (count == 0)
    {
        QTimer::singleShot(ABSTRACTVIEW_DELAY, q, SLOT(onCreate()));
    }

    count++;
#endif
#endif // SK_WIN_NATIVE
}

#ifdef SK_WIN_NATIVE

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WAbstractViewPrivate::applyFullScreen()
{
    Q_Q(WAbstractView);

    // FIXME Windows: The task bar overlaps the window.
    if (isWindows10())
    {
        SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) & ~WS_CAPTION);
    }

#ifdef QT_4
    QRect geometry = qApp->desktop()->screenGeometry(q);
#else
    QRect geometry = q->screen()->geometry();
#endif

    q->setGeometry(geometry);
}

void WAbstractViewPrivate::restoreFullScreen()
{
    Q_Q(WAbstractView);

    fullScreen = false;

    q->setGeometry(rect);

    // FIXME Windows: The task bar overlaps the window.
    if (isWindows10())
    {
        SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) | WS_CAPTION);
    }
}

//-------------------------------------------------------------------------------------------------

void WAbstractViewPrivate::setFlag(LONG flag, bool enabled) const
{
    if (enabled)
    {
         SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) | flag);
    }
    else SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) & ~flag);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractViewPrivate::isWindows10()
{
    if (version == 0)
    {
#ifdef QT_4
        if (QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS10)
        {
             version = 10;
        }
        else version = 1;
#else
        version = QSysInfo::productVersion().toInt();
#endif
    }

    if (version < 10)
    {
         return false;
    }
    else return true;
}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ HICON WAbstractViewPrivate::getIcon(const QIcon & icon, int width, int height)
{
    QSize size = icon.actualSize(QSize(width, height));

    QPixmap pixmap = icon.pixmap(size);

    if (pixmap.isNull())
    {
         return NULL;
    }
#ifdef QT_4
    else return pixmap.toWinHICON();
#else
    else return QtWin::toHICON(pixmap);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private static events
//-------------------------------------------------------------------------------------------------

/* static */ LRESULT CALLBACK WAbstractViewPrivate::events(HWND handle, UINT   message,
                                                                        WPARAM wParam,
                                                                        LPARAM lParam)
{
    if (message == WM_ACTIVATE)
    {
        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        WAbstractViewPrivate * d = view->d_func();

        if (d->fullScreen)
        {
            d->applyFullScreen();
        }

        return 0;
    }
    else if (message == WM_SETFOCUS)
    {
        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        WAbstractViewPrivate * d = view->d_func();

        // FIXME Windows: Sometimes the focus fails.
        if (SetFocus(d->id) == NULL)
        {
            d->method.invoke(view, Qt::QueuedConnection);
        }

        return 0;
    }
    else if (message == WM_NCCALCSIZE)
    {
        return 0;
    }
    else if (message == WM_GETMINMAXINFO)
    {
        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        if (view == NULL) return 0;

        WAbstractViewPrivate * d = view->d_func();

        MINMAXINFO * info = (MINMAXINFO *) lParam;

        info->ptMinTrackSize.x = d->minimumWidth;
        info->ptMinTrackSize.y = d->minimumHeight;

        info->ptMaxTrackSize.x = d->maximumWidth;
        info->ptMaxTrackSize.y = d->maximumHeight;

        return 0;
    }
    else if (message == WM_MOVE)
    {
        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        if (view == NULL) return 0;

        WAbstractViewPrivate * d = view->d_func();

        RECT rect;

        GetWindowRect(handle, &rect);

        d->x = rect.left;
        d->y = rect.top;

#ifdef QT_4
        view->QDeclarativeView::move(0, 0);
#else
        d->timer.start();
#endif

        return 0;
    }
    else if (message == WM_SIZE)
    {
        WINDOWPLACEMENT placement;

        placement.length = sizeof(WINDOWPLACEMENT);

        GetWindowPlacement(handle, &placement);

        if (placement.showCmd == SW_SHOWMINIMIZED) return 0;

        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        if (view == NULL) return 0;

        WAbstractViewPrivate * d = view->d_func();

        RECT rect;

        GetClientRect(handle, &rect);

        if (placement.showCmd == SW_MAXIMIZE)
        {
            RECT adjust = { 0, 0, 0, 0 };

            UINT style = GetWindowLong(handle, GWL_STYLE);

            AdjustWindowRectEx(&adjust, style, FALSE, 0);

            int border   = adjust.right;
            int border2x = border * 2;

            int width  = rect.right  - border2x;
            int height = rect.bottom - border2x;

            d->width  = width;
            d->height = height;

#ifdef QT_4
            view->QDeclarativeView::setGeometry(border, border, width, height);
#elif defined(Q_OS_WIN)
            // FIXME Qt5 Windows: Workaround for opengl full screen flicker.
            d->viewport->setGeometry(border, border, width + 1, height);
#else
            d->viewport->setGeometry(border, border, width, height);
#endif

            if (d->maximized == false)
            {
                d->maximized = true;

                view->onStateChanged(Qt::WindowMaximized);
            }
        }
        else
        {
            int width  = rect.right;
            int height = rect.bottom;

            d->width  = width;
            d->height = height;

#ifdef QT_4
            view->QDeclarativeView::setGeometry(0, 0, width, height);
#elif defined(Q_OS_WIN)
            // FIXME Qt5 Windows: Workaround for opengl full screen flicker.
            if (d->fullScreen)
            {
                 d->viewport->setGeometry(0, 0, width + 1, height);
            }
            else d->viewport->setGeometry(0, 0, width, height);
#else
            d->viewport->setGeometry(0, 0, width, height);
#endif

            if (d->maximized)
            {
                d->maximized = false;

                view->onStateChanged(Qt::WindowNoState);
            }
        }

        return 0;
    }
#ifdef QT_LATEST
    // FIXME Qt5.12: We have to send mouse release manually when dragging and resizing.
    else if (message == WM_CAPTURECHANGED)
    {
        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        QPoint pos = QCursor::pos();

        QMouseEvent event(QEvent::MouseButtonRelease, view->mapFromGlobal(pos), pos,
                          Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

        QCoreApplication::sendEvent(view, &event);

        return 0;
    }
#endif
    else if (message == WM_CLOSE)
    {
        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        view->onClose();

        return 0;
    }
    else return DefWindowProc(handle, message, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

#ifdef QT_LATEST

// FIXME Qt5 Windows: We need to create a QWindow to receive QScreen events.
void WAbstractViewPrivate::onCreate()
{
    view = new QWindow;

    view->setFlags(Qt::Tool | Qt::FramelessWindowHint);

    view->resize(0, 0);

    view->show();
    view->hide();
}

void WAbstractViewPrivate::onMove()
{
    if (maximized || fullScreen) return;

    QSize size = viewport->size();

    viewport->resize(size.width() + 1, size.height());
    viewport->resize(size);
}

#endif

void WAbstractViewPrivate::onFocus()
{
    SetFocus(id);
}

#endif // SK_WIN_NATIVE

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
WAbstractView::WAbstractView(QWidget * parent, Qt::WindowFlags flags)
    : QDeclarativeView(parent), WPrivatable(new WAbstractViewPrivate(this))
#else
WAbstractView::WAbstractView(QWindow * parent, Qt::WindowFlags flags)
    : QQuickWindow(parent), WPrivatable(new WAbstractViewPrivate(this))
#endif
{
    Q_D(WAbstractView); d->init(flags);
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WAbstractView::WAbstractView(WAbstractViewPrivate * p, QWidget * parent, Qt::WindowFlags flags)
    : QDeclarativeView(parent), WPrivatable(p)
#else
WAbstractView::WAbstractView(WAbstractViewPrivate * p, QWindow * parent, Qt::WindowFlags flags)
    : QQuickWindow(parent), WPrivatable(p)
#endif
{
    Q_D(WAbstractView); d->init(flags);
}

#ifdef SK_WIN_NATIVE

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::show()
{
    setVisible(true);
}

/* Q_INVOKABLE */ void WAbstractView::hide()
{
    setVisible(false);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::showNormal()
{
    Q_D(WAbstractView);

    if (d->maximized)
    {
        d->maximized = false;
    }
    else if (d->fullScreen)
    {
        d->restoreFullScreen();

        return;
    }

    ShowWindow(d->handle, SW_RESTORE);
}

/* Q_INVOKABLE */ void WAbstractView::showMaximized()
{
    Q_D(WAbstractView);

    if (d->maximized) return;

    if (d->fullScreen)
    {
        d->restoreFullScreen();

        d->maximized = true;

        // FIXME Windows 10: Hiding the window to avoid the animation.
        if (d->isWindows10())
        {
            ShowWindow(d->handle, SW_HIDE);
        }
    }
    else d->maximized = true;

    ShowWindow(d->handle, SW_SHOWMAXIMIZED);
}

/* Q_INVOKABLE */ void WAbstractView::showFullScreen()
{
    Q_D(WAbstractView);

    if (d->fullScreen) return;

    if (d->maximized)
    {
        d->maximized = false;

        // FIXME Windows 10: Hiding the window to avoid the animation.
        if (d->isWindows10())
        {
            ShowWindow(d->handle, SW_HIDE);
        }

        ShowWindow(d->handle, SW_RESTORE);
    }

    d->fullScreen = true;

    d->rect = QRect(x(), y(), width(), height());

    d->applyFullScreen();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::showMinimized()
{
    Q_D(WAbstractView);

    ShowWindow(d->handle, SW_SHOWMINIMIZED);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::raise()
{
    Q_D(WAbstractView);

    SetWindowPos(d->handle, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
}

/* Q_INVOKABLE */ void WAbstractView::lower()
{
    Q_D(WAbstractView);

    SetWindowPos(d->handle, HWND_BOTTOM, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* Q_INVOKABLE */ void WAbstractView::move(int x, int y)
#else
/* Q_INVOKABLE */ void WAbstractView::setPosition(int x, int y)
#endif
{
    Q_D(WAbstractView);

    SetWindowPos(d->handle, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
}

#ifdef QT_4
/* Q_INVOKABLE */ void WAbstractView::move(const QPoint & position)
#else
/* Q_INVOKABLE */ void WAbstractView::setPosition(const QPoint & position)
#endif
{
#ifdef QT_4
    move(position.x(), position.y());
#else
    setPosition(position.x(), position.y());
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::resize(int width, int height)
{
    Q_D(WAbstractView);

    SetWindowPos(d->handle, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
}

/* Q_INVOKABLE */ void WAbstractView::resize(const QSize & size)
{
    resize(size.width(), size.height());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::setGeometry(int x, int y, int width, int height)
{
    Q_D(WAbstractView);

    SetWindowPos(d->handle, HWND_TOP, x, y, width, height, 0);
}

/* Q_INVOKABLE */ void WAbstractView::setGeometry(const QRect & rect)
{
    setGeometry(rect.x(), rect.y(), rect.width(), rect.height());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::setMinimumSize(int width, int height)
{
    Q_D(WAbstractView);

    d->minimumWidth  = width;
    d->minimumHeight = height;

    if (d->width < width)
    {
        if (d->height < height)
        {
             SetWindowPos(d->handle, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
        }
        else SetWindowPos(d->handle, HWND_TOP, 0, 0, width, d->height, SWP_NOMOVE);
    }
    else if (d->height < height)
    {
        SetWindowPos(d->handle, HWND_TOP, 0, 0, d->width, height, SWP_NOMOVE);
    }
}

/* Q_INVOKABLE */ void WAbstractView::setMaximumSize(int width, int height)
{
    Q_D(WAbstractView);

    d->maximumWidth  = width;
    d->maximumHeight = height;

    if (d->width > width)
    {
        if (d->height > height)
        {
             SetWindowPos(d->handle, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
        }
        else SetWindowPos(d->handle, HWND_TOP, 0, 0, width, d->height, SWP_NOMOVE);
    }
    else if (d->height > height)
    {
        SetWindowPos(d->handle, HWND_TOP, 0, 0, d->width, height, SWP_NOMOVE);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::setMinimumSize(const QSize & size)
{
    setMinimumSize(size.width(), size.height());
}

/* Q_INVOKABLE */ void WAbstractView::setMaximumSize(const QSize & size)
{
    setMaximumSize(size.width(), size.height());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::setMinimumWidth(int width)
{
    Q_D(WAbstractView);

    d->minimumWidth = width;

    if (d->width < width)
    {
        SetWindowPos(d->handle, HWND_TOP, 0, 0, width, d->height, SWP_NOMOVE);
    }
}

/* Q_INVOKABLE */ void WAbstractView::setMinimumHeight(int height)
{
    Q_D(WAbstractView);

    d->minimumHeight = height;

    if (d->height < height)
    {
        SetWindowPos(d->handle, HWND_TOP, 0, 0, d->width, height, SWP_NOMOVE);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::setMaximumWidth(int width)
{
    Q_D(WAbstractView);

    d->maximumWidth = width;

    if (d->width > width)
    {
        SetWindowPos(d->handle, HWND_TOP, 0, 0, width, d->height, SWP_NOMOVE);
    }
}

/* Q_INVOKABLE */ void WAbstractView::setMaximumHeight(int height)
{
    Q_D(WAbstractView);

    d->maximumHeight = height;

    if (d->height > height)
    {
        SetWindowPos(d->handle, HWND_TOP, 0, 0, d->width, height, SWP_NOMOVE);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::setVisible(bool visible)
{
    Q_D(WAbstractView);

    if (d->visible == visible) return;

    d->visible = visible;

#ifdef QT_4
    QDeclarativeView::setVisible(visible);
#else
    QQuickWindow::setVisible(visible);
#endif

    if (visible)
    {
         ShowWindow(d->handle, SW_SHOW);
    }
    else ShowWindow(d->handle, SW_HIDE);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::setFocus()
{
    Q_D(WAbstractView);

    SetFocus(d->id);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* Q_INVOKABLE */ void WAbstractView::setWindowIcon(const QIcon & icon)
#else
/* Q_INVOKABLE */ void WAbstractView::setIcon(const QIcon & icon)
#endif
{
    Q_D(WAbstractView);

    HICON iconSmall = WAbstractViewPrivate::getIcon(icon, GetSystemMetrics(SM_CXSMICON),
                                                          GetSystemMetrics(SM_CYSMICON));

    HICON iconBig = WAbstractViewPrivate::getIcon(icon, GetSystemMetrics(SM_CXICON),
                                                        GetSystemMetrics(SM_CYICON));

    if (iconBig)
    {
        PostMessage(d->handle, WM_SETICON, ICON_SMALL, (LPARAM) iconSmall);
        PostMessage(d->handle, WM_SETICON, ICON_BIG,   (LPARAM) iconBig);
    }
    else
    {
        PostMessage(d->handle, WM_SETICON, ICON_SMALL, (LPARAM) iconSmall);
        PostMessage(d->handle, WM_SETICON, ICON_BIG,   (LPARAM) iconSmall);
    }
}

#ifdef QT_4
/* Q_INVOKABLE */ void WAbstractView::setWindowTitle(const QString & title)
#else
/* Q_INVOKABLE */ void WAbstractView::setTitle(const QString & title)
#endif
{
    Q_D(WAbstractView);

    SetWindowText(d->handle, (wchar_t *) title.utf16());
}

#endif // SK_WIN_NATIVE

#ifdef Q_OS_WIN

//-------------------------------------------------------------------------------------------------

#ifdef SK_WIN_NATIVE
/* Q_INVOKABLE */ void WAbstractView::setWindowSnap(bool enabled)
{
    Q_D(WAbstractView);

    if (d->windowSnap == enabled) return;

    d->windowSnap = enabled;

    d->setFlag(WS_THICKFRAME, enabled);
}
#else
/* Q_INVOKABLE */ void WAbstractView::setWindowSnap(bool) {}
#endif

#ifdef SK_WIN_NATIVE
/* Q_INVOKABLE */ void WAbstractView::setWindowMaximize(bool enabled)
{
    Q_D(WAbstractView);

    if (d->windowMaximize == enabled) return;

    d->windowMaximize = enabled;

    d->setFlag(WS_MAXIMIZEBOX, enabled);
}
#else
/* Q_INVOKABLE */ void WAbstractView::setWindowMaximize(bool) {}
#endif

#ifdef SK_WIN_NATIVE
/* Q_INVOKABLE */ void WAbstractView::setWindowClip(bool enabled)
{
    Q_D(WAbstractView);

    if (d->windowClip == enabled) return;

    d->windowClip = enabled;

    d->setFlag(WS_CLIPCHILDREN, enabled);
}
#else
/* Q_INVOKABLE */ void WAbstractView::setWindowClip(bool) {}
#endif

#endif // Q_OS_WIN

#ifdef SK_WIN_NATIVE

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WAbstractView::close()
{
    Q_D(WAbstractView);

    ShowWindow(d->handle, SW_HIDE);

#ifdef QT_4
    return QWidget::close();
#else
    return QQuickWindow::close();
#endif
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ bool WAbstractView::winEvent(MSG * msg, long * result)
#else
/* virtual */ bool WAbstractView::nativeEvent(const QByteArray & event, void * msg, long * result)
#endif
{
#ifdef QT_4
    UINT message = msg->message;
#else
    UINT message = static_cast<MSG *> (msg)->message;
#endif

    if (message == WM_SETFOCUS)
    {
        QFocusEvent event(QEvent::FocusIn);

        QCoreApplication::sendEvent(this, &event);
    }
    else if (message == WM_KILLFOCUS)
    {
        QFocusEvent event(QEvent::FocusOut);

        QCoreApplication::sendEvent(this, &event);
    }

#ifdef QT_4
    return QDeclarativeView::winEvent(msg, result);
#else
    return QQuickWindow::nativeEvent(event, msg, result);
#endif
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractView::onStateChanged(Qt::WindowState) {}

/* virtual */ void WAbstractView::onClose()
{
    close();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WId WAbstractView::winId() const
{
    Q_D(const WAbstractView); return (WId) d->handle;
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_LATEST

QScreen * WAbstractView::screen() const
{
    Q_D(const WAbstractView);

    return fromWinId((WId) d->id)->screen();
}

#endif

//-------------------------------------------------------------------------------------------------

QRect WAbstractView::geometry() const
{
    Q_D(const WAbstractView);

    return QRect(d->x, d->y, d->width, d->height);
}

//-------------------------------------------------------------------------------------------------

int WAbstractView::x() const
{
    Q_D(const WAbstractView); return d->x;
}

int WAbstractView::y() const
{
    Q_D(const WAbstractView); return d->y;
}

//-------------------------------------------------------------------------------------------------

int WAbstractView::width() const
{
    Q_D(const WAbstractView); return d->width;
}

int WAbstractView::height() const
{
    Q_D(const WAbstractView); return d->height;
}

//-------------------------------------------------------------------------------------------------

int WAbstractView::minimumWidth() const
{
    Q_D(const WAbstractView); return d->minimumWidth;
}

int WAbstractView::minimumHeight() const
{
    Q_D(const WAbstractView); return d->minimumHeight;
}

//-------------------------------------------------------------------------------------------------

int WAbstractView::maximumWidth() const
{
    Q_D(const WAbstractView); return d->maximumWidth;
}

int WAbstractView::maximumHeight() const
{
    Q_D(const WAbstractView); return d->maximumHeight;
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
qreal WAbstractView::windowOpacity() const
#else
qreal WAbstractView::opacity() const
#endif
{
    Q_D(const WAbstractView);

    return d->opacity;
}

#ifdef QT_4
void WAbstractView::setWindowOpacity(qreal level)
#else
void WAbstractView::setOpacity(qreal level)
#endif
{
    Q_D(WAbstractView);

    if (level == 1.0)
    {
        d->opacity = level;

        SetLayeredWindowAttributes(d->handle, 0, 255, LWA_ALPHA);

        SetWindowLong(d->handle,
                      GWL_EXSTYLE, GetWindowLong(d->handle, GWL_EXSTYLE) & ~WS_EX_LAYERED);
    }
    else
    {
        qreal oldOpacity = d->opacity;

        d->opacity = level;

        SetLayeredWindowAttributes(d->handle, 0, level * 255, LWA_ALPHA);

        if (oldOpacity == 1.0)
        {
            SetWindowLong(d->handle,
                          GWL_EXSTYLE, GetWindowLong(d->handle, GWL_EXSTYLE) | WS_EX_LAYERED);
        }
    }
}

#endif // SK_WIN_NATIVE

#endif // SK_NO_ABSTRACTVIEW
