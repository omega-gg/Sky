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

#ifndef WDECLARATIVECOREAPPLICATION_P_H
#define WDECLARATIVECOREAPPLICATION_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/Sk_p>

#ifndef SK_NO_DECLARATIVECOREAPPLICATION

class SK_CORE_EXPORT WDeclarativeCoreApplicationPrivate : public WPrivate
{
public:
    WDeclarativeCoreApplicationPrivate(WDeclarativeCoreApplication * p);

    /* virtual */ ~WDeclarativeCoreApplicationPrivate();

    void init();

public: // Functions
    void deleteItems();

public: // Variables
    QList<QDeclarativeItem *> items;

    QString plugins;

protected:
    W_DECLARE_PUBLIC(WDeclarativeCoreApplication)
};

#endif // SK_NO_DECLARATIVECOREAPPLICATION
#endif // WDECLARATIVECOREAPPLICATION_P_H
