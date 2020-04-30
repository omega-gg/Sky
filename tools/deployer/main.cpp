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
#include <QSize>

// Defines
#define C_STR toLatin1().constData()

//-------------------------------------------------------------------------------------------------
// Global variables

static QString version;

static QStringList defines;

static QStringList fileNames;

static QHash<QString, QString> files;

static QStringList paths;

static QList<int>   dps;
static QList<QSize> sizes;

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

inline bool sort(const QSize & sizeA, const QSize & sizeB)
{
    return (sizeA.width() < sizeB.width());
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void extractPaths(const QString & path)
{
    QDir Dir(path);

    QFileInfoList list = Dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    foreach (const QFileInfo & info, list)
    {
        if (info.fileName().startsWith('.')) continue;

        if (info.isDir())
        {
            extractPaths(info.filePath());
        }
        else paths.append(info.filePath());
    }
}

int extractNumber(const QString & content, int * index)
{
    QString string;

    QChar character = content.at(*index);

    while (character.isDigit())
    {
        string.append(character);

        (*index)++;

        character = content.at(*index);
    }

    return string.toInt();
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

    qDebug("Copying file: %s", fileName.C_STR);

    return true;
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

void applyFiles(const QString & path)
{
    foreach (const QString & fileName, fileNames)
    {
        QString name = fileName;

        int index = name.lastIndexOf('/');

        if (index != -1)
        {
            name = name.mid(index + 1);
        }

        if (copyFile(fileName, path + name) == false)
        {
            qWarning("applyFiles: Cannot copy file %s.", fileName.C_STR);
        }

        name = name.mid(0, name.indexOf('.'));

        defines.append(name);

        files.insert(name, fileName);
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

QString scanFile(const QString & input)
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
                        return content;
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
                            return content;
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

    return content;
}

void createResource(const QString & path, const QString & filePath, const QString & source)
{
    QString fileName = path + source.mid(0, source.lastIndexOf('/'));

    if (QDir().mkpath(fileName) == false)
    {
        qWarning("createResource: Cannot create folder %s.", fileName.C_STR);

        return;
    }

    fileName = filePath + source;

    if (copyFile(fileName, path + source) == false)
    {
        qWarning("createResource: Cannot copy file %s.", fileName.C_STR);

        return;
    }
}

void scanResources(const QString & path, const QString & content, const QString & fileName)
{
    int indexA = content.indexOf("\"pictures/");

    while (indexA != -1)
    {
        indexA++;

        int indexB = content.indexOf('"', indexA);

        QString source = content.mid(indexA, indexB - indexA);

        int index = fileName.lastIndexOf('/');

        if (index != -1)
        {
             createResource(path, fileName.mid(0, index + 1), source);
        }
        else createResource(path, "", source);

        indexA = content.indexOf("\"pictures/", indexB + 1);
    }
}

void scanDp(const QString & content)
{
    int index = content.indexOf("dp");

    while (index != -1)
    {
        index += 2;

        int number = extractNumber(content, &index);

        if (number && dps.contains(number) == false)
        {
            dps.append(number);
        }

        index = content.indexOf("dp", index);
    }
}

void scanSize(const QString & content)
{
    int index = content.indexOf("size");

    while (index != -1)
    {
        index += 4;

        int numberA = extractNumber(content, &index);

        if (numberA && content.at(index) == 'x')
        {
            index++;

            int numberB = extractNumber(content, &index);

            if (numberB)
            {
                QSize size(numberA, numberB);

                if (sizes.contains(size) == false)
                {
                    sizes.append(size);
                }
            }
        }

        index = content.indexOf("size", index);
    }
}

void scanFolder(const QString & path)
{
    QString stylePath;
    QString styleContent;

    QDir dir(path);

    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    foreach (const QFileInfo & info, list)
    {
        if (info.isDir() || info.suffix().toLower() != "qml") continue;

        QString content = scanFile(info.filePath());

        scanDp  (content);
        scanSize(content);

        QString name = info.baseName();

        if (name.contains("Style"))
        {
            QString fileName = files.value(name);

            if (fileName.isEmpty() == false)
            {
                scanResources(path, content, fileName);

                if (name == "Style")
                {
                    stylePath = info.path() + '/' + name + ".qml";

                    styleContent = content;

                    continue;
                }
            }
        }

        replaceFile(info.path() + '/' + name + ".qml", content);
    }

    if (stylePath.isEmpty()) return;

    std::sort(dps.begin(), dps.end());

    std::sort(sizes.begin(), sizes.end(), sort);

    QString string;

    string.append('\n');

    qDebug("Generating dp(s)");

    foreach (int dp, dps)
    {
        QString number = QString::number(dp);

        string.append("    property int dp" + number + ": " + number + " * ratio\n");
    }

    string.append('\n');

    qDebug("Generating size(s)");

    foreach (const QSize & size, sizes)
    {
        QString numberA = QString::number(size.width ());
        QString numberB = QString::number(size.height());

        string.append("    property variant size" + numberA + 'x' + numberB
                      +
                      ": size(" + numberA + ", " + numberB + ")\n");
    }

    int index = styleContent.indexOf("ratio") + 5;

    index = styleContent.indexOf("\n", index) + 1;

    styleContent.insert(index, string);

    replaceFile(stylePath, styleContent);
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

    QString path = QString(argv[1]) + '/';

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

    applyFiles(path);

    scanFolder(path);

    extractPaths(path);

    generateQrc(argv[3]);

    qDebug("DONE");

    return 0;
}
