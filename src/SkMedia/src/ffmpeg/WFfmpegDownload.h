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

#ifndef WFFMPEGDOWNLOAD_H
#define WFFMPEGDOWNLOAD_H

// Qt includes
#include <QThread>
#include <QAtomicInt>
#include <QStringList>

// Sk includes
#include <Sk>

#ifndef SK_NO_FFMPEGDOWNLOAD

// Forward declarations
struct AVIOContext;
struct WFfmpegDownloadInput;

//-------------------------------------------------------------------------------------------------
// WFfmpegDownload
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WFfmpegDownload : public QThread
{
    Q_OBJECT

public:
    // NOTE: The urls are read in sequence, then a single file comes out of them at 'path', which
    //       has no extension: the extension is up to the streams and fileName gives the result.
    WFfmpegDownload(const QStringList & urls,
                    const QString     & path,
                    const QStringList & options = QStringList(), QObject * parent = NULL);

    /* virtual */ ~WFfmpegDownload();

public: // Interface
    void setProxy(const QString & host, const QString & password = QString());

    void abort();

protected: // QThread reimplementation
    /* virtual */ void run();

private: // Functions
    AVIOContext * openIo(const QString & url);

    qint64 getTotal();

    bool copy(const QString & url, const QString & fileName);

    bool mux(const QStringList & urls, bool remote);

    bool openInput(QList<WFfmpegDownloadInput> * inputs, const QString & url, bool remote);

    int write(QList<WFfmpegDownloadInput> * inputs, const char    * format,
                                                    const QString & fileName, bool remote);

    bool apply(const QString & fileName, const QString & extension);

    void setProgress(int percent, qint64 received);

private: // Static functions
    static int interrupt(void * data);

signals:
    // NOTE: The total is -1 when we don't know it.
    void progress(int percent, qint64 received, qint64 total);

public: // Properties
    // NOTE: This is empty until the download is done, and when it failed.
    QString fileName() const;

private: // Variables
    QStringList _urls;

    QString _path;

    QByteArray _header;
    QByteArray _proxy;

    QString _fileName;

    QAtomicInt _abort;

    int _progress;

    qint64 _received;
    qint64 _done;
    qint64 _total;
};

#endif // SK_NO_FFMPEGDOWNLOAD
#endif // WFFMPEGDOWNLOAD_H
