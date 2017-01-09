//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WCOOKIEJAR_P_H
#define WCOOKIEJAR_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QBasicTimer>
#include <QTime>

// Sk includes
#include <private/Sk_p>

#ifndef SK_NO_COOKIEJAR

class SK_CORE_EXPORT WCookieJarPrivate : public WPrivate
{
public:
    WCookieJarPrivate(WCookieJar * p);

    /* virtual */ ~WCookieJarPrivate();

    void init();

public: // Functions
    void load        ();
    void loadSettings();

    void save     ();
    void saveLater();
    void saveNow  ();

    void purgeCookies();

    QString getPath();

public: // Variables
    QString path;

    bool loaded;

    WCookieJar::PolicyAccept policyAccept;
    WCookieJar::PolicyKeep   policyKeep;

    QStringList cookiesBlocked;
    QStringList cookiesAllowed;
    QStringList cookiesSession;

    QBasicTimer timer;

    QTime time;

protected:
    W_DECLARE_PUBLIC(WCookieJar)
};

#endif // SK_NO_COOKIEJAR
#endif // WCOOKIEJAR_P_H
