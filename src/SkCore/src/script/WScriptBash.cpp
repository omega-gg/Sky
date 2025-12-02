//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.

    - Private License Usage:
    Sky kit licensees holding valid private licenses may use this file in accordance with the
    private license agreement provided with the Software or, alternatively, in accordance with the
    terms contained in written agreement between you and Sky kit authors. For further information
    contact us at contact@omega.gg.
*/
//=================================================================================================

#include "WScriptBash.h"

#ifndef SK_NO_SCRIPTBASH

// Qt includes
#include <QCoreApplication>
#include <QFileInfo>
#ifdef Q_OS_WIN
#include <QDir>
#endif
#ifdef QT_OLD
#include <QDebug>
#endif
#ifdef QT_4
// Qt includes
#include <QStringList>
#endif

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WScriptBashPrivate::WScriptBashPrivate(WScriptBash * p) : WPrivate(p) {}

/* virtual */ WScriptBashPrivate::~WScriptBashPrivate()
{
    if (running == false) return;

    process.kill();

    process.waitForFinished(1000);
}

void WScriptBashPrivate::init()
{
    running = false;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WScriptBashPrivate::applyRunning(bool running)
{
    Q_Q(WScriptBash);

    this->running = running;

    emit q->runningChanged();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WScriptBashPrivate::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (running == false) return;

    Q_Q(WScriptBash);

    QObject::disconnect(&process, 0, q, 0);

    onOutput     ();
    onOutputError();

    applyRunning(false);

    bool ok = (exitCode == 0 && exitStatus == QProcess::NormalExit);

    emit q->finished(ok);
}

void WScriptBashPrivate::onOutput()
{
    QByteArray data = process.readAllStandardOutput();

    if (data.isEmpty()) return;

    if (data.endsWith('\n')) data.chop(1);

    qDebug().noquote().nospace() << QString::fromUtf8(data);
}

void WScriptBashPrivate::onOutputError()
{
    QByteArray data = process.readAllStandardError();

    if (data.isEmpty()) return;

    if (data.endsWith('\n')) data.chop(1);

    qDebug().noquote().nospace() << QString::fromUtf8(data);
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WScriptBash::WScriptBash(QObject * parent)
    : QObject(parent), WPrivatable(new WScriptBashPrivate(this))
{
    Q_D(WScriptBash); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WScriptBash::run(const QString & fileName, const QStringList & arguments, bool asynchronous)
{
    Q_D(WScriptBash);

    if (d->running) stop();

    if (d->pathBash.isEmpty())
    {
        setPathBash(findBash());
    }

#ifdef Q_OS_WIN
    QString command = quote(QDir::fromNativeSeparators(fileName));
#else
    QString command = quote(fileName);
#endif

    foreach (const QString & string, arguments)
    {
        command.append(' ' + quote(string));
    }

    QStringList list;

    list.append("-lc");
    list.append("set -e; " + command);

    d->process.setProgram(d->pathBash);

    d->process.setArguments(list);

    connect(&(d->process), SIGNAL(readyReadStandardOutput()), this, SLOT(onOutput     ()));
    connect(&(d->process), SIGNAL(readyReadStandardError ()), this, SLOT(onOutputError()));

    d->process.setWorkingDirectory(QFileInfo(fileName).absolutePath());

#ifdef Q_OS_WIN
    if (d->environment.isEmpty() == false)
    {
        QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();

        environment.insert("PATH", d->environment + environment.value("PATH"));

        d->process.setProcessEnvironment(environment);
    }
#endif

    if (asynchronous)
    {
        connect(&(d->process), SIGNAL(finished(int, QProcess::ExitStatus)),
                this,          SLOT(onFinished(int, QProcess::ExitStatus)));

        d->process.start();

        d->applyRunning(true);

        return true;
    }
    else
    {
        d->process.start();

        d->applyRunning(true);

        if (d->process.waitForStarted() == false) return false;

        while (d->process.state() != QProcess::NotRunning)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        }

        d->applyRunning(false);

        QObject::disconnect(&(d->process), 0, this, 0);

        d->onOutput     ();
        d->onOutputError();

        return (d->process.exitCode() == 0 && d->process.exitStatus() == QProcess::NormalExit);
    }
}

void WScriptBash::stop()
{
    Q_D(WScriptBash);

    if (d->running == false) return;

    disconnect(&(d->process), 0, this, 0);

    d->process.kill();

    d->process.waitForFinished();

    d->applyRunning(false);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ QString WScriptBash::findBash()
{
    QString environment = QString::fromLocal8Bit(qgetenv("SKY_PATH_BASH"));

    if (environment.isEmpty() == false)
    {
        return environment;
    }

#ifdef Q_OS_WIN
    QStringList list;

    list.append("C:/Program Files/Git/usr/bin/bash.exe");
    list.append("C:/Program Files/Git/bin/bash.exe");
    list.append("C:/Program Files (x86)/Git/usr/bin/bash.exe");
    list.append("C:/Program Files (x86)/Git/bin/bash.exe");

    foreach (const QString & string, list)
    {
        if (QFile::exists(string)) return string;
    }

    return "bash.exe";
#elif defined(Q_OS_MACOS)
    QStringList list;

    list.append("/opt/homebrew/bin/bash");
    list.append("/usr/local/bin/bash");
    list.append("/bin/bash");

    foreach (const QString & string, list)
    {
        if (QFile::exists(string)) return string;
    }

    return "bash";
#elif defined(Q_OS_LINUX)
    QStringList list;

    list.append("/usr/bin/bash");
    list.append("/bin/bash");

    foreach (const QString & string, list)
    {
        if (QFile::exists(string)) return string;
    }

    return "bash";
#else
    return "bash";
#endif
}

/* static */ QString WScriptBash::quote(const QString & string)
{
    QString result = string;

    result.replace('\'', "'\"'\"'");

    return "'" + result + "'";
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WScriptBash::isRunning() const
{
    Q_D(const WScriptBash); return d->running;
}

QString WScriptBash::pathBash() const
{
    Q_D(const WScriptBash); return d->pathBash;
}

void WScriptBash::setPathBash(const QString & path)
{
    Q_D(WScriptBash);

    if (d->pathBash == path) return;

    d->pathBash = path;

#ifdef Q_OS_WIN
    if (path.contains("usr/bin/bash.exe"))
    {
        QString pathUser = d->pathBash;

        pathUser.remove("/bash.exe");

        QString pathMingw = pathUser.replace("usr/", "mingw64/");

        if (QFile::exists(pathMingw))
        {
             d->environment = pathMingw + ";" + pathUser + ";";
        }
        else d->environment = QString();
    }
    else d->environment = QString();
#endif

    emit pathBashChanged();
}

#endif // SK_NO_SCRIPTBASH
