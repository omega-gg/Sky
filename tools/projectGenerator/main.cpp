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

//-------------------------------------------------------------------------------------------------
// Global variables

QStringList headers;
QStringList sources;

QStringList paths;

QString prefix;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void scanFolder(const QString & path)
{
    QDir dir(path);

    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    QFileInfoList folders;

    foreach (const QFileInfo & info, list)
    {
        if (info.isDir())
        {
            folders.append(info);

            continue;
        }

        if (info.isFile() == false) continue;

        QString path = prefix + info.filePath().remove(0, 2);

        qDebug(qPrintable(path));

        QString suffix = info.suffix();

        if (suffix == "h" || suffix == "hpp")
        {
            headers.append(path);
        }
        else if (suffix == "c" || suffix == "cpp")
        {
            sources.append(path);
        }
        else paths.append(path);
    }

    foreach (const QFileInfo & info, folders)
    {
        scanFolder(info.filePath());
    }
}

//-------------------------------------------------------------------------------------------------

void writePaths(QString & content, const QString & name, QStringList & paths)
{
    if (paths.isEmpty()) return;

    QString string = name + " += ";

    QString spaces;

    for (int i = 0; i < string.length(); i++)
    {
        spaces.append(' ');
    }

    content += string + paths.takeFirst() + " \\\n";

    foreach (const QString & path, paths)
    {
        content += spaces + path + " \\\n";
    }

    content += '\n';
}

void writeProject(const QString & name)
{
    qDebug("Generating: %s", qPrintable(name));

    QString content;

    writePaths(content, "HEADERS",     headers);
    writePaths(content, "SOURCES",     sources);
    writePaths(content, "OTHER_FILES", paths);

    // NOTE: Removing the last '\n'.
    content.chop(1);

    QFile file(name);

    file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream stream(&file);

    stream << content;

    file.close();
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);

    if (argc == 4)
    {
        QDir::setCurrent(argv[2]);

        prefix = argv[3];
    }
    else if (argc == 3)
    {
        QDir::setCurrent(argv[2]);
    }
    else if (argc != 2)
    {
        qDebug("Usage: projectGenerator <name> [path] [prefix]");

        return 0;
    }

    qDebug("Sky project generator");

    QString name = argv[1];

    qDebug("Name: %s", qPrintable(name));

    qDebug("Path: %s", qPrintable(QDir::currentPath()));

    qDebug("\nGENERATING");

    scanFolder("");

    writeProject(name + ".pro");

    qDebug("DONE");
}
