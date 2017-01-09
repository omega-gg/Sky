//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WLOADERNETWORK_P_H
#define WLOADERNETWORK_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/WAbstractLoader_p>

#ifndef SK_NO_LOADERNETWORK

// Forward declarations
class QNetworkAccessManager;
class QNetworkReply;

class SK_CORE_EXPORT WLoaderNetworkPrivate : public WAbstractLoaderPrivate
{
public:
    WLoaderNetworkPrivate(WLoaderNetwork * p);

    void init();

public: // Slots
    void onFinished(QNetworkReply * reply);

public: // Variables
    QNetworkAccessManager * manager;

protected:
    W_DECLARE_PUBLIC(WLoaderNetwork)
};

#endif // SK_NO_LOADERNETWORK
#endif // WLOADERNETWORK_P_H
