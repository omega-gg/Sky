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

#include "WResizer.h"

#ifndef SK_NO_RESIZER

// Qt includes
#include <QCursor>

// Sk includes
#include <WView>

#ifdef Q_OS_WIN
// Windows includes
#include <qt_windows.h>
#endif

#ifdef Q_OS_LINUX
// Linux includes
#include <QX11Info>
#endif

// Private includes
#include <private/WView_p>
#if defined(QT_4) && defined(Q_OS_LINUX)
#include <private/qt_x11_p.h>
#endif

//-------------------------------------------------------------------------------------------------
// Defines

#ifdef Q_OS_WIN

#define SZ_SIZETOPLEFT      0xf004
#define SZ_SIZETOPRIGHT     0xf005
#define SZ_SIZEBOTTOMLEFT   0xf007
#define SZ_SIZEBOTTOMRIGHT  0xf008
#define SZ_SIZELEFT         0xf001
#define SZ_SIZERIGHT        0xf002
#define SZ_SIZETOP          0xf003
#define SZ_SIZEBOTTOM       0xf006

#elif defined(QT_4) && defined(Q_OS_LINUX)

bool isSupportedByWM(Atom atom)
{
    if (X11->net_supported_list == NULL) return false;

    for (int i = 0; X11->net_supported_list[i] != 0; i++)
    {
        if (X11->net_supported_list[i] == atom)
        {
            return true;
        }
    }

    return false;
}

#endif

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WResizer_p.h"

WResizerPrivate::WResizerPrivate(WResizer * p) : WDeclarativeMouseAreaPrivate(p) {}

