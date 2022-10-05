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

QStringList paths;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void scanFolder(const QString & path)
{
    QDir dir(path);

    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    foreach (QFileInfo info, list)
    {
        if (info.isDir())
        {
            scanFolder(info.filePath());
        }
        else if (info.isFile())
        {
            qDebug(qPrintable(info.filePath()));

            paths.append(info.filePath());
        }
    }
}

//-------------------------------------------------------------------------------------------------

void writeProject(const QString & name)
{
    qDebug("Generating: %s", qPrintable(name));

    QString content = "OTHER_FILES += " + paths.takeFirst() + " \\\n";

    foreach (const QString & path, paths)
    {
        content += "               " + path + " \\\n";
    }

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

    if (argc == 3)
    {
        QDir::setCurrent(argv[2]);
    }
    else if (argc != 2)
    {
        qDebug("Usage: projectGenerator <name> [path]");

        return 0;
    }

    qDebug("Sky project generator");

    QString name = argv[1];

    qDebug("Name: %s", qPrintable(name));

    qDebug("Path: %s", qPrintable(QDir::currentPath()));

    qDebug("\nGENERATING");

    scanFolder("");

    if (paths.isEmpty() == false)
    {
        writeProject(name + ".pro");
    }

    qDebug("DONE");
}
