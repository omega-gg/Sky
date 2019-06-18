//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WBACKENDUNIVERSAL_P_H
#define WBACKENDUNIVERSAL_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#include <private/WBackendNet_p>

#ifndef SK_NO_BACKENDUNIVERSAL

class SK_BACKEND_EXPORT WBackendUniversalPrivate : public WBackendNetPrivate
{
public:
    WBackendUniversalPrivate(WBackendUniversal * p);

    void init();

public: // Variables
    QString source;

protected:
    W_DECLARE_PUBLIC(WBackendUniversal)
};

#endif // SK_NO_BACKENDUNIVERSAL
#endif // WBACKENDUNIVERSAL_P_H
