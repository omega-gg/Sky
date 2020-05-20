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

#ifndef WVLCENGINE_P_H
#define WVLCENGINE_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/Sk_p>

#ifndef SK_NO_VLCENGINE

class SK_MEDIA_EXPORT WVlcEnginePrivate : public WPrivate
{
public: // Enums
    enum EventType
    {
        EventCreate = QEvent::User,
        EventClear
    };

public:
    WVlcEnginePrivate(WVlcEngine * p);

    void init(QThread * thread);

public: // Variables
    libvlc_instance_t * instance;

protected:
    W_DECLARE_PUBLIC(WVlcEngine)
};

#endif // SK_NO_VLCENGINE
#endif // WVLCENGINE_P_H
