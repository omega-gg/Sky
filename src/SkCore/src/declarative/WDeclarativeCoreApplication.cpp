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
#include <QDeclarativeItem>

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
    foreach (QDeclarativeItem * item, items) delete item;

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

QDeclarativeListProperty<QDeclarativeItem> WDeclarativeCoreApplication::children()
{
    return QDeclarativeListProperty<QDeclarativeItem>(this, 0, childrenAppend, childrenCount,
                                                               childrenAt,     childrenClear);
}

//-------------------------------------------------------------------------------------------------
// Declarative
//-------------------------------------------------------------------------------------------------

/* static */
void WDeclarativeCoreApplication::childrenAppend(QDeclarativeListProperty
                                                 <QDeclarativeItem> * property,
                                                 QDeclarativeItem * item)
{
    static_cast<WDeclarativeCoreApplication *> (property->object)->d_func()->items.append(item);
}

/* static */
void WDeclarativeCoreApplication::childrenClear(QDeclarativeListProperty
                                                <QDeclarativeItem> * property)
{
    static_cast<WDeclarativeCoreApplication *> (property->object)->d_func()->deleteItems();
}

/* static */
int WDeclarativeCoreApplication::childrenCount(QDeclarativeListProperty
                                               <QDeclarativeItem> * property)
{
    return static_cast<WDeclarativeCoreApplication *> (property->object)->d_func()->items.count();
}

/* static */
QDeclarativeItem * WDeclarativeCoreApplication::childrenAt(QDeclarativeListProperty
                                                           <QDeclarativeItem> * property,
                                                           int index)
{
    return
    static_cast<WDeclarativeCoreApplication *> (property->object)->d_func()->items.at(index);
}

#endif // SK_NO_DECLARATIVEMAIN
