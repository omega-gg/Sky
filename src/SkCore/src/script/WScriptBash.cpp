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

// Sk includes
#include <WControllerApplication>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WScriptBashPrivate::WScriptBashPrivate(WScriptBash * p) : WPrivate(p) {}

/* virtual */ WScriptBashPrivate::~WScriptBashPrivate()
{
    if (running == false) return;

    terminate();
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

void WScriptBashPrivate::terminate()
{
#ifdef Q_OS_WIN
    qint64 id = process.processId();

    if (id > 0)
    {
        // NOTE windows: This is useful for killing child processes.
        QProcess::execute("taskkill", { "/PID", QString::number(id), "/T", "/F" });
    }
    else process.terminate();
#else
    process.terminate();
#endif

    if (process.waitForFinished(3000) == false)
    {
        process.kill();

        process.waitForFinished(1000);
    }
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

    WScriptBashResult result(exitCode == 0 && exitStatus == QProcess::NormalExit);

    result.output      = output;
    result.outputError = outputError;

    emit q->finished(result);
}

void WScriptBashPrivate::onOutput()
{
    QByteArray data = process.readAllStandardOutput();

    if (data.isEmpty()) return;

    output.append(data);

    if (data.endsWith('\n')) data.chop(1);

#ifdef QT_4
    qDebug().nospace() << QString::fromUtf8(data);
#else
    qDebug().noquote().nospace() << QString::fromUtf8(data);
#endif
}

void WScriptBashPrivate::onOutputError()
{
    QByteArray data = process.readAllStandardError();

    if (data.isEmpty()) return;

    outputError.append(data);

    if (data.endsWith('\n')) data.chop(1);

#ifdef QT_4
    qDebug().nospace() << QString::fromUtf8(data);
#else
    qDebug().noquote().nospace() << QString::fromUtf8(data);
#endif
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

WScriptBashResult WScriptBash::run(const QString     & fileName,
                                   const QStringList & arguments, bool asynchronous)
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

#ifndef QT_4
    d->process.setProgram(d->pathBash);

    d->process.setArguments(list);
#endif

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

#ifdef QT_4
        d->process.start(d->pathBash, list);
#else
        d->process.start();
#endif

        d->applyRunning(true);

        return WScriptBashResult(true);
    }
    else
    {
#ifdef QT_4
        d->process.start(d->pathBash, list);
#else
        d->process.start();
#endif

        d->applyRunning(true);

        if (d->process.waitForStarted() == false)
        {
            return WScriptBashResult();
        }

        while (d->process.state() != QProcess::NotRunning)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);

            if (sk->isQuitting())
            {
                return WScriptBashResult();
            }
        }

        d->applyRunning(false);

        QObject::disconnect(&(d->process), 0, this, 0);

        d->onOutput     ();
        d->onOutputError();

        WScriptBashResult result(d->process.exitCode() == 0
                                 &&
                                 d->process.exitStatus() == QProcess::NormalExit);

        result.output      = d->output;
        result.outputError = d->outputError;

        return result;
    }
}

void WScriptBash::stop()
{
    Q_D(WScriptBash);

    if (d->running == false) return;

    disconnect(&(d->process), 0, this, 0);

    d->terminate();

    d->applyRunning(false);

    d->output      = QString();
    d->outputError = QString();
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

/* static */ QVariantMap WScriptBash::resultToMap(const WScriptBashResult & result)
{
    QVariantMap map;

    map.insert("ok", result.ok);

    map.insert("output",      result.output);
    map.insert("outputError", result.outputError);

    return map;
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
