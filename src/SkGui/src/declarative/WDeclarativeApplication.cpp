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
    QList<QObject *> items;

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
    foreach (QObject * item, items) delete item;

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
// Private declarative
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* static */
void WDeclarativeApplication::childrenAppend(QDeclarativeListProperty<QObject> * property,
                                             QObject                           * object)
#else
/* static */ void WDeclarativeApplication::childrenAppend(QQmlListProperty<QObject> * property,
                                                          QObject                   * object)
#endif
{
    static_cast<WDeclarativeApplication *> (property->object)->d_func()->items.append(object);
}

#ifdef QT_4
/* static */
void WDeclarativeApplication::childrenClear(QDeclarativeListProperty<QObject> * property)
#else
/* static */ void WDeclarativeApplication::childrenClear(QQmlListProperty<QObject> * property)
#endif
{
    static_cast<WDeclarativeApplication *> (property->object)->d_func()->deleteItems();
}

#ifdef QT_4
/* static */
int WDeclarativeApplication::childrenCount(QDeclarativeListProperty<QObject> * property)
#elif defined(QT_5)
/* static */ int WDeclarativeApplication::childrenCount(QQmlListProperty<QObject> * property)
#else // QT_6
/* static */ qsizetype WDeclarativeApplication::childrenCount(QQmlListProperty<QObject> * property)
#endif
{
    return static_cast<WDeclarativeApplication *> (property->object)->d_func()->items.count();
}

#ifdef QT_4
/* static */
QObject * WDeclarativeApplication::childrenAt(QDeclarativeListProperty<QObject> * property,
                                              int index)
#elif defined(QT_5)
/* static */ QObject * WDeclarativeApplication::childrenAt(QQmlListProperty<QObject> * property,
                                                           int index)
#else // QT_6
/* static */ QObject * WDeclarativeApplication::childrenAt(QQmlListProperty<QObject> * property,
                                                           qsizetype index)
#endif
{
    return static_cast<WDeclarativeApplication *> (property->object)->d_func()->items.at(index);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
QDeclarativeListProperty<QObject> WDeclarativeApplication::children()
#else
QQmlListProperty<QObject> WDeclarativeApplication::children()
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

#endif // SK_NO_DECLARATIVEAPPLICATION
