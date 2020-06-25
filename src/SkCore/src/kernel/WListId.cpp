//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#include "WListId.h"

#ifndef SK_NO_LISTID

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

int WListId::generateId(int id)
{
    for (int i = 0; i < ids.count(); i++)
    {
        if (ids.at(i) > id)
        {
            ids.insert(i, id);

            return id;
        }

        id++;
    }

    ids.append(id);

    return id;
}

bool WListId::insertId(int id)
{
    for (int i = 0; i < ids.count(); i++)
    {
        int itemId = ids.at(i);

        if (itemId == id)
        {
            return false;
        }
        else if (itemId > id)
        {
            ids.insert(i, id);

            return true;
        }
    }

    ids.append(id);

    return true;
}

//-------------------------------------------------------------------------------------------------

bool WListId::removeOne(int id)
{
    return ids.removeOne(id);
}

void WListId::clear()
{
    ids.clear();
}

#endif // SK_NO_LISTID
