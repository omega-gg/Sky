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

static const int BASHMANAGER_MAX_JOBS = 1;

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

    WScriptBash * script = job.script;

    jobs.append(script);

    script->run(job.fileName, job.arguments, true);

    QObject::connect(script, SIGNAL(finished      (const WScriptBashResult &)),
                     q,      SLOT(onScriptFinished(const WScriptBashResult &)));
}

void WBashManagerPrivate::removePending(WScriptBash * script)
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

void WBashManagerPrivate::onScriptFinished(const WScriptBashResult & result)
{
    Q_Q(WBashManager);

    WScriptBash * script = static_cast<WScriptBash *> (q->sender());

    QObject::disconnect(script, 0, q, 0);

    int index = scripts.indexOf(script);

    script->deleteLater();

    if (index != -1)
    {
        jobs.removeOne(script);

        scripts.removeAt(index);

        WBashManagerResult manager;

        manager.id = ids.takeAt(index);

        manager.bash = result;

        emit q->finished(manager);
    }

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

    WScriptBash * script = new WScriptBash(this);

    d->scripts.append(script);

    int id = d->ids.generateId();

    WBashManagerResult result;

    result.id = id;

    if (d->jobs.count() == d->maxJobs)
    {
        WBashManagerPrivateJob job;

        job.script = script;

        job.fileName  = fileName;
        job.arguments = arguments;

        d->pending.append(job);

        return result;
    }

    d->jobs.append(script);

    result.bash = script->run(fileName, arguments, true);

    connect(script, SIGNAL(finished      (const WScriptBashResult &)),
            this,   SLOT(onScriptFinished(const WScriptBashResult &)));

    return result;
}

/* Q_INVOKABLE */ bool WBashManager::stop(int id)
{
    Q_D(WBashManager);

    int index = d->ids.indexOf(id);

    if (index == -1) return false;

    d->ids.removeAt(index);

    WScriptBash * script = d->scripts.takeAt(index);

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

    foreach (WScriptBash * script, d->scripts)
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
    QVariantMap map = WScriptBash::resultToMap(result.bash);

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
