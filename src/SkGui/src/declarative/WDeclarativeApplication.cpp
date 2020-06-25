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

#include "WDeclarativeApplication.h"

#ifndef SK_NO_DECLARATIVEAPPLICATION

// Qt includes
#ifdef QT_4
#include <QDeclarativeItem>
#else
#include <QQuickItem>
#endif

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/Sk_p>

class SK_GUI_EXPORT WDeclarativeApplicationPrivate : public WPrivate
{
public:
    WDeclarativeApplicationPrivate(WDeclarativeApplication * p);

    /* virtual */ ~WDeclarativeApplicationPrivate();

    void init();

public: // Functions
    void deleteItems();

public: // Variables
#ifdef QT_4
    QList<QDeclarativeItem *> items;
#else
    QList<QQuickItem *> items;
#endif

protected:
    W_DECLARE_PUBLIC(WDeclarativeApplication)
};

//-------------------------------------------------------------------------------------------------

WDeclarativeApplicationPrivate::WDeclarativeApplicationPrivate(WDeclarativeApplication * p)
    : WPrivate(p) {}


/* virtual */ WDeclarativeApplicationPrivate::~WDeclarativeApplicationPrivate()
{
    deleteItems();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeApplicationPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeApplicationPrivate::deleteItems()
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

/* explicit */ WDeclarativeApplication::WDeclarativeApplication(QObject * parent)
    : QObject(parent), WPrivatable(new WDeclarativeApplicationPrivate(this))
{
    Q_D(WDeclarativeApplication); d->init();
}

//-------------------------------------------------------------------------------------------------
// Declarative
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* static */ void WDeclarativeApplication::childrenAppend(QDeclarativeListProperty
                                                          <QDeclarativeItem> * property,
                                                          QDeclarativeItem   * item)
#else
/* static */ void WDeclarativeApplication::childrenAppend(QQmlListProperty
                                                          <QQuickItem> * property,
                                                          QQuickItem   * item)
#endif
{
    static_cast<WDeclarativeApplication *> (property->object)->d_func()->items.append(item);
}

#ifdef QT_4
/* static */ void WDeclarativeApplication::childrenClear(QDeclarativeListProperty
                                                         <QDeclarativeItem> * property)
#else
/* static */ void WDeclarativeApplication::childrenClear(QQmlListProperty
                                                         <QQuickItem> * property)
#endif
{
    static_cast<WDeclarativeApplication *> (property->object)->d_func()->deleteItems();
}

#ifdef QT_4
/* static */ int WDeclarativeApplication::childrenCount(QDeclarativeListProperty
                                                        <QDeclarativeItem> * property)
#else
/* static */ int WDeclarativeApplication::childrenCount(QQmlListProperty
                                                        <QQuickItem> * property)
#endif
{
    return static_cast<WDeclarativeApplication *> (property->object)->d_func()->items.count();
}

#ifdef QT_4
/* static */
QDeclarativeItem * WDeclarativeApplication::childrenAt(QDeclarativeListProperty
                                                       <QDeclarativeItem> * property, int index)
#else
/* static */ QQuickItem * WDeclarativeApplication::childrenAt(QQmlListProperty
                                                              <QQuickItem> * property, int index)
#endif
{
    return
    static_cast<WDeclarativeApplication *> (property->object)->d_func()->items.at(index);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
QDeclarativeListProperty<QDeclarativeItem> WDeclarativeApplication::children()
#else
QQmlListProperty<QQuickItem> WDeclarativeApplication::children()
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

#endif // SK_NO_DECLARATIVEAPPLICATION
