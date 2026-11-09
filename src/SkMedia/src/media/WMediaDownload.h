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

#ifndef WMEDIADOWNLOAD_H
#define WMEDIADOWNLOAD_H

// Qt includes
#include <QObject>
#include <QVariantMap>
#include <QStringList>

// Sk includes
#include <Sk>

#ifndef SK_NO_MEDIADOWNLOAD

// Forward declarations
class WMediaDownload;
class WAbstractDownloadHookPrivate;
class WMediaDownloadPrivate;

//-------------------------------------------------------------------------------------------------
// WAbstractDownloadHook
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WAbstractDownloadHook : public QObject, public WPrivatable
{
    Q_OBJECT

protected:
    explicit WAbstractDownloadHook(QObject * parent = NULL);

    WAbstractDownloadHook(WAbstractDownloadHookPrivate * p, QObject * parent = NULL);

public: // Abstract interface
    // NOTE: Returns true when the hook gets the file of this url itself.
    virtual bool check(const QString & url) = 0;

    // NOTE: Gets the file of this url, then calls applyFile on the download.
    virtual void load(WMediaDownload * download, const QString & url) = 0;

    // NOTE: Releases what the hook created for this download, it's the last call for it. This can
    //       happen during one of the hook's signals.
    virtual void clear(WMediaDownload * download) = 0;

private:
    W_DECLARE_PRIVATE(WAbstractDownloadHook)
};

//-------------------------------------------------------------------------------------------------
// WMediaDownload
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WMediaDownload : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(bool isRunning READ isRunning NOTIFY ended)

    Q_PROPERTY(bool hasError READ hasError NOTIFY ended)

    Q_PROPERTY(QString source READ source NOTIFY ended)

    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)

    Q_PROPERTY(qint64 received READ received NOTIFY progressChanged)
    Q_PROPERTY(qint64 total    READ total    NOTIFY progressChanged)

private:
    WMediaDownload(const QVariantMap & track,
                   const QString     & path, const QStringList & options, QObject * parent);
public:
    /* virtual */ ~WMediaDownload();

public: // Interface
    // NOTE: The download removes its files, and it doesn't emit ended.
    Q_INVOKABLE void abort();

    // NOTE: This aborts a running download, then deletes it. A finished download keeps its files.
    Q_INVOKABLE void remove();

    // NOTE: This is for a WAbstractDownloadHook. An empty fileName means the download failed.
    Q_INVOKABLE void applyFile(const QString & fileName);

    // NOTE: This is for the thread and a WAbstractDownloadHook.
    Q_INVOKABLE void applyProgress(int percent, qint64 received, qint64 total);

signals:
    void progressChanged();

    void ended();

public: // Properties
    bool isRunning() const;

    bool hasError() const;

    QString source() const;

    int progress() const;

    qint64 received() const;
    qint64 total   () const;

private:
    W_DECLARE_PRIVATE(WMediaDownload)

    Q_PRIVATE_SLOT(d_func(), void onFinished())

    Q_PRIVATE_SLOT(d_func(), void onWritten(bool))

    friend class WControllerMedia;
    friend class WControllerMediaPrivate;
};

#include <private/WMediaDownload_p>

#endif // SK_NO_MEDIADOWNLOAD
#endif // WMEDIADOWNLOAD_H
