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
import android.view.WindowManager;
import android.database.Cursor;
import android.provider.MediaStore;
import android.media.MediaScannerConnection;
import android.support.v4.content.FileProvider;
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
    static final int ACTIVITY_SHARE   = 2;

    static String message = null;

    //---------------------------------------------------------------------------------------------
    // C++ functions
    //---------------------------------------------------------------------------------------------

    public static native void updateIntent();

    public static native void imageSelected(String fileName);

    public static native void shareFinished(boolean ok);

    //---------------------------------------------------------------------------------------------
    // Interface
    //---------------------------------------------------------------------------------------------

    public void openGallery()
    {
        // NOTE: Using ACTION_GET_CONTENT instead of ACTION_PICK otherwise we get a RESULT_CANCELED
        //       when selecting among multiple applications.
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);

        intent.setType("image/*");

        startActivityForResult(intent, ACTIVITY_GALLERY);
    }

    public void share(String title, String text, String fileName, String type)
    {
        Intent intent = new Intent(Intent.ACTION_SEND);

        intent.setType(type);

        if (text.isEmpty() == false)
        {
            intent.putExtra(Intent.EXTRA_TEXT, text);
        }

        if (fileName.isEmpty() == false)
        {
            File file = new File(fileName);

            Uri uri = FileProvider.getUriForFile(this, "gg.omega.provider", file);

            intent.putExtra(Intent.EXTRA_STREAM, uri);

            intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        }

        startActivityForResult(Intent.createChooser(intent, title), ACTIVITY_SHARE);
    }

    public void scanFile(String fileName)
    {
        String [] paths = { fileName };

        MediaScannerConnection.scanFile(getApplicationContext(), paths, null, null);

        //Intent intent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.fromFile(file));

        //sendBroadcast(intent);
    }

    public void prepareFullScreen(boolean enabled)
    {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.P) return;

        WindowManager.LayoutParams attributes = getWindow().getAttributes();

        if (enabled)
        {
            attributes.layoutInDisplayCutoutMode
                = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        }
        else attributes.layoutInDisplayCutoutMode
                = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_DEFAULT;
    }

    public void goBack()
    {
        Intent intent = new Intent(Intent.ACTION_MAIN);

        intent.addCategory(Intent.CATEGORY_HOME);

        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

        startActivity(intent);
    }

    public String getIntentText()
    {
        Intent intent = getIntent();

        String action = intent.getAction();

        if (action == "") return message;

        // NOTE: We clear the action to avoid applying the same intent when restarting the
        //       application.
        intent.setAction("");

        setIntent(intent);

        if (action == "android.intent.action.SEND")
        {
            Bundle bundle = intent.getExtras();

            if (bundle != null)
            {
                Object object = bundle.get(Intent.EXTRA_STREAM);

                if (object == null)
                {
                    object = bundle.getCharSequence("android.intent.extra.TEXT");

                    if (object != null)
                    {
                        message = object.toString();
                    }
                }
                else message = WFile.getPath(getApplicationContext(), (Uri) object);
            }
        }
        else if (action == "android.intent.action.VIEW")
        {
            message = intent.getData().toString();
        }

        return message;
    }

    public void clearIntent()
    {
        message = null;
    }

    public int getOrientation()
    {
        int orientation = getWindowManager().getDefaultDisplay().getRotation();

        if      (orientation == 1) return  90;
        else if (orientation == 2) return 180;
        else if (orientation == 3) return 270;
        else                       return   0;
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
        else if (request == ACTIVITY_SHARE)
        {
            if (result == RESULT_OK) shareFinished(true);
            else                     shareFinished(false);
        }

        super.onActivityResult(request, result, data);
    }
}
