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
#include <QStringList>

// Sk includes
#include <Sk>

#ifndef SK_NO_FFMPEGDOWNLOAD

class WFfmpegDownloadPrivate;

class SK_MEDIA_EXPORT WFfmpegDownload : public QThread, public WPrivatable
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

signals:
    // NOTE: The total is -1 when we don't know it.
    void progress(int percent, qint64 received, qint64 total);

public: // Properties
    // NOTE: This is empty until the download is done, and when it failed.
    QString fileName() const;

private:
    W_DECLARE_PRIVATE(WFfmpegDownload)
};

#include <private/WFfmpegDownload_p>

#endif // SK_NO_FFMPEGDOWNLOAD
#endif // WFFMPEGDOWNLOAD_H
