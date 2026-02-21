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

#include "WBashScript.h"

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

WBashScriptPrivate::WBashScriptPrivate(WBashScript * p) : WPrivate(p) {}

/* virtual */ WBashScriptPrivate::~WBashScriptPrivate()
{
    if (running == false) return;

    terminate();
}

void WBashScriptPrivate::init()
{
    running = false;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBashScriptPrivate::applyRunning(bool running)
{
    Q_Q(WBashScript);

    this->running = running;

    emit q->runningChanged();
}

void WBashScriptPrivate::terminate()
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

    if (process.waitForFinished(1000) == false)
    {
        if (process.waitForFinished(2000) == false)
        {
            process.kill();

            process.waitForFinished(1000);
        }
    }
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBashScriptPrivate::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (running == false) return;

    Q_Q(WBashScript);

    QObject::disconnect(&process, 0, q, 0);

    onOutput     ();
    onOutputError();

    applyRunning(false);

    WBashScriptResult result(exitCode == 0 && exitStatus == QProcess::NormalExit);

    result.output      = output;
    result.outputError = outputError;

    emit q->finished(result);
}

void WBashScriptPrivate::onOutput()
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

void WBashScriptPrivate::onOutputError()
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

/* explicit */ WBashScript::WBashScript(QObject * parent)
    : QObject(parent), WPrivatable(new WBashScriptPrivate(this))
{
    Q_D(WBashScript); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBashScriptResult WBashScript::run(const QString     & fileName,
                                                     const QStringList & arguments,
                                                     bool                asynchronous)
{
    Q_D(WBashScript);

    if (d->running == false)
    {
        d->output      = QString();
        d->outputError = QString();
    }
    else stop();

    if (d->pathBash.isEmpty())
    {
        setPathBash(findBash());
    }

    if (QFile::exists(fileName) == false)
    {
        qWarning("WBashScript::run: fileName %s does not exist.", fileName.C_STR);

        return WBashScriptResult();
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
    list.append(command);

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

        return WBashScriptResult(true);
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
            QObject::disconnect(&(d->process), 0, this, 0);

            d->applyRunning(false);

            return WBashScriptResult();
        }

        QEventLoop loop;

        connect(sk, SIGNAL(aboutToQuit()), &loop, SLOT(quit()));

        while (d->process.state() != QProcess::NotRunning)
        {
            loop.exec();

            if (sk->isQuitting())
            {
                QObject::disconnect(&(d->process), 0, this, 0);

                d->terminate();

                d->applyRunning(false);

                return WBashScriptResult();
            }
        }

        /*while (d->process.state() != QProcess::NotRunning)
        {
            QCoreApplication::processEvents();

            if (sk->isQuitting())
            {
                QObject::disconnect(&(d->process), 0, this, 0);

                return WBashScriptResult();
            }
        }*/

        QObject::disconnect(&(d->process), 0, this, 0);

        d->applyRunning(false);

        d->onOutput     ();
        d->onOutputError();

        WBashScriptResult result(d->process.exitCode() == 0
                                 &&
                                 d->process.exitStatus() == QProcess::NormalExit);

        result.output      = d->output;
        result.outputError = d->outputError;

        return result;
    }
}

/* Q_INVOKABLE */ void WBashScript::stop()
{
    Q_D(WBashScript);

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

/* Q_INVOKABLE static */ QString WBashScript::findBash()
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

/* Q_INVOKABLE static */ QString WBashScript::quote(const QString & string)
{
    QString result = string;

    result.replace('\'', "'\"'\"'");

    return "'" + result + "'";
}

/* Q_INVOKABLE static */ QVariantMap WBashScript::resultToMap(const WBashScriptResult & result)
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

bool WBashScript::isRunning() const
{
    Q_D(const WBashScript); return d->running;
}

QString WBashScript::pathBash() const
{
    Q_D(const WBashScript); return d->pathBash;
}

void WBashScript::setPathBash(const QString & path)
{
    Q_D(WBashScript);

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
