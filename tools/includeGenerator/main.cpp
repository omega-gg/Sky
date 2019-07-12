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

void writeInclude(const QFileInfo & info)
{
    QString content;

    QFile fileNew;
    QFile fileLite;

    if (info.fileName().contains("_p.h"))
    {
        fileNew .setFileName("private/" + info.fileName());
        fileLite.setFileName("private/" + info.baseName());

        content = "#include \"../" + info.filePath() + "\"\n";
    }
    else
    {
        fileNew .setFileName(info.fileName());
        fileLite.setFileName(info.baseName());

        paths.append(info.fileName());

        content = "#include \"" + info.filePath() + "\"\n";
    }

    qDebug(info.fileName().toLatin1().constData());

    fileNew .open(QIODevice::WriteOnly | QIODevice::Truncate);
    fileLite.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream out(&fileNew);

    out << content;

    fileNew.close();

    out.setDevice(&fileLite);

    out << content;

    fileLite.close();
}

//-------------------------------------------------------------------------------------------------

void scanFolder(const QString & path, const QString & destination = QString())
{
    QDir Dir(path);

    QFileInfoList list = Dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    foreach (QFileInfo info, list)
    {
        if (info.isDir())
        {
            if (info.fileName() == "3rdparty") continue;

            qDebug(info.filePath().toLatin1().constData());

            scanFolder(info.filePath(), destination);
        }
        else
        {
            if (info.suffix() != "h") continue;

            writeInclude(info);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void writeGlobalInclude(const QString & name)
{
    qDebug("Generating %s", qPrintable(name + ".h"));

    QString content;

    QString nameUpper = name.toUpper();

    content = "#ifndef " + nameUpper + "_H\n"
              +
              "#define " + nameUpper + "_H\n";

    foreach (const QString & path, paths)
    {
        content.append("#include \"" + path + "\"\n");
    }

    content.append("#endif // " + nameUpper + "_H\n");

    QFile file(name + ".h");

    file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream stream(&file);

    stream << content;

    file.close();

    file.setFileName(name);

    file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    stream << content;
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
        qDebug("Usage: includeGenerator <path input> [path output]");

        return 0;
    }

    qDebug("Sky include generator");

    QDir().mkdir("private");

    QString path = argv[1];

    qDebug("Input: %s",  qPrintable(path));
    qDebug("Output: %s", qPrintable(QDir::currentPath()));

    qDebug("\nGENERATING");

    scanFolder(path);

    QString name = QDir::current().dirName();

    writeGlobalInclude(name);

    qDebug("DONE");
}
