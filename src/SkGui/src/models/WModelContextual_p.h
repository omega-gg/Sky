//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WMODELCONTEXTUAL_P_H
#define WMODELCONTEXTUAL_P_H

#include <private/Sk_p>

#ifndef SK_NO_MODELCONTEXTUAL

// Forward declarations
class WModelContextualBase;

class WModelContextualPrivate : public WPrivate
{
public:
    WModelContextualPrivate(WModelContextual * p);

    /* virtual */ ~WModelContextualPrivate();

    void init();

public: // Properties
    WModelContextualBase * model;

    WDeclarativeContextualPage * currentPage;

    const WDeclarativeContextualItem * oldItem;

protected:
    W_DECLARE_PUBLIC(WModelContextual)
};

#endif // SK_NO_MODELCONTEXTUAL
#endif // WMODELCONTEXTUAL_P_H
