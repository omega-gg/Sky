//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#ifndef WHOOKOUTPUT_P_H
#define WHOOKOUTPUT_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#include <private/WAbstractHook_p>

#ifndef SK_NO_HOOKOUTPUT

// Sk includes
#include <WBroadcastClient>

//-------------------------------------------------------------------------------------------------
// WHookOutputData
//-------------------------------------------------------------------------------------------------

struct WHookOutputData
{
    WHookOutputData(const WBackendOutput * output)
    {
        this->output = output;
    }

    WBroadcastSource source;

    const WBackendOutput * output;
};

//-------------------------------------------------------------------------------------------------
// WHookOutputPrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WHookOutputPrivate : public WAbstractHookPrivate
{
public:
    WHookOutputPrivate(WHookOutput * p);

    void init();

public: // Functions
    WHookOutputData * getData(const WBroadcastSource & source);
    WHookOutputData * getData(const WBackendOutput   * output);

    void setActive(bool active);

public: // Slots
    void onOutputChanged();

    void onConnectedChanged();

public: // Variables
    WBroadcastClient client;

    QList<WHookOutputData> datas;

    WHookOutputData * currentData;

    bool active;

protected:
    W_DECLARE_PUBLIC(WHookOutput)
};

#endif // SK_NO_HOOKOUTPUT
#endif // WHOOKOUTPUT_P_H
