//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WLIBRARYITEM_P_H
#define WLIBRARYITEM_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/WLocalObject_p>

#ifndef SK_NO_LIBRARYITEM

class SK_GUI_EXPORT WLibraryItemPrivate : public WLocalObjectPrivate
{
public:
    WLibraryItemPrivate(WLibraryItem * p);

    /* virtual */ ~WLibraryItemPrivate();

    void init(WLibraryItem::Type type);

public: // Functions
    void setStateQuery(WLocalObject::State state);

    void setQueryLoading(bool loading);

    void setQueryDefault();
    void setQueryLoaded ();
    void setQueryEnded  ();

public: // Variables
    QList<int> idFull;

    WLibraryFolder * parentFolder;

    WLibraryItem::Type type;

    WLocalObject::State stateQuery;

    QString source;

    QString title;
    QString cover;

    QString label;

    QList<int> futureIds;

protected:
    W_DECLARE_PUBLIC(WLibraryItem)
};

#endif // SK_NO_LIBRARYITEM
#endif // WLIBRARYITEM_P_H
