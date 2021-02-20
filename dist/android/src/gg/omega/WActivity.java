//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

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

package gg.omega;

// Android includes
import android.os.*;
import android.content.Intent;
//import android.util.Log;

// Qt includes
import org.qtproject.qt5.android.bindings.QtActivity;

public class WActivity extends QtActivity
{
    //---------------------------------------------------------------------------------------------
    // C++ functions
    //---------------------------------------------------------------------------------------------

    public static native void updateIntent();

    //---------------------------------------------------------------------------------------------
    // QtActivity reimplementation
    //---------------------------------------------------------------------------------------------

    @Override public void onNewIntent(Intent intent)
    {
        //Log.d("Sk", "onNewIntent QShareActivity");

        super.onNewIntent(intent);

        setIntent(intent);

        updateIntent();
    }
}
