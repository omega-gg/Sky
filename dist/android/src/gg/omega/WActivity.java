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

// Java includes
import java.io.File;

// Android includes
import android.os.*;
import android.net.Uri;
import android.content.Intent;
import android.content.Context;
import android.database.Cursor;
import android.provider.MediaStore;
import android.media.MediaScannerConnection;
//import android.util.Log;

// Qt includes
import org.qtproject.qt.android.bindings.QtActivity;

// Sk includes
import gg.omega.WFile;

public class WActivity extends QtActivity
{
    //---------------------------------------------------------------------------------------------
    // Static variables
    //---------------------------------------------------------------------------------------------

    static final int ACTIVITY_GALLERY = 1;

    //---------------------------------------------------------------------------------------------
    // C++ functions
    //---------------------------------------------------------------------------------------------

    public static native void updateIntent();

    public static native void imageSelected(String fileName);

    //---------------------------------------------------------------------------------------------
    // Interface
    //---------------------------------------------------------------------------------------------

    public void openGallery()
    {
        Intent intent = new Intent(Intent.ACTION_PICK);

        intent.setType("image/*");

        startActivityForResult(intent, ACTIVITY_GALLERY);
    }

    public String getIntentText()
    {
        Intent intent = getIntent();

        String action = intent.getAction();

        if (action == "android.intent.action.SEND")
        {
            Bundle bundle = intent.getExtras();

            if (bundle == null) return null;

            Object object = bundle.get(Intent.EXTRA_STREAM);

            if (object == null)
            {
                object = bundle.getCharSequence("android.intent.extra.TEXT");

                if (object == null) return null;

                return object.toString();
            }
            else return WFile.getPath(getApplicationContext(), (Uri) object);
        }
        else if (action == "android.intent.action.VIEW")
        {
            return WFile.getPath(getApplicationContext(), intent.getData());
        }
        else return null;
    }

    public void scanFile(String fileName)
    {
        File file = new File(fileName);

        String [] paths = { file.toString() };

        MediaScannerConnection.scanFile(getApplicationContext(), paths, null, null);

        //Intent intent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.fromFile(file));

        //sendBroadcast(intent);
    }

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

    //---------------------------------------------------------------------------------------------
    // Events
    //---------------------------------------------------------------------------------------------

    @Override protected void onActivityResult(int request, int result, Intent data)
    {
        if (request == ACTIVITY_GALLERY)
        {
            if (result == RESULT_OK)
            {
                String fileName = WFile.getPath(getApplicationContext(), data.getData());

                imageSelected(fileName);
            }
            else imageSelected("");
        }

        super.onActivityResult(request, result, data);
    }
}
