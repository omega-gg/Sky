//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

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
#include <QDir>
#include <QTextStream>

// Defines
#define C_STR toLocal8Bit().constData()

//-------------------------------------------------------------------------------------------------
// Global variables

QList<QString> paths;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void extractPaths(const QString & path)
{
    QDir Dir(path);

    QFileInfoList list = Dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    foreach (QFileInfo info, list)
    {
        if (info.isDir())
        {
            extractPaths(info.filePath());
        }
        else paths.append(info.filePath());
    }
}

//-------------------------------------------------------------------------------------------------

void replaceFile(const QString & path, const QString & content)
{
    qDebug("Fixing file headers: %s", path.C_STR);

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

void scanFile(const QString & path)
{
    QFile file(path);

    file.open(QIODevice::ReadOnly);

    QTextStream stream(&file);

    QString line = stream.readLine();

    while (line.isNull() == false)
    {
        if (line.startsWith("import SkyComponents"))
        {
            int pos = stream.pos() - 2;

            stream.seek(0);

            QString content = stream.readAll();

            int end = content.indexOf('\n', pos);

            if (end == -1) return;

            pos -= line.length();

            content.remove(pos, end - pos + 1);

            replaceFile(path, content);

            return;
        }

        line = stream.readLine();
    }
}

void scanFolder(const QString & path)
{
    QDir Dir(path);

    QFileInfoList list = Dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    foreach (QFileInfo info, list)
    {
        if (info.isDir())
        {
            extractPaths(info.filePath());
        }
        else if (info.baseName().isEmpty() == false)
        {
            QString filePath = info.filePath();

            if (info.suffix() == "qml")
            {
                scanFile(filePath);
            }

            paths.append(filePath);
        }
    }
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);

    if (argc < 2)
    {
        qDebug("Usage: deployer <path> <qrc output>");

        return -1;
    }

    qDebug("Sky deployer");

    qDebug("\nDEPLOYING");

    scanFolder(argv[1]);

    generateQrc(argv[2]);

    qDebug("DONE");
}
