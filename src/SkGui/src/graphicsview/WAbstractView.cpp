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

#ifdef Q_OS_WIN
#include <dwmapi.h>
#endif

//-------------------------------------------------------------------------------------------------
// Static variables

#ifdef Q_OS_WIN
static const DWORD windowFlags = WS_OVERLAPPED | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
                                 WS_CLIPCHILDREN;
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

    q->setWindowFlags(flags);

#ifdef Q_OS_WIN
    width  = 0;
    height = 0;

    x = 0;
    y = 0;

    WNDCLASSEX wcx = { 0 };

    wcx.cbSize = sizeof( WNDCLASSEX );

    wcx.style = CS_HREDRAW | CS_VREDRAW;

    wcx.hInstance = instance;

    wcx.lpfnWndProc = events;

    wcx.cbClsExtra	= 0;
    wcx.cbWndExtra	= 0;

    wcx.lpszClassName = L"WindowClass";

    wcx.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

    wcx.hCursor = LoadCursor(instance, IDC_ARROW);

    RegisterClassEx(&wcx);

    handle = CreateWindow(L"WindowClass", L"WindowTitle", windowFlags, 0, 0, 0, 0, 0, 0, instance,
                          NULL);

    SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (q));

#ifdef QT_LATEST
    setProperty("_q_embedded_native_parent_handle", (WId) handle);
#endif

    SetParent((HWND) q->QDeclarativeView::winId(), handle);
#endif // Q_OS_WIN
}

#ifdef Q_OS_WIN
//-------------------------------------------------------------------------------------------------
// Private events
//-------------------------------------------------------------------------------------------------

/* static */ LRESULT CALLBACK WAbstractViewPrivate::events(HWND handle, UINT   message,
                                                                        WPARAM wParam,
                                                                        LPARAM lParam)
{
    if (message == WM_SETFOCUS)
    {
        SetFocus(handle);

        return 0;
    }
    else if (message == WM_NCCALCSIZE)
    {
        return 0;
    }
    else if (message == WM_GETMINMAXINFO)
    {
        return 0;
    }
    else if (message == WM_NCHITTEST)
    {
        return HTCAPTION;
    }
    else if (message == WM_SIZE)
    {
        RECT rect;

        GetClientRect(handle, &rect);

        WINDOWPLACEMENT placement;

        placement.length = sizeof(WINDOWPLACEMENT);

        GetWindowPlacement(handle, &placement);

        WAbstractView * view
            = reinterpret_cast<WAbstractView *> (GetWindowLongPtr(handle, GWLP_USERDATA));

        if (view == NULL) return 0;

        if (placement.showCmd == SW_MAXIMIZE)
        {
             view->QDeclarativeView::setGeometry(8, 8, rect.right - 16, rect.bottom - 16);
        }
        else view->QDeclarativeView::setGeometry(0, 0, rect.right, rect.bottom);

        return 0;
    }
    else if (message == WM_DESTROY)
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

/* Q_INVOKABLE */ void WAbstractView::setGeometry(int x, int y, int width, int height)
{
    Q_D(WAbstractView);

    d->width  = width;
    d->height = height;

    d->x = x;
    d->y = y;

    SetWindowPos(d->handle, HWND_TOP, x, y, width, height, 0);
}

/* Q_INVOKABLE */ void WAbstractView::setGeometry(const QRect & rect)
{
    setGeometry(rect.x(), rect.y(), rect.width(), rect.height());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::resize(int width, int height)
{
    Q_D(WAbstractView);

    d->width  = width;
    d->height = height;

    SetWindowPos(d->handle, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
}

/* Q_INVOKABLE */ void WAbstractView::resize(const QSize & size)
{
    resize(size.width(), size.height());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WAbstractView::move(int x, int y)
{
    Q_D(WAbstractView);

    d->x = x;
    d->y = y;

    SetWindowPos(d->handle, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
}

/* Q_INVOKABLE */ void WAbstractView::move(const QPoint & position)
{
    move(position.x(), position.y());
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WAbstractView::showEvent(QShowEvent * event)
{
    Q_D(WAbstractView);

    QDeclarativeView::showEvent(event);

    SetWindowPos(d->handle, HWND_TOP, d->x, d->y, d->width, d->height, SWP_SHOWWINDOW);
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

int WAbstractView::width() const
{
    Q_D(const WAbstractView); return d->width;
}

int WAbstractView::height() const
{
    Q_D(const WAbstractView); return d->height;
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

#endif // Q_OS_WIN

#endif // SK_NO_ABSTRACTVIEW
