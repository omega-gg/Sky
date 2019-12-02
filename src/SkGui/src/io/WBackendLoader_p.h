//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WBACKENDLOADER_P_H
#define WBACKENDLOADER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_BACKENDLOADER

class SK_GUI_EXPORT WBackendLoaderPrivate : public WPrivate
{
public:
    WBackendLoaderPrivate(WBackendLoader * p);

    /* virtual */ ~WBackendLoaderPrivate();

    void init();

public: // Static functions
    static WBackendNet * getBackend(const QString & id);

protected:
    W_DECLARE_PUBLIC(WBackendLoader)
};

#endif // SK_NO_BACKENDLOADER
#endif // WBACKENDLOADER_P_H
