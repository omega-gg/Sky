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

#include "WBashManager.h"

#ifndef SK_NO_BASHMANAGER

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BASHMANAGER_MAX_JOBS = 8;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WBashManagerPrivate::WBashManagerPrivate(WBashManager * p) : WPrivate(p) {}

void WBashManagerPrivate::init()
{
    maxJobs = BASHMANAGER_MAX_JOBS;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBashManagerPrivate::processJob()
{
    if (pending.isEmpty() || jobs.count() == maxJobs) return;

    Q_Q(WBashManager);

    WBashManagerPrivateJob job = pending.takeFirst();

    WBashScript * script = job.script;

    jobs.append(script);

    WBashScriptResult result = script->run(job.fileName, job.arguments, true);

    if (result.ok)
    {
        QObject::connect(script, SIGNAL(finished      (const WBashScriptResult &)),
                         q,      SLOT(onScriptFinished(const WBashScriptResult &)));
    }
    else onScriptFinished(result);
}

void WBashManagerPrivate::removePending(WBashScript * script)
{
    for (int i = 0; i < pending.count(); i++)
    {
        const WBashManagerPrivateJob & job = pending.at(i);

        if (job.script != script) continue;

        pending.removeAt(i);
    }
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBashManagerPrivate::onScriptFinished(const WBashScriptResult & result)
{
    Q_Q(WBashManager);

    WBashScript * script = static_cast<WBashScript *> (q->sender());

    QObject::disconnect(script, 0, q, 0);

    int index = scripts.indexOf(script);

    script->deleteLater();

    if (index == -1) return;

    jobs.removeOne(script);

    scripts.removeAt(index);

    WBashManagerResult manager(ids.takeAt(index));

    manager.bash = result;

    emit q->finished(manager);

    processJob();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WBashManager::WBashManager(QObject * parent)
    : QObject(parent), WPrivatable(new WBashManagerPrivate(this))
{
    Q_D(WBashManager); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WBashManagerResult WBashManager::run(const QString     & fileName,
                                                       const QStringList & arguments)
{
    Q_D(WBashManager);

    WBashScript * script = new WBashScript(this);

    if (d->jobs.count() == d->maxJobs)
    {
        d->scripts.append(script);

        WBashManagerPrivateJob job;

        job.script = script;

        job.fileName  = fileName;
        job.arguments = arguments;

        d->pending.append(job);

        return WBashManagerResult(d->ids.generateId(), true);
    }

    WBashScriptResult bash = script->run(fileName, arguments, true);

    if (bash.ok == false)
    {
        delete script;

        return WBashManagerResult();
    }

    connect(script, SIGNAL(finished      (const WBashScriptResult &)),
            this,   SLOT(onScriptFinished(const WBashScriptResult &)));

    d->scripts.append(script);

    d->jobs.append(script);

    WBashManagerResult result(d->ids.generateId());

    result.bash = bash;

    return result;
}

/* Q_INVOKABLE */ bool WBashManager::stop(int id)
{
    Q_D(WBashManager);

    int index = d->ids.indexOf(id);

    if (index == -1) return false;

    d->ids.removeAt(index);

    WBashScript * script = d->scripts.takeAt(index);

    d->removePending(script);

    d->jobs.removeOne(script);

    script->stop();

    delete script;

    d->processJob();

    return true;
}

/* Q_INVOKABLE */ bool WBashManager::clear()
{
    Q_D(WBashManager);

    foreach (WBashScript * script, d->scripts)
    {
        script->stop();

        delete script;
    }

    d->scripts.clear();
    d->ids    .clear();

    d->pending.clear();
    d->jobs   .clear();

    return true;
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QVariantMap WBashManager::resultToMap(const WBashManagerResult & result)
{
    QVariantMap map = WBashScript::resultToMap(result.bash);

    map.insert("id", result.id);

    return map;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

int WBashManager::maxJobs() const
{
    Q_D(const WBashManager); return d->maxJobs;
}

void WBashManager::setMaxJobs(int max)
{
    Q_D(WBashManager);

    if (d->maxJobs == max) return;

    d->maxJobs = max;

    emit maxJobsChanged();
}

#endif // SK_NO_BASHMANAGER
