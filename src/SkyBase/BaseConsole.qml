//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkyBase.

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

import QtQuick 1.0
import Sky     1.0

BaseTextEdit
{
    id: baseConsole

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property string log

    property int maximumLength: st.baseConsole_maximumLength

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pUpdate: true

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    wrapMode: Text.Wrap

//#MOBILE
    // NOTE: We want to be read only.
    enabled: false
//#END

    //---------------------------------------------------------------------------------------------
    // Style

    color      : st.baseConsole_color
    colorCursor: color

    font.family   : st.baseConsole_fontFamily
    font.pixelSize: st.baseConsole_pixelSize
    font.bold     : st.baseConsole_bold

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    onVisibleChanged: if (visible) text = log

    onLogChanged: if (visible && pUpdate) text = log

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function append(string)
    {
//#QT_4
        pSetLog(log + string);

        var size = log.length;

        if (size > maximumLength)
        {
            pSetLog(log.substring(size - maximumLength));
        }

        if (visible) text = log;
//#ELSE
        if (visible)
        {
            //-------------------------------------------------------------------------------------
            // NOTE: The length between log and the TextEdit may vary.

            pSetLog(log + string);

            insert(length, string);

            //-------------------------------------------------------------------------------------

            var count = length - maximumLength;

            if (count > 0)
            {
                pSetLog(log.substring(count));

                remove(0, count);
            }
        }
        else
        {
            pSetLog(log + string);

            var size = log.length;

            if (size > maximumLength)
            {
                pSetLog(log.substring(size - maximumLength));
            }
        }
//#END
    }

    //---------------------------------------------------------------------------------------------
    // Private

    function pSetLog(string)
    {
        pUpdate = false;

        log = string;

        pUpdate = true;
    }
}
