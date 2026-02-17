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
// Private
//-------------------------------------------------------------------------------------------------

WBashManagerPrivate::WBashManagerPrivate(WBashManager * p) : WPrivate(p) {}

void WBashManagerPrivate::init() {}

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

    if (index == -1) return;

    scripts.removeAt(index);

    WBashManagerResult manager;

    manager.id = ids.takeAt(index);

    manager.bash = result;

    emit q->finished(manager);
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

    result.id   = id;
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

    script->stop();

    delete script;

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

#endif // SK_NO_BASHMANAGER
