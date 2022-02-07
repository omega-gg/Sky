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
class WDeclarativeApplicationPrivate;

class SK_GUI_EXPORT WDeclarativeApplication : public QObject, public WPrivatable
{
    Q_OBJECT

#ifdef QT_4
    Q_PROPERTY(QDeclarativeListProperty<QObject> children READ children)
#else
    Q_PROPERTY(QQmlListProperty<QObject> children READ children)
#endif

    Q_CLASSINFO("DefaultProperty", "children")

public:
    explicit WDeclarativeApplication(QObject * parent = NULL);

private: // Declarative
#ifdef QT_4
    static void childrenAppend(QDeclarativeListProperty<QObject> * property, QObject * object);
    static void childrenClear (QDeclarativeListProperty<QObject> * property);

    static int childrenCount(QDeclarativeListProperty<QObject> * property);

    static QObject * childrenAt(QDeclarativeListProperty<QObject> * property, int index);
#else
    static void childrenAppend(QQmlListProperty<QObject> * property, QObject * object);
    static void childrenClear (QQmlListProperty<QObject> * property);

#ifdef QT_5
    static int childrenCount(QQmlListProperty<QObject> * property);

    static QObject * childrenAt(QQmlListProperty<QObject> * property, int index);
#else // QT_6
    static qsizetype childrenCount(QQmlListProperty<QObject> * property);

    static QObject * childrenAt(QQmlListProperty<QObject> * property, qsizetype index);
#endif
#endif

public: // Properties
#ifdef QT_4
    QDeclarativeListProperty<QObject> children();
#else
    QQmlListProperty<QObject> children();
#endif

private:
    W_DECLARE_PRIVATE(WDeclarativeApplication)
};

#endif // SK_NO_DECLARATIVEAPPLICATION
#endif // WDECLARATIVEAPPLICATION_H
