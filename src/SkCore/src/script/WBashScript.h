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

#ifndef WBASHSCRIPT_H
#define WBASHSCRIPT_H

// Qt includes
#include <QObject>
#include <QVariantMap>
#ifdef QT_4
#include <QStringList>
#endif

// Sk includes
#include <Sk>

#ifndef SK_NO_BASHSCRIPT

class WBashScriptPrivate;

//-------------------------------------------------------------------------------------------------
// WBashScriptResult
//-------------------------------------------------------------------------------------------------

struct WBashScriptResult
{
    WBashScriptResult(bool ok = false) { this->ok = ok; }

    bool ok;

    QString output;
    QString outputError;
};

//-------------------------------------------------------------------------------------------------
// WBashScript
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WBashScript : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(bool isRunning READ isRunning NOTIFY runningChanged)

    // NOTE: This is useful to override the bash default path.
    Q_PROPERTY(QString pathBash READ pathBash WRITE setPathBash NOTIFY pathBashChanged)

public:
    explicit WBashScript(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE WBashScriptResult run(const QString     & fileName,
                                      const QStringList & arguments    = QStringList(),
                                      bool                asynchronous = true);

    Q_INVOKABLE void stop();

public: // Static functions
    Q_INVOKABLE static QString findBash();

    Q_INVOKABLE static QString quote(const QString & string);

    Q_INVOKABLE static QVariantMap resultToMap(const WBashScriptResult & result);

signals:
    void finished(const WBashScriptResult & result);

    void runningChanged();

    void pathBashChanged();

public: // Properties
    bool isRunning() const;

    QString pathBash() const;
    void    setPathBash(const QString & path);

private:
    W_DECLARE_PRIVATE(WBashScript)

    Q_PRIVATE_SLOT(d_func(), void onFinished(int, QProcess::ExitStatus))

    Q_PRIVATE_SLOT(d_func(), void onOutput     ())
    Q_PRIVATE_SLOT(d_func(), void onOutputError())
};

#include <private/WBashScript_p>

#endif // SK_NO_BASHSCRIPT
#endif // WBASHSCRIPT_H
