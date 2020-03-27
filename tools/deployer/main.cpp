//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the tools for Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

// Qt includes
#include <QCoreApplication>
#include <QTextStream>
#include <QDir>

// Defines
#define C_STR toLatin1().constData()

//-------------------------------------------------------------------------------------------------
// Global variables

static QString version;

static QStringList defines;

static QStringList fileNames;

static QStringList paths;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void extractPaths(const QString & path)
{
    QDir Dir(path);

    QFileInfoList list = Dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    foreach (const QFileInfo & info, list)
    {
        if (info.isDir())
        {
            extractPaths(info.filePath());
        }
        else paths.append(info.filePath());
    }
}

//-------------------------------------------------------------------------------------------------

bool match(const QString & string)
{
    QStringList list = string.split(':');

    foreach (const QString & name, list)
    {
        if (name.endsWith('*'))
        {
            QString string = name;

            string.chop(1);

            foreach (const QString & define, defines)
            {
                if (define.startsWith(string))
                {
                    return true;
                }
            }
        }
        else if (defines.contains(name))
        {
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool copyFile(const QString & fileName, const QString & newName)
{
    QFile::remove(newName);

    if (QFile::copy(fileName, newName) == false)
    {
        qWarning("copyFile: Cannot copy file %s.", fileName.C_STR);

        return false;
    }
    else return true;
}

void replaceFile(const QString & path, const QString & content)
{
    qDebug("Generating file: %s", path.C_STR);

    QFile file(path);

    file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream stream(&file);

    stream << content;
}

//-------------------------------------------------------------------------------------------------

void createFile(const QString & path, const QString & filePath, const QString & source)
{
    QString fileName = path + source.mid(0, source.lastIndexOf('/'));

    if (QDir().mkpath(fileName) == false)
    {
        qWarning("createFile: Cannot create folder %s.", fileName.C_STR);

        return;
    }

    qDebug("Copying file: %s", source.C_STR);

    fileName = filePath + source;

    if (copyFile(fileName, path + source) == false)
    {
        qWarning("createFile: Cannot copy file %s.", fileName.C_STR);
    }
}

void copyFiles(const QString & path, const QString & filePath, const QString & fileName)
{
    QFile file(fileName);

    file.open(QIODevice::ReadOnly);

    QByteArray bytes = file.readAll();

    int indexA = bytes.indexOf("\"pictures/");

    while (indexA != -1)
    {
        indexA++;

        int indexB = bytes.indexOf('"', indexA);

        QString source = bytes.mid(indexA, indexB - indexA);

        createFile(path, filePath, source);

        indexA = bytes.indexOf("\"pictures/", indexB + 1);
    }
}

void applyFiles(const QString & path)
{
    foreach (const QString & fileName, fileNames)
    {
        QString name = fileName;

        int index = name.lastIndexOf('/');

        if (index != -1)
        {
            name = name.mid(index + 1);

            if (name.contains("Style"))
            {
                copyFiles(path, fileName.mid(0, index + 1), fileName);
            }
        }
        else if (name.contains("Style"))
        {
            copyFiles(path, "", fileName);
        }

        if (copyFile(fileName, path + name) == false)
        {
            qWarning("applyFiles: Cannot copy file %s.", fileName.C_STR);
        }

        name = name.mid(0, name.indexOf('.'));

        defines.append(name);
    }
}

//-------------------------------------------------------------------------------------------------

bool skipLines(QTextStream * stream, QString * line, const QString & string)
{
    *line = stream->readLine();

    if (line->isNull()) return false;

    while (line->startsWith(string) == false)
    {
        *line = stream->readLine();

        if (line->isNull()) return false;
    }

    return true;
}

void scanFile(const QString & input, const QString & output)
{
    QString content;

    QFile file(input);

    file.open(QIODevice::ReadOnly);

    QTextStream stream(&file);

    QString line = stream.readLine();

    while (line.isNull() == false)
    {
        if (line.startsWith("import "))
        {
            line = line.simplified();

            if (line.startsWith("import QtQuick"))
            {
                content.append("import QtQuick " + version + '\n');
            }
            else if (line.startsWith("import Sky") == false || line.at(10) == ' ')
            {
                content.append(line + '\n');
            }

            line = stream.readLine();
        }
        else if (line.startsWith("//#"))
        {
            QString string = line.mid(3);

            if (string == "END")
            {
                line = stream.readLine();

                continue;
            }

            if (match(string))
            {
                line = stream.readLine();

                if (line.isNull()) break;

                while (line.startsWith("//#") == false)
                {
                    content.append(line + '\n');

                    line = stream.readLine();

                    if (line.isNull())
                    {
                        replaceFile(output, content);

                        return;
                    }
                }

                if (line.startsWith("//#ELSE"))
                {
                    if (skipLines(&stream, &line, "//#") == false) break;
                }
            }
            else
            {
                if (skipLines(&stream, &line, "//#") == false) break;

                if (line.startsWith("//#ELSE"))
                {
                    line = stream.readLine();

                    if (line.isNull()) break;

                    while (line.startsWith("//#") == false)
                    {
                        content.append(line + '\n');

                        line = stream.readLine();

                        if (line.isNull())
                        {
                            replaceFile(output, content);

                            return;
                        }
                    }
                }
            }
        }
        else
        {
            content.append(line + '\n');

            line = stream.readLine();
        }
    }

    replaceFile(output, content);
}

void scanFolder(const QString & path)
{
    QDir Dir(path);

    QFileInfoList list = Dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    foreach (const QFileInfo & info, list)
    {
        if (info.isDir())
        {
            extractPaths(info.filePath());
        }
        else if (info.baseName().isEmpty() == false)
        {
            QString filePath = info.filePath();

            if (info.suffix().toLower() == "qml")
            {
                scanFile(filePath, info.path() + '/' + info.fileName());
            }

            paths.append(filePath);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void generateQrc(const QString & path)
{
    QString content = "<RCC>\n<qresource>\n";

    foreach (const QString & filePath, paths)
    {
        content.append("    <file>" + filePath + "</file>\n");
    }

    content.append("</qresource>\n</RCC>\n");

    QFile file(path);

    file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream stream(&file);

    stream << content;
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);

    if (argc < 5)
    {
        qDebug("Usage: deployer <path> <version> <qrc output> <defines> [file paths]");

        return -1;
    }

    qDebug("Sky deployer");

    qDebug("\nDEPLOYING");

    QString path = argv[1];

    version = argv[2];

    if (version.startsWith("1."))
    {
         defines.append("QT_4");
    }
    else defines.append("QT_5");

    QStringList list = QString(argv[4]).split(' ');

    foreach (const QString & string, list)
    {
        defines.append(string);
    }

    for (int i = 5; i < argc; i++)
    {
        fileNames.append(QDir::fromNativeSeparators(argv[i]));
    }

    applyFiles(path + '/');

    scanFolder(path);

    generateQrc(argv[3]);

    qDebug("DONE");
}
