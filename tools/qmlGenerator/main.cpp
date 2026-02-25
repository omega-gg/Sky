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

// Forward declarations
void skipLines(QTextStream * stream, QString * content, QString * line);

void skipElse(QTextStream * stream, QString * line);

//-------------------------------------------------------------------------------------------------
// Global variables

static QStringList defines;

//-------------------------------------------------------------------------------------------------
// Functions
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
    int index = string.lastIndexOf(' ');

    if (index == -1)
    {
        return false;
    }
    else return match(string.mid(index + 1));
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

QStringList getFileNames(const QString & path)
{
    QStringList fileNames;

    QDir dir(path);

    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    foreach (const QFileInfo & info, list)
    {
        QString suffix = info.suffix().toLower();

        if (suffix != "qml" && suffix != "sky") continue;

        fileNames.append(info.fileName());
    }

    return fileNames;
}

QString scanFile(const QString & input)
{
    QString content;

    QFile file(input);

    if (file.open(QIODevice::ReadOnly) == false)
    {
        return QString();
    }

    QTextStream stream(&file);

    QString line = stream.readLine();

    while (line.isNull() == false)
    {
        if (line.startsWith("//#"))
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

void applyContent(const QString & path, const QString & content)
{
    qDebug("Generating file: %s", path.C_STR);

    QFile file(path);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate) == false) return;

    QTextStream stream(&file);

    stream << content;
}

void generate(const QString & input, const QString & output)
{
    QStringList fileNames = getFileNames(input);

    foreach (const QString & fileName, fileNames)
    {
        QString path = input + fileName;

        QString content = scanFile(path);

        if (content.isEmpty()) continue;

        applyContent(output + fileName, content);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);

    if (argc < 4)
    {
        qDebug("Usage: qmlGenerator <input path> <output path> <defines>");

        return -1;
    }

    qDebug("Sky qmlGenerator");

    QString input(argv[1]);

    if (QFile::exists(input) == false)
    {
        qWarning("Input folder does not exist: %s", input.C_STR);

        return 1;
    }

    QString output(argv[2]);

    if (QFile::exists(output) == false)
    {
        qWarning("Ouput folder does not exist: %s", output.C_STR);

        return 1;
    }

    QStringList list = QString(argv[3]).split(' ');

    foreach (const QString & string, list)
    {
        defines.append(string);
    }

    qDebug("\nGENERATING");

    generate(input + '/', output + '/');

    qDebug("DONE");

    return 0;
}
