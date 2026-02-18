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

#ifndef WBASHMANAGER_H
#define WBASHMANAGER_H

// Sk includes
#include <WScriptBash>

#ifndef SK_NO_BASHMANAGER

class WBashManagerPrivate;

//-------------------------------------------------------------------------------------------------
// WBashManagerResult
//-------------------------------------------------------------------------------------------------

struct WBashManagerResult
{
    WBashManagerResult(int id = -1, bool ok = false)
    {
        this->id = id;

        bash.ok = ok;
    }

    int id;

    WScriptBashResult bash;
};

//-------------------------------------------------------------------------------------------------
// WBashManager
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WBashManager : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(int maxJobs READ maxJobs WRITE setMaxJobs NOTIFY maxJobsChanged)

public:
    explicit WBashManager(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE WBashManagerResult run(const QString     & fileName,
                                       const QStringList & arguments = QStringList());

    Q_INVOKABLE bool stop(int id);

    Q_INVOKABLE bool clear();

public: // Static functions
    Q_INVOKABLE static QVariantMap resultToMap(const WBashManagerResult & result);

signals:
    void finished(const WBashManagerResult & map);

    void maxJobsChanged();

public: // Properties
    int  maxJobs() const;
    void setMaxJobs(int max);

private:
    W_DECLARE_PRIVATE(WBashManager)

    Q_PRIVATE_SLOT(d_func(), void onScriptFinished(const WScriptBashResult &))
};

#include <private/WBashManager_p>

#endif // SK_NO_BASHMANAGER
#endif // WBASHMANAGER_H
