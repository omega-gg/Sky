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

#ifndef WDECLARATIVEAPPLICATION_H
#define WDECLARATIVEAPPLICATION_H

// Qt includes
#ifdef QT_4
#include <QDeclarativeListProperty>
#else
#include <QQmlListProperty>
#endif

// Sk includes
#include <Sk>

#ifndef SK_NO_DECLARATIVEAPPLICATION

// Forward declarations
#ifdef QT_4
class QDeclarativeItem;
#else
class QQuickItem;
#endif
class WDeclarativeApplicationPrivate;

class SK_GUI_EXPORT WDeclarativeApplication : public QObject, public WPrivatable
{
    Q_OBJECT

#ifdef QT_4
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeItem> children READ children)
#else
    Q_PROPERTY(QQmlListProperty<QQuickItem> children READ children)
#endif

    Q_CLASSINFO("DefaultProperty", "children")

public:
    explicit WDeclarativeApplication(QObject * parent = NULL);

private: // Declarative
#ifdef QT_4
    static void childrenAppend(QDeclarativeListProperty<QDeclarativeItem> * property,
                               QDeclarativeItem * item);

    static void childrenClear(QDeclarativeListProperty<QDeclarativeItem> * property);

    static int childrenCount(QDeclarativeListProperty<QDeclarativeItem> * property);

    static QDeclarativeItem * childrenAt(QDeclarativeListProperty<QDeclarativeItem> * property,
                                         int index);
#else
    static void childrenAppend(QQmlListProperty<QQuickItem> * property, QQuickItem * item);
    static void childrenClear (QQmlListProperty<QQuickItem> * property);

    static int childrenCount(QQmlListProperty<QQuickItem> * property);

    static QQuickItem * childrenAt(QQmlListProperty<QQuickItem> * property, int index);
#endif

public: // Properties
#ifdef QT_4
    QDeclarativeListProperty<QDeclarativeItem> children();
#else
    QQmlListProperty<QQuickItem> children();
#endif

private:
    W_DECLARE_PRIVATE(WDeclarativeApplication)
};

#endif // SK_NO_DECLARATIVEAPPLICATION
#endif // WDECLARATIVEAPPLICATION_H
