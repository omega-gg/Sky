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

#include "WAbstractView.h"

#ifndef SK_NO_ABSTRACTVIEW

// Qt includes
#ifdef SK_WIN_NATIVE
#include <QApplication>
#include <QDesktopWidget>
#include <QFocusEvent>
#include <QIcon>
#ifdef QT_LATEST
#include <QtWinExtras>
#endif
#endif

#ifdef SK_WIN_NATIVE
#include <dwmapi.h>
#endif

//-------------------------------------------------------------------------------------------------
// Static variables

#ifdef SK_WIN_NATIVE
static const DWORD windowFlags = WS_OVERLAPPED | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
#endif

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WAbstractViewPrivate::WAbstractViewPrivate(WAbstractView * p) : WPrivate(p) {}

#ifdef SK_WIN_NATIVE
/* virtual */ WAbstractViewPrivate::~WAbstractViewPrivate()
{
    DestroyWindow(handle);
}
#endif

//-------------------------------------------------------------------------------------------------

void WAbstractViewPrivate::init(Qt::WindowFlags flags)
{
    Q_Q(WAbstractView);

    this->flags = flags;

#ifndef SK_WIN_NATIVE
    q->setWindowFlags(flags);
#else

#ifdef QT_4
    id = q->QDeclarativeView::winId();
#else
    id = (HWND) q->QDeclarativeView::winId();
#endif

    x = 0;
    y = 0;

    width  = 0;
    height = 0;

    opacity = 0.0;

    maximized  = false;
    fullScreen = false;

    windowSnap     = true;
    windowMaximize = true;

    const QMetaObject * meta = q->metaObject();

    method = meta->method(meta->indexOfMethod("onFocus()"));

    //---------------------------------------------------------------------------------------------

    WNDCLASSEX wcx;

    wcx.cbSize = sizeof(WNDCLASSEX);

    wcx.style = CS_HREDRAW | CS_VREDRAW;

    wcx.lpfnWndProc = events;

    wcx.cbClsExtra	= 0;
    wcx.cbWndExtra	= 0;

    wcx.hInstance = 0;

    wcx.hCursor = 0;

    wcx.hbrBackground = 0;

    wcx.lpszMenuName  = 0;
    wcx.lpszClassName = L"Window";

    HINSTANCE application = qWinAppInst();

    wcx.hIcon = (HICON) LoadImage(application, L"IDI_ICON1", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);

    if (wcx.hIcon)
    {
        int width  = GetSystemMetrics(SM_CXSMICON);
        int height = GetSystemMetrics(SM_CYSMICON);

        wcx.hIconSm = (HICON) LoadImage(application, L"IDI_ICON1", IMAGE_ICON, width, height, 0);
    }
    else wcx.hIconSm = 0;

    RegisterClassEx(&wcx);

    handle = CreateWindow(L"Window", 0, windowFlags, 0, 0, 0, 0, 0, 0, 0, NULL);

    SetWindowLong(handle, GWL_EXSTYLE, GetWindowLong(handle, GWL_EXSTYLE) | WS_EX_LAYERED);

    SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (q));

#ifdef QT_4
    SetWindowLong(id, GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN);
#else
    q->setProperty("_q_embedded_native_parent_handle", (WId) handle);

    q->setWindowFlags(Qt::FramelessWindowHint);
#endif

    SetParent(id, handle);
#endif // SK_WIN_NATIVE
}

