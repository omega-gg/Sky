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
#ifdef Q_OS_WIN
#include <QCoreApplication>
#include <QFocusEvent>
#include <QIcon>
#endif

#ifdef Q_OS_WIN
#include <dwmapi.h>
#endif

//-------------------------------------------------------------------------------------------------
// Static variables

#ifdef Q_OS_WIN
static const DWORD windowFlags = WS_OVERLAPPED | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
#endif

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WAbstractView_p.h"

WAbstractViewPrivate::WAbstractViewPrivate(WAbstractView * p) : WPrivate(p) {}

#ifdef Q_OS_WIN
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

#ifndef Q_OS_WIN
    q->setWindowFlags(flags);
#else
    id = q->QDeclarativeView::winId();

    x = 0;
    y = 0;

    width  = 0;
    height = 0;

    opacity = 0.0;

    WNDCLASSEX wcx;

    wcx.cbSize = sizeof(WNDCLASSEX);

    wcx.style = CS_HREDRAW | CS_VREDRAW;

    wcx.lpfnWndProc = events;

    wcx.cbClsExtra	= 0;
    wcx.cbWndExtra	= 0;

    wcx.hInstance = instance;

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

    handle = CreateWindow(L"Window", 0, windowFlags, 0, 0, 0, 0, 0, 0, instance, NULL);

    SetWindowLong(handle, GWL_EXSTYLE, GetWindowLong(handle, GWL_EXSTYLE) | WS_EX_LAYERED);

    SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (q));

#ifdef QT_LATEST
    setProperty("_q_embedded_native_parent_handle", (WId) handle);
#endif

    SetWindowLong(id, GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN);

    SetParent(id, handle);
#endif // Q_OS_WIN
}

#ifdef Q_OS_WIN

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
    else return pixmap.toWinHICON();
}

//-------------------------------------------------------------------------------------------------
// Private events
//-------------------------------------------------------------------------------------------------

/* static */ LRESULT CALLBACK WAbstractViewPrivate::events(HWND handle, UINT   message,
                                                                        WPARAM wParam,
                                                                        LPARAM lParam)
{
    if (message == WM_SETFOCUS)
    {
        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        QFocusEvent event(QEvent::FocusIn);

        QCoreApplication::sendEvent(view, &event);

        return 0;
    }
    else if (message == WM_KILLFOCUS)
    {
        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        QFocusEvent event(QEvent::FocusOut);

        QCoreApplication::sendEvent(view, &event);

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

        RECT rect;

        GetWindowRect(handle, &rect);

        view->d_func()->x = rect.left;
        view->d_func()->y = rect.top;

        return 0;
    }
    else if (message == WM_SIZE)
    {
        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        if (view == NULL) return 0;

        RECT rect;

        GetClientRect(handle, &rect);

        WINDOWPLACEMENT placement;

        placement.length = sizeof(WINDOWPLACEMENT);

        GetWindowPlacement(handle, &placement);

        int width;
        int height;

        if (placement.showCmd == SW_MAXIMIZE)
        {
            width  = rect.right  - 16;
            height = rect.bottom - 16;

            view->d_func()->width  = width;
            view->d_func()->height = height;

            view->QDeclarativeView::setGeometry(8, 8, width, height);
        }
        else
        {
            width  = rect.right;
            height = rect.bottom;

            view->d_func()->width  = width;
            view->d_func()->height = height;

            view->QDeclarativeView::setGeometry(0, 0, width, height);
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

#endif // Q_OS_WIN

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

#ifdef Q_OS_WIN
//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::showNormal()
{
    Q_D(WAbstractView);

    ShowWindow(d->handle, SW_SHOWNORMAL);
}

/* Q_INVOKABLE */ void WAbstractView::showMaximized()
{
    Q_D(WAbstractView);

    ShowWindow(d->handle, SW_SHOWMAXIMIZED);
}

/* Q_INVOKABLE */ void WAbstractView::showFullScreen()
{
    Q_D(WAbstractView);

    ShowWindow(d->handle, SHOW_FULLSCREEN);
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

#endif // Q_OS_WIN

#endif // SK_NO_ABSTRACTVIEW
