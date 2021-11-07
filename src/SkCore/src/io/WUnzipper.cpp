//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#include "WUnzipper.h"

#ifndef SK_NO_UNZIPPER

// Qt includes
#include <QDir>
#include <QBuffer>
#ifdef QT_LATEST
#include <QFileInfo>
#endif

// QuaZip includes
#include <quazipfile.h>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/Sk_p>

class SK_CORE_EXPORT WUnzipperPrivate : public WPrivate
{
public:
    WUnzipperPrivate(WUnzipper * p);

    void init();

public: // Static functions
    static QByteArray extractData(QuaZip * zip, const QString & fileName);

    static void extractFile(QuaZip * zip, const QString & fileName, const QString & destination);

    static QStringList getFileNames(QuaZip * zip);

public: // Variables
    QString fileName;

    QString destination;

    QStringList fileNames;

protected:
    W_DECLARE_PUBLIC(WUnzipper)
};

//-------------------------------------------------------------------------------------------------

WUnzipperPrivate::WUnzipperPrivate(WUnzipper * p) : WPrivate(p) {}

void WUnzipperPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Prviate static functions
//-------------------------------------------------------------------------------------------------

/* static */ QByteArray WUnzipperPrivate::extractData(QuaZip * zip, const QString & fileName)
{
    zip->setCurrentFile(fileName);

    QuaZipFile zipFile(zip);

    if (zipFile.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WUnzipperPrivate:extractData: Cannot extract file %s", fileName.C_STR);

        return QByteArray();
    }
    else return zipFile.readAll();
}

/* static */ void WUnzipperPrivate::extractFile(QuaZip * zip, const QString & fileName,
                                                              const QString & destination)
{
    QByteArray data = extractData(zip, fileName);

    if (data.isEmpty()) return;

    QString path = QDir(destination).absolutePath() + '/' + fileName;

    QFileInfo info(path);

    QDir().mkpath(info.absolutePath());

    QFile file(path);

    if (file.open(QIODevice::WriteOnly) == false)
    {
        qWarning("WUnzipperPrivate:extractFile: Cannot open file %s", fileName.C_STR);

        return;
    }

    file.write(data);
}

//-------------------------------------------------------------------------------------------------

/* static */ QStringList WUnzipperPrivate::getFileNames(QuaZip * zip)
{
    QStringList list;

    QuaZipFileInfo info;

    while (zip->getCurrentFileInfo(&info))
    {
        list.push_back(info.name);

        if (zip->goToNextFile() == false) break;
    }

    return list;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WUnzipper::WUnzipper(QObject * parent)
    : QObject(parent), WPrivatable(new WUnzipperPrivate(this))
{
    Q_D(WUnzipper); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WUnzipper::run()
{
    Q_D(WUnzipper);

    extract(d->fileName, d->destination, d->fileNames);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void WUnzipper::extract(const QString     & fileName,
                                                 const QString     & destination,
                                                 const QStringList & fileNames)
{
    QuaZip zip(fileName);

    if (zip.open(QuaZip::mdUnzip) == false)
    {
        qWarning("WUnzipper::extract: Cannot open zip file %s.", fileName.C_STR);

        return;
    }

    if (fileNames.isEmpty())
    {
        foreach (const QString & fileName, getFileNames(fileName))
        {
            WUnzipperPrivate::extractFile(&zip, fileName, destination);
        }
    }
    else
    {
        foreach (const QString & fileName, fileNames)
        {
            WUnzipperPrivate::extractFile(&zip, fileName, destination);
        }
    }
}

/* Q_INVOKABLE static */ QByteArray WUnzipper::extractFile(QBuffer       * buffer,
                                                           const QString & fileName)
{
    QuaZip zip(buffer);

    if (zip.open(QuaZip::mdUnzip) == false)
    {
        qWarning("WUnzipper::extractFile: Cannot open zip buffer.");

        return QByteArray();
    }

    return WUnzipperPrivate::extractData(&zip, fileName);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QStringList WUnzipper::getFileNames(const QString & fileName)
{
    QuaZip zip(fileName);

    if (zip.open(QuaZip::mdUnzip) == false)
    {
        qWarning("WUnzipper::getFileNames: Cannot open zip file %s.", fileName.C_STR);

        return QStringList();
    }

    if (zip.goToFirstFile() == false)
    {
        qWarning("WUnzipper::getFileNames: Zip file is empty %s.", fileName.C_STR);

        return QStringList();
    }

    return WUnzipperPrivate::getFileNames(&zip);
}

/* Q_INVOKABLE static */ QStringList WUnzipper::getFileNames(QBuffer * buffer)
{
    QuaZip zip(buffer);

    if (zip.open(QuaZip::mdUnzip) == false)
    {
        qWarning("WUnzipper::getFileNames: Cannot open zip buffer.");

        return QStringList();
    }

    if (zip.goToFirstFile() == false)
    {
        qWarning("WUnzipper::getFileNames: Zip buffer is empty.");

        return QStringList();
    }

    return WUnzipperPrivate::getFileNames(&zip);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WUnzipper::fileName() const
{
    Q_D(const WUnzipper); return d->fileName;
}

void WUnzipper::setFileName(const QString & fileName)
{
    Q_D(WUnzipper);

    if (d->fileName == fileName) return;

    d->fileName = fileName;

    emit fileNameChanged();
}

//-------------------------------------------------------------------------------------------------

QString WUnzipper::destination() const
{
    Q_D(const WUnzipper); return d->destination;
}

void WUnzipper::setDestination(const QString & destination)
{
    Q_D(WUnzipper);

    if (d->destination == destination) return;

    d->destination = destination;

    emit destinationChanged();
}

//-------------------------------------------------------------------------------------------------

QStringList WUnzipper::fileNames() const
{
    Q_D(const WUnzipper); return d->fileNames;
}

void WUnzipper::setFileNames(const QStringList & fileNames)
{
    Q_D(WUnzipper);

    if (d->fileNames == fileNames) return;

    d->fileNames = fileNames;

    emit fileNamesChanged();
}

#endif // SK_NO_UNZIPPER
