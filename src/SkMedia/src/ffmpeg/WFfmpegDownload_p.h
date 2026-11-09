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

#ifndef WFFMPEGDOWNLOAD_P_H
#define WFFMPEGDOWNLOAD_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QAtomicInt>

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_FFMPEGDOWNLOAD

// Forward declarations
struct AVFormatContext;
struct AVIOContext;
struct AVPacket;
struct AVStream;

//-------------------------------------------------------------------------------------------------
// WFfmpegDownloadInput
//-------------------------------------------------------------------------------------------------

struct WFfmpegDownloadInput
{
    AVFormatContext * format;

    AVPacket * packet;

    int video;
    int audio;

    AVStream * outputVideo;
    AVStream * outputAudio;

    bool ready;
};

//-------------------------------------------------------------------------------------------------
// WFfmpegDownloadPrivate
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WFfmpegDownloadPrivate : public WPrivate
{
public:
    WFfmpegDownloadPrivate(WFfmpegDownload * p);

    void init(const QStringList & urls, const QString & path, const QStringList & options);

public: // Functions
    AVIOContext * openIo(const QString & url);

    qint64 getTotal();

    bool copy(const QString & url, const QString & part);

    bool mux(const QStringList & sources, bool remote);

    bool openInput(QList<WFfmpegDownloadInput> * inputs, const QString & url, bool remote);

    int write(QList<WFfmpegDownloadInput> * inputs, const char    * format,
                                                    const QString & part, bool remote);

    bool apply(const QString & part, const QString & extension);

    void setProgress(int percent, qint64 bytes);

public: // Static functions
    static int interrupt(void * data);

    static QByteArray getUrl(const QString & url);

    static void closeInputs(QList<WFfmpegDownloadInput> * inputs);

    static int readInput(WFfmpegDownloadInput * input);

    static qint64 getPacketTime(const WFfmpegDownloadInput & input);

public: // Variables
    QStringList urls;

    // NOTE: The file path without its extension.
    QString path;

    QByteArray header;
    QByteArray proxy;

    QString fileName;

    QAtomicInt abort;

    int progress;

    qint64 received;
    qint64 done;
    qint64 total;

protected:
    W_DECLARE_PUBLIC(WFfmpegDownload)
};

#endif // SK_NO_FFMPEGDOWNLOAD
#endif // WFFMPEGDOWNLOAD_P_H
