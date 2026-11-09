//=================================================================================================
/*
    Copyright (C) 2015-2026 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMedia.

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

#ifndef WMEDIADOWNLOAD_P_H
#define WMEDIADOWNLOAD_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_MEDIADOWNLOAD

// Forward declarations
class WFfmpegDownload;

//-------------------------------------------------------------------------------------------------
// WAbstractDownloadHookPrivate
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WAbstractDownloadHookPrivate : public WPrivate
{
public:
    WAbstractDownloadHookPrivate(WAbstractDownloadHook * p);

protected:
    W_DECLARE_PUBLIC(WAbstractDownloadHook)
};

//-------------------------------------------------------------------------------------------------
// WMediaDownloadPrivate
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WMediaDownloadPrivate : public WPrivate
{
public:
    WMediaDownloadPrivate(WMediaDownload * p);

    void init(const QVariantMap & track, const QString & path, const QStringList & options);

public: // Functions
    void start(const QStringList & urls, bool withProgress);

    void finish(bool ok);

public: // Static functions
    static void addValue(QString & vbml, const QString & key, const QString & value);

    static QString getVbml(const QVariantMap & track, const QString & fileName);

public: // Slots
    void onFinished();

    void onWritten(bool ok);

public: // Variables
    QVariantMap track;

    // NOTE: The file path without its extension.
    QString path;

    QStringList options;

    WFfmpegDownload * thread;

    // NOTE: The hook that gets the file, when there's one.
    WAbstractDownloadHook * hook;

    bool started;
    bool running;
    bool error;

    // NOTE: The media file, once the thread is done.
    QString fileName;

    QString source;

    int progress;

    qint64 received;
    qint64 total;

protected:
    W_DECLARE_PUBLIC(WMediaDownload)
};

#endif // SK_NO_MEDIADOWNLOAD
#endif // WMEDIADOWNLOAD_P_H
