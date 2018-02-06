//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WDeclarativeCoreApplication.h"

#ifndef SK_NO_DECLARATIVECOREAPPLICATION

// Qt includes
#ifdef QT_4
#include <QDeclarativeItem>
#else
#include <QQuickItem>
#endif

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WDeclarativeCoreApplication_p.h"

WDeclarativeCoreApplicationPrivate::
WDeclarativeCoreApplicationPrivate(WDeclarativeCoreApplication * p) : WPrivate(p) {}

/* virtual */ WDeclarativeCoreApplicationPrivate::~WDeclarativeCoreApplicationPrivate()
{
    deleteItems();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeCoreApplicationPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeCoreApplicationPrivate::deleteItems()
{
#ifdef QT_4
    foreach (QDeclarativeItem * item, items) delete item;
#else
    foreach (QQuickItem * item, items) delete item;
#endif

    items.clear();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WDeclarativeCoreApplication::WDeclarativeCoreApplication(QObject * object)
    : QObject(object), WPrivatable(new WDeclarativeCoreApplicationPrivate(this))
{
    Q_D(WDeclarativeCoreApplication); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WDeclarativeCoreApplication::WDeclarativeCoreApplication(WDeclarativeCoreApplicationPrivate * p,
                                                         QObject * object)
    : QObject(object), WPrivatable(p)
{
    Q_D(WDeclarativeCoreApplication); d->init();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
QDeclarativeListProperty<QDeclarativeItem> WDeclarativeCoreApplication::children()
#else
QQmlListProperty<QQuickItem> WDeclarativeCoreApplication::children()
#endif
{
#ifdef QT_4
    return QDeclarativeListProperty<QDeclarativeItem>(this, 0, childrenAppend, childrenCount,
                                                               childrenAt,     childrenClear);
#else
    return QQmlListProperty<QQuickItem>(this, 0, childrenAppend, childrenCount,
                                                 childrenAt,     childrenClear);
#endif
}

//-------------------------------------------------------------------------------------------------
// Declarative
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* static */ void WDeclarativeCoreApplication::childrenAppend(QDeclarativeListProperty
                                                              <QDeclarativeItem> * property,
                                                              QDeclarativeItem   * item)
#else
/* static */ void WDeclarativeCoreApplication::childrenAppend(QQmlListProperty
                                                              <QQuickItem> * property,
                                                              QQuickItem   * item)
#endif
{
    static_cast<WDeclarativeCoreApplication *> (property->object)->d_func()->items.append(item);
}

#ifdef QT_4
/* static */ void WDeclarativeCoreApplication::childrenClear(QDeclarativeListProperty
                                                             <QDeclarativeItem> * property)
#else
/* static */ void WDeclarativeCoreApplication::childrenClear(QQmlListProperty
                                                             <QQuickItem> * property)
#endif
{
    static_cast<WDeclarativeCoreApplication *> (property->object)->d_func()->deleteItems();
}

#ifdef QT_4
/* static */ int WDeclarativeCoreApplication::childrenCount(QDeclarativeListProperty
                                                            <QDeclarativeItem> * property)
#else
/* static */ int WDeclarativeCoreApplication::childrenCount(QQmlListProperty
                                                            <QQuickItem> * property)
#endif
{
    return static_cast<WDeclarativeCoreApplication *> (property->object)->d_func()->items.count();
}

#ifdef QT_4
/* static */
QDeclarativeItem * WDeclarativeCoreApplication::childrenAt(QDeclarativeListProperty
                                                           <QDeclarativeItem> * property,
                                                           int index)
#else
/* static */ QQuickItem * WDeclarativeCoreApplication::childrenAt(QQmlListProperty
                                                                  <QQuickItem> * property,
                                                                  int index)
#endif
{
    return
    static_cast<WDeclarativeCoreApplication *> (property->object)->d_func()->items.at(index);
}

#endif // SK_NO_DECLARATIVEMAIN
