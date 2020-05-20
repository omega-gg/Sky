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

#ifndef WPLUGININTERFACE_H
#define WPLUGININTERFACE_H

#include <Sk>

class SK_CORE_EXPORT WPluginInterface
{
public:
    virtual ~WPluginInterface() {}

    virtual bool load()   { return true; }
    virtual bool unload() { return true; }
};

Q_DECLARE_INTERFACE(WPluginInterface, "sk.core.pluginInterface")

#endif // WPLUGININTERFACE_H
