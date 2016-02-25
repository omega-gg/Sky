//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WDECLARATIVECOREAPPLICATION_H
#define WDECLARATIVECOREAPPLICATION_H

// Qt includes
#include <QDeclarativeListProperty>

// Sk includes
#include <Sk>

#ifndef SK_NO_COREAPPLICATION

class WDeclarativeCoreApplicationPrivate;
class QDeclarativeItem;

class SK_CORE_EXPORT WDeclarativeCoreApplication : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeItem> children READ children)

    Q_CLASSINFO("DefaultProperty", "children")

public:
    WDeclarativeCoreApplication(QObject * object = 0);
protected:
    WDeclarativeCoreApplication(WDeclarativeCoreApplicationPrivate * p, QObject * object = 0);

public: // Properties
    QDeclarativeListProperty<QDeclarativeItem> children();

private: // Declarative
    static void childrenAppend(QDeclarativeListProperty<QDeclarativeItem> * property,
                               QDeclarativeItem * item);

    static void childrenClear(QDeclarativeListProperty<QDeclarativeItem> * property);

    static int childrenCount(QDeclarativeListProperty<QDeclarativeItem> * property);

    static QDeclarativeItem * childrenAt(QDeclarativeListProperty<QDeclarativeItem> * property,
                                         int index);

private:
    W_DECLARE_PRIVATE(WDeclarativeCoreApplication)
};

#endif // SK_NO_DECLARATIVECOREAPPLICATION
#endif // WDECLARATIVECOREAPPLICATION_H
