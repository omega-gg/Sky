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

QString version;

QStringList defines;

//QString defineA;
//QString defineB;

QStringList paths;

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

    qDebug("MATCH A %s", string.C_STR);

    foreach (const QString & name, list)
    {
        qDebug("MATCH B %s", name.C_STR);

        if (name.endsWith('*'))
        {
            QString string = name.chopped(1);

            foreach (const QString & define, defines)
            {
                if (define.startsWith(string))
                {
                    qDebug("MATCH A");

                    return true;
                }
            }
        }
        else if (defines.contains(name))
        {
            qDebug("MATCH B");

            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

void replaceFile(const QString & path, const QString & content)
{
    qDebug("Generating file: %s", path.C_STR);

    QFile file(path);

    file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream stream(&file);

    stream << content;
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

    QStringList inputs;
    QStringList outputs;

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
                QString name = info.baseName();

                inputs.append(filePath);

                outputs.append(info.path() + '/' + name + ".qml");

                defines.append(name);
            }

            paths.append(filePath);
        }
    }

    foreach (const QString & define, defines)
    {
        qDebug("DEFINE %s", define.C_STR);
    }

    foreach (const QString & input, inputs)
    {
        scanFile(input, outputs.takeFirst());
    }
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);

    if (argc < 4)
    {
        qDebug("Usage: deployer <path> <version> <qrc output> [defines]");

        return -1;
    }

    qDebug("Sky deployer");

    qDebug("\nDEPLOYING");

    version = argv[2];

    if (version.startsWith("1."))
    {
         defines.append("QT_4");
    }
    else defines.append("QT_5");

    for (int i = 4; i < argc; i++)
    {
        defines.append(argv[i]);
    }

    scanFolder(argv[1]);

    generateQrc(argv[3]);

    qDebug("DONE");
}
