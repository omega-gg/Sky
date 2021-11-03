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

#ifndef WLISTID_H
#define WLISTID_H

// Qt includes
#include <QList>

// Sk includes
#include <Sk>

#ifndef SK_NO_LISTID

class SK_CORE_EXPORT WListId
{
public: // Interface
    int generateId(int id = 1);

    bool insertId(int id);

    bool removeOne(int id);

    void clear();

private: // Variables
    QList<int> ids;
};

#endif // SK_NO_LISTID
#endif // WLISTID_H
