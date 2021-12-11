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

// Forward declarations
void skipLines(QTextStream * stream, QString * content, QString * line);

void skipElse(QTextStream * stream, QString * line);

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

void extractPaths(const QString & path, const QString & base)
{
    QDir dir(path);

    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    foreach (const QFileInfo & info, list)
    {
        if (info.fileName().startsWith('.')) continue;

        if (info.isDir())
        {
            extractPaths(info.filePath(), base + info.fileName() + '/');
        }
        else paths.append(base + info.fileName());
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

bool matchStar(const QString & string, bool match)
{
    foreach (const QString & define, defines)
    {
        if (define.startsWith(string) == match)
        {
            return true;
        }
    }

    return false;
}

bool match(const QString & string)
{
    QStringList listA = string.split(':');

    foreach (const QString & name, listA)
    {
        bool result = true;

        QStringList listB = name.split('+');

        foreach (const QString & name, listB)
        {
            bool match = true;

            QString string = name;

            if (string.startsWith('!'))
            {
                string.remove(0, 1);

                match = false;
            }

            if (string.endsWith('*'))
            {
                string.chop(1);

                if (matchStar(string, match) == false)
                {
                    result = false;

                    break;
                }
            }
            else if (defines.contains(string) != match)
            {
                result = false;

                break;
            }
        }

        if (result) return true;
    }

    return false;
}

bool matchElif(const QString & string)
{
    int index = string.lastIndexOf(' ', 7);

    if (index == -1)
    {
        return false;
    }
    else return match(string.mid(index + 1));
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

bool writeNext(QTextStream * stream, QString * content, QString * line)
{
    *line = stream->readLine();

    if (line->isNull()) return false;

    while (line->startsWith("//#") == false)
    {
        content->append(*line + '\n');

        *line = stream->readLine();

        if (line->isNull()) return false;
    }

    return true;
}

void writeLines(QTextStream * stream, QString * content, QString * line)
{
    while (writeNext(stream, content, line))
    {
        if (line->startsWith("//#END")) return;

        if (line->startsWith("//#ELSE") || line->startsWith("//#ELIF"))
        {
            skipElse(stream, line);

            return;
        }

        if (match(line->mid(3)))
        {
            writeLines(stream, content, line);
        }
        else skipLines(stream, content, line);
    }
}

//-------------------------------------------------------------------------------------------------

bool skipNext(QTextStream * stream, QString * line)
{
    *line = stream->readLine();

    if (line->isNull()) return false;

    while (line->startsWith("//#") == false)
    {
        *line = stream->readLine();

        if (line->isNull()) return false;
    }

    return true;
}

void skipLines(QTextStream * stream, QString * content, QString * line)
{
    int count = 0;

    while (skipNext(stream, line))
    {
        if (line->startsWith("//#ELSE"))
        {
            if (count == 0)
            {
                writeLines(stream, content, line);

                return;
            }
        }
        else if (line->startsWith("//#ELIF"))
        {
            if (count == 0 && matchElif(*line))
            {
                writeLines(stream, content, line);

                return;
            }
        }
        else if (line->startsWith("//#END"))
        {
            if (count == 0) return;

            count--;
        }
        else count++;
    }
}

void skipElse(QTextStream * stream, QString * line)
{
    int count = 0;

    while (skipNext(stream, line))
    {
        if (line->startsWith("//#ELSE") || line->startsWith("//#ELIF")) continue;

        if (line->startsWith("//#END"))
        {
            if (count == 0) return;

            count--;
        }
        else count++;
    }
}

//-------------------------------------------------------------------------------------------------

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
            if (match(line.mid(3)))
            {
                writeLines(&stream, &content, &line);
            }
            else skipLines(&stream, &content, &line);

            if (line.startsWith("//#END"))
            {
                line = stream.readLine();
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

void scanResources(const QString & name,
                   const QString & path, const QString & content, const QString & fileName)
{
    QString match = '"' + name + '/';

    int indexA = content.indexOf(match);

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

        indexA = content.indexOf(match, indexB + 1);
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
                scanResources("pictures", path, content, fileName);
                scanResources("icons",    path, content, fileName);

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

    // NOTE: We want a clean base path.
    extractPaths(path, QString());

    generateQrc(argv[3]);

    qDebug("DONE");

    return 0;
}
