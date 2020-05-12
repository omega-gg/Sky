//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WLISTID_H
#define WLISTID_H

// Qt includes
#include <QList>

// Sk includes
#include <Sk>

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

#endif // WLISTID_H
