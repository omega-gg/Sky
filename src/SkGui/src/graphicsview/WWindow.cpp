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

#include "WWindow.h"

#ifndef SK_NO_WINDOW

#if defined(Q_OS_ANDROID) && defined(QT_5)
// Qt includes
#include <QtAndroid>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WWindow_p.h"

WWindowPrivate::WWindowPrivate(WWindow * p) : WViewPrivate(p) {}

/* virtual */ WWindowPrivate::~WWindowPrivate()
{
    deleteItems();
}

//-------------------------------------------------------------------------------------------------

void WWindowPrivate::init()
{
    Q_Q(WWindow);

    viewport = static_cast<WDeclarativeMouseArea *> (item);

    icon = sk->icon();

    visible = true;

    viewport->setAcceptHoverEvents(true);

#ifdef QT_NEW
    viewport->setFlag(QQuickItem::ItemAcceptsDrops);
#endif

#ifdef QT_4
    q->setWindowTitle(sk->name());
#else
    q->setTitle(sk->name());
#endif

    if (icon.isEmpty() == false)
    {
#ifdef QT_4
        q->setWindowIcon(QIcon(icon));
#else
        q->WView::setIcon(QIcon(icon));
#endif
    }

    q->WView::setVisible(true);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WWindowPrivate::deleteItems()
{
    foreach (QObject * item, items) delete item;

    items.clear();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WWindow::WWindow(QWidget * parent)
#else
/* explicit */ WWindow::WWindow(QWindow * parent)
#endif
#ifdef SK_WIN_NATIVE
    : WView(new WWindowPrivate(this), new WDeclarativeMouseArea, parent)
#elif defined(Q_OS_WIN)
    : WView(new WWindowPrivate(this), new WDeclarativeMouseArea, parent,
            Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
#else
    : WView(new WWindowPrivate(this), new WDeclarativeMouseArea, parent, Qt::FramelessWindowHint)
#endif
{
    Q_D(WWindow); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------
// Focus

/* Q_INVOKABLE */ bool WWindow::getFocus() const
{
#ifdef QT_4
    Q_D(const WWindow);

    return (d->scene->focusItem() != NULL);
#else
    return (focusObject() != NULL);
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WWindow::setFocus(bool focus)
{
    Q_D(WWindow); d->viewport->setFocus(focus);
}

/* Q_INVOKABLE */ void WWindow::clearFocus()
{
    setFocus(true);
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* Q_INVOKABLE */ void WWindow::clearFocusItem(QDeclarativeItem * item)
#else
/* Q_INVOKABLE */ void WWindow::clearFocusItem(QQuickItem * item)
#endif
{
    Q_ASSERT(item);

#ifdef QT_4
    if (item->focusItem())
    {
        setFocus(true);
    }
#else
    QQuickItem * parent = activeFocusItem();

    while (parent)
    {
        parent = parent->parentItem();

        if (parent == item)
        {
            setFocus(true);

            return;
        }
    }
#endif
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

#if defined(Q_OS_ANDROID) && defined(QT_NEW)

/* Q_INVOKABLE static */ void WWindow::hideSplash(int duration)
{
#ifdef QT_5
    QtAndroid::hideSplashScreen(duration);
#else
    QNativeInterface::QAndroidApplication::hideSplashScreen(duration);
#endif
}

#endif

//-------------------------------------------------------------------------------------------------
// Protected events
//-------------------------------------------------------------------------------------------------

//#ifdef QT_4

///* virtual */ void WWindow::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
//{
//    Q_D(WWindow);

//    d->view->hoverEnter();

//    WDeclarativeMouseArea::hoverEnterEvent(event);
//}

///* virtual */ void WWindow::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)

//{
//    Q_D(WWindow);

//    d->view->hoverLeave();

//    WDeclarativeMouseArea::hoverLeaveEvent(event);
//}

//#endif

//-------------------------------------------------------------------------------------------------

//#ifdef QT_NEW

///* virtual */ void WWindow::dragEnterEvent(QDragEnterEvent * event)
//{
//    Q_D(WWindow); d->view->dragEnterEvent(event);
//}

///* virtual */ void WWindow::dragLeaveEvent(QDragLeaveEvent * event)
//{
//    Q_D(WWindow); d->view->dragLeaveEvent(event);
//}

///* virtual */ void WWindow::dragMoveEvent(QDragMoveEvent * event)
//{
//    Q_D(WWindow); d->view->dragMoveEvent(event);
//}

////-------------------------------------------------------------------------------------------------

///* virtual */ void WWindow::dropEvent(QDropEvent * event)
//{
//    Q_D(WWindow); d->view->dropEvent(event);
//}

//#endif

//-------------------------------------------------------------------------------------------------
// Private declarative
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* static */ void WWindow::childrenAppend(QDeclarativeListProperty<QObject> * property,
                                          QObject                           * object)
#else
/* static */ void WWindow::childrenAppend(QQmlListProperty<QObject> * property,
                                          QObject                   * object)
#endif
{
    WWindowPrivate * p = static_cast<WWindow *> (property->object)->d_func();

#ifdef QT_4
    QDeclarativeItem * item = qobject_cast<QDeclarativeItem *> (object);
#else
    QQuickItem * item = qobject_cast<QQuickItem *> (object);
#endif

    if (item) item->setParentItem(p->viewport);

    p->items.append(object);
}

#ifdef QT_4
/* static */ void WWindow::childrenClear(QDeclarativeListProperty<QObject> * property)
#else
/* static */ void WWindow::childrenClear(QQmlListProperty<QObject> * property)
#endif
{
    static_cast<WWindow *> (property->object)->d_func()->deleteItems();
}

#ifdef QT_4
/* static */ int WWindow::childrenCount(QDeclarativeListProperty<QObject> * property)
#elif defined(QT_5)
/* static */ int WWindow::childrenCount(QQmlListProperty<QObject> * property)
#else // QT_6
/* static */ qsizetype WWindow::childrenCount(QQmlListProperty<QObject> * property)
#endif
{
    return static_cast<WWindow *> (property->object)->d_func()->items.count();
}

#ifdef QT_4
/* static */ QObject * WWindow::childrenAt(QDeclarativeListProperty<QObject> * property, int index)
#elif defined(QT_5)
/* static */ QObject * WWindow::childrenAt(QQmlListProperty<QObject> * property, int index)
#else // QT_6
/* static */ QQuickItem * WWindow::childrenAt(QQmlListProperty<QObject> * property,
                                              qsizetype index)
#endif
{
    return static_cast<WWindow *> (property->object)->d_func()->items.at(index);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
QDeclarativeListProperty<QObject> WWindow::children()
#else
QQmlListProperty<QObject> WWindow::children()
#endif
{
#ifdef QT_4
    return QDeclarativeListProperty<QObject>(this, 0, childrenAppend, childrenCount, childrenAt,
                                             childrenClear);
#else
    return QQmlListProperty<QObject>(this, 0, childrenAppend, childrenCount, childrenAt,
                                     childrenClear);
#endif
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

#ifdef QT_4
    setWindowIcon(QIcon(icon));
#else
    WView::setIcon(QIcon(icon));
#endif

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

    WView::setVisible(visible);

    emit visibleChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WWindow::opacity() const
{
#ifdef QT_4
    return windowOpacity();
#else
    return WView::opacity();
#endif
}

void WWindow::setOpacity(qreal opacity)
{
#ifdef QT_4
    if (windowOpacity() == opacity) return;

    setWindowOpacity(opacity);
#else
    if (WView::opacity() == opacity) return;

    WView::setOpacity(opacity);
#endif

    emit opacityChanged();
}

//-------------------------------------------------------------------------------------------------

void WWindow::setLocked(bool locked)
{
    Q_D(WWindow);

    WView::setLocked(locked);

    bool visible = WView::isVisible();

    if (d->visible != visible)
    {
        d->visible = visible;

        emit visibleChanged();
    }
}

#endif // SK_NO_WINDOW
