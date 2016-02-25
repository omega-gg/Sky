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

#include "WListId.h"

#ifndef SK_NO_LISTID

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

int WListId::generateId()
{
    int id = 1;

    for (int i = 0; i < count(); i++)
    {
        if (at(i) > id)
        {
            insert(i, id);

            return id;
        }

        id++;
    }

    append(id);

    return id;
}

bool WListId::insertId(int id)
{
    for (int i = 0; i < count(); i++)
    {
        int itemId = at(i);

        if (itemId == id)
        {
            return false;
        }
        else if (itemId > id)
        {
            insert(i, id);

            return true;
        }
    }

    append(id);

    return true;
}

#endif // SK_NO_LISTID
