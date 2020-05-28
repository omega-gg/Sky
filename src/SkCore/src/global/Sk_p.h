//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

#ifndef SK_P_H_
#define SK_P_H_

#include <Sk>

//-------------------------------------------------------------------------------------------------
// Defines

#define W_DECLARE_PUBLIC(Class)               \
                                              \
inline Class * q_func()                       \
{                                             \
    return static_cast<Class *> (sk_q);       \
}                                             \
                                              \
inline const Class * q_func() const           \
{                                             \
    return static_cast<const Class *> (sk_q); \
}                                             \
                                              \
friend class Class;                           \

//-------------------------------------------------------------------------------------------------

/**
 * \internal
 * @class WPrivate
 */
class SK_CORE_EXPORT WPrivate
{
protected:
    WPrivate(WPrivatable * p);

public:
    virtual ~WPrivate();

    WPrivatable * sk_q;
};

#endif /* SK_P_H_ */