#ifdef SK_WIN_NATIVE

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WAbstractViewPrivate::applyFullScreen()
{
    Q_Q(WAbstractView);

    QRect geometry = qApp->desktop()->screenGeometry(q);

    q->setGeometry(geometry);
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
// Private events
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

        view->d_func()->method.invoke(view, Qt::QueuedConnection);

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

        MINMAXINFO * info = (MINMAXINFO *) lParam;

        info->ptMinTrackSize.x = view->minimumWidth ();
        info->ptMinTrackSize.y = view->minimumHeight();

        info->ptMaxTrackSize.x = view->maximumWidth ();
        info->ptMaxTrackSize.y = view->maximumHeight();

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

        view->QDeclarativeView::move(0, 0);

        return 0;
    }
    else if (message == WM_SIZE)
    {
        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        if (view == NULL) return 0;

        WAbstractViewPrivate * d = view->d_func();

        RECT rect;

        GetClientRect(handle, &rect);

        WINDOWPLACEMENT placement;

        placement.length = sizeof(WINDOWPLACEMENT);

        GetWindowPlacement(handle, &placement);

        if (placement.showCmd == SW_MAXIMIZE)
        {
            int border;
            int border2x;

            if (d->windowSnap)
            {
                border   = 8;
                border2x = 16;
            }
            else
            {
                border   = 3;
                border2x = 6;
            }

            int width  = rect.right  - border2x;
            int height = rect.bottom - border2x;

            d->width  = width;
            d->height = height;

            view->QDeclarativeView::setGeometry(border, border, width, height);

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

            view->QDeclarativeView::setGeometry(0, 0, width, height);

            if (d->maximized)
            {
                d->maximized = false;

                view->onStateChanged(Qt::WindowNoState);
            }
        }

        return 0;
    }
    else if (message == WM_CLOSE)
    {
        PostQuitMessage(0);

        return 0;
    }
    else return DefWindowProc(handle, message, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WAbstractViewPrivate::onFocus()
{
    SetFocus(id);
}

#endif // SK_WIN_NATIVE

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WAbstractView::WAbstractView(QWidget * parent, Qt::WindowFlags flags)
    : QDeclarativeView(parent), WPrivatable(new WAbstractViewPrivate(this))
{
    Q_D(WAbstractView); d->init(flags);
}

//-------------------------------------------------------------------------------------------------
// Protected

WAbstractView::WAbstractView(WAbstractViewPrivate * p, QWidget * parent, Qt::WindowFlags flags)
    : QDeclarativeView(parent), WPrivatable(p)
{
    Q_D(WAbstractView); d->init(flags);
}

#ifdef SK_WIN_NATIVE

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::showNormal()
{
    Q_D(WAbstractView);

    if (d->maximized)
    {
        d->maximized = false;

        ShowWindow(d->handle, SW_RESTORE);
    }
    else if (d->fullScreen)
    {
        d->fullScreen = false;

        setGeometry(d->rect);
    }
    else ShowWindow(d->handle, SW_SHOWNORMAL);
}

/* Q_INVOKABLE */ void WAbstractView::showMaximized()
{
    Q_D(WAbstractView);

    if (d->maximized) return;

    if (d->fullScreen)
    {
        d->fullScreen = false;

        setGeometry(d->rect);

        d->maximized = true;

        // FIXME Windows: Hide the window to avoid the animation.
        ShowWindow(d->handle, SW_HIDE);
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

        // FIXME Windows: Hide the window to avoid the animation.
        ShowWindow(d->handle, SW_HIDE);
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

/* Q_INVOKABLE */ void WAbstractView::move(int x, int y)
{
    Q_D(WAbstractView);

    SetWindowPos(d->handle, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
}

/* Q_INVOKABLE */ void WAbstractView::move(const QPoint & position)
{
    move(position.x(), position.y());
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

/* Q_INVOKABLE */ void WAbstractView::setFocus()
{
    Q_D(WAbstractView);

    SetFocus(d->id);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::setWindowIcon(const QIcon & icon)
{
    Q_D(WAbstractView);

    HICON iconSmall = WAbstractViewPrivate::getIcon(icon, GetSystemMetrics(SM_CXSMICON),
                                                          GetSystemMetrics(SM_CYSMICON));

    HICON iconBig = WAbstractViewPrivate::getIcon(icon, GetSystemMetrics(SM_CXICON),
                                                        GetSystemMetrics(SM_CYICON));

    if (iconBig)
    {
        SendMessage(d->handle, WM_SETICON, ICON_SMALL, (LPARAM) iconSmall);
        SendMessage(d->handle, WM_SETICON, ICON_BIG,   (LPARAM) iconBig);
    }
    else
    {
        SendMessage(d->handle, WM_SETICON, ICON_SMALL, (LPARAM) iconSmall);
        SendMessage(d->handle, WM_SETICON, ICON_BIG,   (LPARAM) iconSmall);
    }
}

/* Q_INVOKABLE */ void WAbstractView::setWindowTitle(const QString & title)
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

    if (enabled)
    {
         SetWindowLong(d->handle, GWL_STYLE,
                       GetWindowLong(d->handle, GWL_STYLE) | WS_THICKFRAME);
    }
    else SetWindowLong(d->handle, GWL_STYLE,
                       GetWindowLong(d->handle, GWL_STYLE) & ~WS_THICKFRAME);
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

    if (enabled)
    {
         SetWindowLong(d->handle, GWL_STYLE,
                       GetWindowLong(d->handle, GWL_STYLE) | WS_MAXIMIZEBOX);
    }
    else SetWindowLong(d->handle, GWL_STYLE,
                       GetWindowLong(d->handle, GWL_STYLE) & ~WS_MAXIMIZEBOX);
}
#else
/* Q_INVOKABLE */ void WAbstractView::setWindowMaximize(bool) {}
#endif

#endif // Q_OS_WIN
#ifdef SK_WIN_NATIVE

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractView::showEvent(QShowEvent * event)
{
    Q_D(WAbstractView);

    QDeclarativeView::showEvent(event);

    ShowWindow(d->handle, SW_SHOW);
}

/* virtual */ void WAbstractView::hideEvent(QHideEvent * event)
{
    Q_D(WAbstractView);

    QDeclarativeView::hideEvent(event);

    ShowWindow(d->handle, SW_HIDE);
}

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
    return QDeclarativeView::nativeEvent(event, msg, result);
#endif
}

//-------------------------------------------------------------------------------------------------
// Protected virtual functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractView::onStateChanged(Qt::WindowState) {}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WId WAbstractView::winId() const
{
    Q_D(const WAbstractView); return (WId) d->handle;
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

qreal WAbstractView::windowOpacity() const
{
    Q_D(const WAbstractView);

    return d->opacity;
}

void WAbstractView::setWindowOpacity(qreal level)
{
    Q_D(WAbstractView);

    d->opacity = level;

    SetLayeredWindowAttributes(d->handle, 0, level * 255, LWA_ALPHA);
}

#endif // SK_WIN_NATIVE

#endif // SK_NO_ABSTRACTVIEW
