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
// NOTE: This implementation is greatly inspired by this project:
//       https://github.com/saparkhid/AndroidFileNamePicker

package gg.omega;

// Java includes
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.UUID;

// Android includes
import android.os.Build;
import android.os.Environment;
import android.net.Uri;
import android.text.TextUtils;
import android.content.Context;
import android.content.ContentUris;
import android.database.Cursor;
import android.provider.DocumentsContract;
import android.provider.OpenableColumns;
import android.provider.MediaStore;
import android.util.Log;

public class WFile
{
    //---------------------------------------------------------------------------------------------
    // Static variables
    //---------------------------------------------------------------------------------------------

    static final String FILE_FOLDER = "upload_part";

    //---------------------------------------------------------------------------------------------
    // Static functions
    //---------------------------------------------------------------------------------------------

    public static String getPath(Context context, Uri uri)
    {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT)
        {
            if (isWhatsApp(uri))
            {
                return getPathWhatsApp(context, uri);
            }

            if ("content".equalsIgnoreCase(uri.getScheme()))
            {
                String [] projection = { MediaStore.Images.Media.DATA };

                Cursor cursor = null;

                try
                {
                    cursor = context.getContentResolver().query(uri, projection, null, null, null);

                    int index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);

                    if (cursor.moveToFirst())
                    {
                        return cursor.getString(index);
                    }
                }
                catch (Exception exception)
                {
                    exception.printStackTrace();
                }
            }
        }
        else
        {
            if (isExternalStorage(uri))
            {
                String id = DocumentsContract.getDocumentId(uri);

                String path = getPathExternal(id.split(":"));

                if (path == null || fileExists(path) == false)
                {
                    Log.d("Sk", "Copy file as a fallback");

                    path = copyFile(context, uri, FILE_FOLDER);
                }

                if (path == "") return null;
                else            return path;
            }

            if (isDownload(uri))
            {
                if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M)
                {
                    String id = DocumentsContract.getDocumentId(uri);

                    if (id.startsWith("raw:"))
                    {
                        return id.replaceFirst("raw:", "");
                    }

                    Uri contentUri = null;

                    try
                    {
                        contentUri = Uri.parse("content://downloads/public_downloads");

                        contentUri = ContentUris.withAppendedId(contentUri, Long.valueOf(id));
                    }
                    catch (NumberFormatException exception)
                    {
                        exception.printStackTrace();
                    }

                    if (contentUri != null)
                    {
                        return getDataColumn(context, contentUri, null, null);
                    }
                }
                else
                {
                    Cursor cursor = null;

                    try
                    {
                        String [] projection = { MediaStore.MediaColumns.DISPLAY_NAME };

                        cursor = context.getContentResolver().query(uri, projection,
                                                                    null, null, null);

                        if (cursor != null && cursor.moveToFirst())
                        {
                            String fileName = cursor.getString(0);

                            fileName = Environment.getExternalStorageDirectory().toString()
                                       +
                                       "/Download/" + fileName;

                            if (TextUtils.isEmpty(fileName) == false)
                            {
                                return fileName;
                            }
                        }
                    }
                    finally
                    {
                        if (cursor != null) cursor.close();
                    }

                    String id = DocumentsContract.getDocumentId(uri);

                    if (TextUtils.isEmpty(id) == false)
                    {
                        if (id.startsWith("raw:"))
                        {
                            return id.replaceFirst("raw:", "");
                        }

                        String [] prefixes =
                        {
                            "content://downloads/public_downloads",
                            "content://downloads/my_downloads"
                        };

                        for (String prefix: prefixes)
                        {
                            try
                            {
                                Uri contentUri = ContentUris.withAppendedId(Uri.parse(prefix),
                                                                            Long.valueOf(id));

                                return getDataColumn(context, contentUri, null, null);
                            }
                            catch (NumberFormatException exception)
                            {
                                // In Android 8 and Android P the id is not a number
                                return uri.getPath().replaceFirst("^/document/raw:", "")
                                                    .replaceFirst("^raw:",           "");
                            }
                        }
                    }
                }
            }

            if (isMedia(uri))
            {
                String [] split = DocumentsContract.getDocumentId(uri).split(":");

                String type = split[0];

                Log.d("Sk", "MEDIA DOCUMENT TYPE: " + type);

                Uri contentUri = null;

                if ("image".equals(type))
                {
                    contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                }
                else if ("video".equals(type))
                {
                    contentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
                }
                else if ("audio".equals(type))
                {
                    contentUri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
                }
                else if ("document".equals(type))
                {
                    contentUri = MediaStore.Files.getContentUri(MediaStore.getVolumeName(uri));
                }

                String [] arguments = { split[1] };

                return getDataColumn(context, contentUri, "_id=?", arguments);
            }

            if (isGoogleDrive(uri))
            {
                return getPathGoogleDrive(context, uri);
            }

            if (isWhatsApp(uri))
            {
                return getPathWhatsApp(context, uri);
            }

            if ("content".equalsIgnoreCase(uri.getScheme()))
            {
                if (isGooglePhotos(uri))
                {
                    return uri.getLastPathSegment();
                }

                if (isGoogleDrive(uri))
                {
                    return getPathGoogleDrive(context, uri);
                }

                if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q)
                {
                    return getDataColumn(context, uri, null, null);
                }
                else return copyFile(context, uri, FILE_FOLDER);
            }

            if ("file".equalsIgnoreCase(uri.getScheme()))
            {
                return uri.getPath();
            }
        }

        return copyFile(context, uri, FILE_FOLDER);
    }

    public static String copyFile(Context context, Uri uri, String path)
    {
        String [] projection = { OpenableColumns.DISPLAY_NAME, OpenableColumns.SIZE };

        Cursor cursor = context.getContentResolver().query(uri, projection, null, null, null);

        /*
         * Get the column indexes of the data in the Cursor,
         *     * move to the first row in the Cursor, get the data,
         *     * and display it.
         * */

        int index = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);

        cursor.moveToFirst();

        String name = cursor.getString(index);

        File fileName;

        if (path.equals("") == false)
        {
            path = File.separator + path + File.separator + UUID.randomUUID().toString();

            File dir = new File(context.getFilesDir() + path);

            if (dir.exists() == false) dir.mkdirs();

            fileName = new File(context.getFilesDir() + path + File.separator + name);
        }
        else fileName = new File(context.getFilesDir() + File.separator + name);

        try
        {
            InputStream input = context.getContentResolver().openInputStream(uri);

            FileOutputStream stream = new FileOutputStream(fileName);

            byte [] buffer = new byte[1024];

            int read = input.read(buffer);

            while (read != -1)
            {
                stream.write(buffer, 0, read);

                read = input.read(buffer);
            }

            input.close();

            stream.close();
        }
        catch (Exception exception)
        {
            Log.e("Sk", exception.getMessage());
        }

        return fileName.getPath();
    }

    public static String getPathExternal(String [] list)
    {
        String type = list[0];

        Log.d("Sk", "MEDIA EXTSD TYPE: " + type);

        String path = File.separator + list[1];

        Log.d("Sk", "Relative path: " + path);

        String fullPath = "";

        // on my Sony devices (4.4.4 & 5.1.1), `type` is a dynamic string
        // something like "71F8-2C0A", some kind of unique id per storage
        // don't know any API that can get the root path of that storage based on its id.
        //
        // so no "primary" type, but let the check here for other devices
        if ("primary".equalsIgnoreCase(type))
        {
            fullPath = Environment.getExternalStorageDirectory() + path;

            if (fileExists(fullPath)) return fullPath;
        }
        else if ("home".equalsIgnoreCase(type))
        {
            fullPath = "/storage/emulated/0/Documents" + path;

            if (fileExists(fullPath)) return fullPath;
        }

        // Environment.isExternalStorageRemovable() is `true` for external and internal storage
        // so we cannot relay on it.
        //
        // instead, for each possible path, check if file exists
        // we'll start with secondary storage as this could be our (physically) removable sd card

        fullPath = System.getenv("SECONDARY_STORAGE") + path;

        if (fileExists(fullPath)) return fullPath;

        fullPath = System.getenv("EXTERNAL_STORAGE") + path;

        if (fileExists(fullPath)) return fullPath;

        return null;
    }

    public static String getDataColumn(Context context, Uri uri, String selection, String [] arguments)
    {
        Cursor cursor = null;

        try
        {
            String [] projection = { "_data" };

            cursor = context.getContentResolver().query(uri, projection, selection, arguments, null);

            if (cursor != null && cursor.moveToFirst())
            {
                return cursor.getString(cursor.getColumnIndexOrThrow("_data"));
            }
        }
        finally
        {
            if (cursor != null) cursor.close();
        }

        return null;
    }

    public static String getPathGoogleDrive(Context context, Uri uri)
    {
        Cursor cursor = context.getContentResolver().query(uri, null, null, null, null);

        /*
         * Get the column indexes of the data in the Cursor,
         *     * move to the first row in the Cursor, get the data,
         *     * and display it.
         * */

        int index = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);

        cursor.moveToFirst();

        String name = cursor.getString(index);

        File file = new File(context.getCacheDir(), name);

        try
        {
            InputStream input = context.getContentResolver().openInputStream(uri);

            FileOutputStream stream = new FileOutputStream(file);

            int size = Math.min(input.available(), 1 * 1024 * 1024);

            byte [] buffer = new byte[size];

            int read = input.read(buffer);

            while (read != -1)
            {
                stream.write(buffer, 0, read);

                read = input.read(buffer);
            }

            input .close();
            stream.close();

            Log.e("Sk", "Path " + file.getPath());
            Log.e("Sk", "Size " + file.length ());
        }
        catch (Exception exception)
        {
            Log.e("Sk", exception.getMessage());
        }

        return file.getPath();
    }

    public static String getPathWhatsApp(Context context, Uri uri)
    {
        return copyFile(context, uri, "whatsapp");
    }

    public static boolean fileExists(String fileName)
    {
        File file = new File(fileName);

        return file.exists();
    }

    public static boolean isExternalStorage(Uri uri)
    {
        return "com.android.externalstorage.documents".equals(uri.getAuthority());
    }

    public static boolean isDownload(Uri uri)
    {
        return "com.android.providers.downloads.documents".equals(uri.getAuthority());
    }

    public static boolean isMedia(Uri uri)
    {
        return "com.android.providers.media.documents".equals(uri.getAuthority());
    }

    public static boolean isGooglePhotos(Uri uri)
    {
        return "com.google.android.apps.photos.content".equals(uri.getAuthority());
    }

    public static boolean isWhatsApp(Uri uri)
    {
        return "com.whatsapp.provider.media".equals(uri.getAuthority());
    }

    public static boolean isGoogleDrive(Uri uri)
    {
        return ("com.google.android.apps.docs.storage".equals(uri.getAuthority())
                ||
                "com.google.android.apps.docs.storage.legacy".equals(uri.getAuthority()));
    }
}