void WResizerPrivate::init(WResizer::ResizeType type)
{
    Q_Q(WResizer);

    this->type = type;

    hoverEnabled = true;
    hoverRetain  = true;

    resizing = false;

    q->setAcceptHoverEvents(true);

    Qt::CursorShape shape;

    if      (type == WResizer::TopLeft)      shape = Qt::SizeFDiagCursor;
    else if (type == WResizer::TopRight)     shape = Qt::SizeBDiagCursor;
    else if (type == WResizer::BottomLeft)   shape = Qt::SizeBDiagCursor;
    else if (type == WResizer::BottomRight)  shape = Qt::SizeFDiagCursor;
    else if (type == WResizer::Left)         shape = Qt::SizeHorCursor;
    else if (type == WResizer::Right)        shape = Qt::SizeHorCursor;
    else if (type == WResizer::Top)          shape = Qt::SizeVerCursor;
    else /* if (type == WResizer::Bottom) */ shape = Qt::SizeVerCursor;

    q->setCursor(shape);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
void WResizerPrivate::startResize(QGraphicsSceneMouseEvent * event)
#else
void WResizerPrivate::startResize(QMouseEvent * event)
#endif
{
    QRect availableGeometry = view->availableGeometry();

#ifdef QT_4
    cursorPos = event->pos().toPoint();
#else
    cursorPos = event->pos();
#endif

    if (type == WResizer::TopLeft)
    {
        resizeArea = QRect(availableGeometry.x(), availableGeometry.y(),
                           view->x() + view->width()  - view->minimumWidth(),
                           view->y() + view->height() - view->minimumHeight());
    }
    else if (type == WResizer::TopRight)
    {
        resizeArea = QRect(availableGeometry.x(), availableGeometry.y(),
                           availableGeometry.x() + availableGeometry.width(),
                           view->y() + view->height() - view->minimumHeight());
    }
    else if (type == WResizer::BottomLeft)
    {
        resizeArea = QRect(availableGeometry.x(), availableGeometry.y(),
                           view->x() + view->width() - view->minimumWidth(),
                           availableGeometry.y() + availableGeometry.height());
    }
    else if (type == WResizer::BottomRight)
    {
        resizeArea = QRect(availableGeometry.x(), availableGeometry.y(),
                           availableGeometry.x() + availableGeometry.width(),
                           availableGeometry.y() + availableGeometry.height());
    }
    else if (type == WResizer::Left)
    {
        resizeArea = QRect(availableGeometry.x(), availableGeometry.y(),
                           view->x() + view->width() - view->minimumWidth(),
                           availableGeometry.y() + availableGeometry.height());
    }
    else if (type == WResizer::Right)
    {
        resizeArea = QRect(availableGeometry.x(), availableGeometry.y(),
                           availableGeometry.x() + availableGeometry.width(),
                           availableGeometry.y() + availableGeometry.height());
    }
    else if (type == WResizer::Top)
    {
        resizeArea = QRect(availableGeometry.x(), availableGeometry.y(),
                           availableGeometry.x() + availableGeometry.width(),
                           view->y() + view->height() - view->minimumHeight());
    }
    else if (type == WResizer::Bottom)
    {
        resizeArea = QRect(availableGeometry.x(), availableGeometry.y(),
                           availableGeometry.x() + availableGeometry.width(),
                           availableGeometry.y() + availableGeometry.height());
    }

    resizing = true;

    lastPos = QCursor::pos();
}

void WResizerPrivate::resize()
{
    Q_Q(WResizer);

    QPoint currentPos = QCursor::pos();

    int diffX = currentPos.x() - lastPos.x();
    int diffY = currentPos.y() - lastPos.y();

    if (diffX == 0 && diffY == 0) return;

    QRect geometry = view->geometry();

    int x = geometry.x();
    int y = geometry.y();

    int width  = geometry.width();
    int height = geometry.height();

    QPoint pos;

    pos = view->mapFromGlobal(currentPos);

    int posX = pos.x() - diffX;
    int posY = pos.y() - diffY;

    if (type == WResizer::Left || type == WResizer::TopLeft || type == WResizer::BottomLeft)
    {
        if (posX > cursorPos.x()
            ||
            (geometry.x() + posX - cursorPos.x()) < resizeArea.x())
        {
            currentPos.setX(geometry.x() + cursorPos.x());
        }
        else
        {
            x += diffX;

            width -= diffX;

            if (x < resizeArea.x())
            {
                x = resizeArea.x();

                width = geometry.width() + geometry.x() - resizeArea.x();
            }

            if (x > resizeArea.width())
            {
                x = resizeArea.width();

                width = view->minimumWidth();
            }
        }
    }
    if (type == WResizer::Right || type == WResizer::TopRight || type == WResizer::BottomRight)
    {
        if (posX < (geometry.width() - (q->width() - cursorPos.x()))
            ||
            (geometry.x() + posX + (q->width() - cursorPos.x())) > resizeArea.width())
        {
            currentPos.setX(geometry.x() + geometry.width() - (q->width() - cursorPos.x()));
        }
        else
        {
            width += diffX;

            if (width < view->minimumWidth())
            {
                width = view->minimumWidth();
            }

            if ((x + width) > resizeArea.width())
            {
                x = geometry.x();

                width = resizeArea.width() - geometry.x();
            }
        }
    }
    if (type == WResizer::Top || type == WResizer::TopLeft || type == WResizer::TopRight)
    {
        if (posY > cursorPos.y()
            ||
            (geometry.y() + posY - cursorPos.y()) < resizeArea.y())
        {
            currentPos.setY(geometry.y() + cursorPos.y());
        }
        else
        {
            y += diffY;

            height -= diffY;

            if (y < resizeArea.y())
            {
                y = resizeArea.y();

                height = geometry.height() + geometry.y() - resizeArea.y();
            }

            if (y > resizeArea.height())
            {
                y = resizeArea.height();

                height = view->minimumHeight();
            }
        }
    }
    if (type == WResizer::Bottom || type == WResizer::BottomLeft || type == WResizer::BottomRight)
    {
        if (posY < (geometry.height() - (q->height() - cursorPos.y()))
            ||
            (geometry.y() + posY + (q->height() - cursorPos.y())) > resizeArea.height())
        {
            currentPos.setY(geometry.y() + geometry.height() - (q->height() - cursorPos.y()));
        }
        else
        {
            height += diffY;

            if (height < view->minimumHeight())
            {
                height = view->minimumHeight();
            }

            if ((y + height) > resizeArea.height())
            {
                y = geometry.y();

                height = resizeArea.height() - geometry.y();
            }
        }
    }

    view->setGeometry(x, y, width, height);

    lastPos = currentPos;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
WResizer::WResizer(ResizeType type, QDeclarativeItem * parent)
#else
WResizer::WResizer(ResizeType type, QQuickItem * parent)
#endif
    : WDeclarativeMouseArea(new WResizerPrivate(this), parent)
{
    Q_D(WResizer); d->init(type);
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WResizer::mousePressEvent(QGraphicsSceneMouseEvent * event)
#else
/* virtual */ void WResizer::mousePressEvent(QMouseEvent * event)
#endif
{
    Q_D(WResizer);

    d->view->d_func()->currentResizer = this;

    d->view->d_func()->setResizing(true);

#if defined(Q_OS_WIN)
    Q_UNUSED(event);

    uint orientation;

    if      (d->type == TopLeft)     orientation = SZ_SIZETOPLEFT;
    else if (d->type == TopRight)    orientation = SZ_SIZETOPRIGHT;
    else if (d->type == BottomLeft)  orientation = SZ_SIZEBOTTOMLEFT;
    else if (d->type == BottomRight) orientation = SZ_SIZEBOTTOMRIGHT;
    else if (d->type == Left)        orientation = SZ_SIZELEFT;
    else if (d->type == Right)       orientation = SZ_SIZERIGHT;
    else if (d->type == Top)         orientation = SZ_SIZETOP;
    else if (d->type == Bottom)      orientation = SZ_SIZEBOTTOM;
    else                             orientation = 0;

    ReleaseCapture();

    PostMessage((HWND) d->view->winId(), WM_SYSCOMMAND, orientation, 0);

    return;
#elif defined(QT_4) && defined(Q_OS_LINUX)
    if (isSupportedByWM(ATOM(_NET_WM_MOVERESIZE)))
    {
        XEvent xev;

        Display * display = X11->display;

        xev.xclient.type         = ClientMessage;
        xev.xclient.message_type = ATOM(_NET_WM_MOVERESIZE);
        xev.xclient.display      = display;
        xev.xclient.window       = d->view->winId();
        xev.xclient.format       = 32;
        xev.xclient.data.l[0]    = d->view->x();
        xev.xclient.data.l[1]    = d->view->y();

        if      (d->type == TopLeft)     xev.xclient.data.l[2] = 0;
        else if (d->type == TopRight)    xev.xclient.data.l[2] = 2;
        else if (d->type == BottomLeft)  xev.xclient.data.l[2] = 6;
        else if (d->type == BottomRight) xev.xclient.data.l[2] = 4;
        else if (d->type == Left)        xev.xclient.data.l[2] = 7;
        else if (d->type == Right)       xev.xclient.data.l[2] = 3;
        else if (d->type == Top)         xev.xclient.data.l[2] = 1;
        else if (d->type == Bottom)      xev.xclient.data.l[2] = 5;

        xev.xclient.data.l[3] = Button1;
        xev.xclient.data.l[4] = 0;

        XUngrabPointer(display, X11->time);

        XSendEvent(display, QX11Info::appRootWindow(d->view->x11Info().screen()), false,
                   SubstructureRedirectMask | SubstructureNotifyMask, &xev);

        return;
    }
#endif

    d->pressed = true;

    d->startResize(event);
}

#ifdef QT_4
/* virtual */ void WResizer::mouseMoveEvent(QGraphicsSceneMouseEvent *)
#else
/* virtual */ void WResizer::mouseMoveEvent(QMouseEvent *)
#endif
{
    Q_D(WResizer);

#if defined(Q_OS_WIN)
    if (GetSystemMenu((HWND) d->view->winId(), FALSE) != 0)
    {
        MSG msg;

        while (PeekMessage(&msg, (HWND) d->view->winId(), WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));

        return;
    }
#elif defined(QT_4) && defined(Q_OS_LINUX)
    if (isSupportedByWM(ATOM(_NET_WM_MOVERESIZE))) return;
#endif

    if (d->resizing) d->resize();
}

#endif // SK_NO_RESIZER
