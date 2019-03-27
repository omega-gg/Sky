//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WUnzipper.h"

#ifndef SK_NO_UNZIPPER

// Qt includes
#include <QDir>
#ifdef QT_LATEST
#include <QFileInfo>
#endif

// QuaZIP includes
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
    static void extractFile(QuaZip * zip, const QString & fileName, const QString & destination);

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

/* static */void WUnzipperPrivate::extractFile(QuaZip * zip, const QString & fileName,
                                                             const QString & destination)
{
    zip->setCurrentFile(fileName);

    QuaZipFile zipFile(zip);

    if (zipFile.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WUnzipperPrivate:extractFile: Cannot extract file %s", fileName.C_STR);

        return;
    }

    QString path = QDir(destination).absolutePath() + '/' + fileName;

    QFileInfo info(path);

    QDir().mkpath(info.absolutePath());

    QFile file(path);

    if (file.open(QIODevice::WriteOnly) == false)
    {
        qWarning("WUnzipperPrivate:extractFile: Cannot open file %s", fileName.C_STR);

        return;
    }

    file.write(zipFile.readAll());
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

    QStringList list;

    QuaZipFileInfo info;

    while (zip.getCurrentFileInfo(&info))
    {
        list.push_back(info.name);

        if (zip.goToNextFile() == false) break;
    }

    return list;
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
