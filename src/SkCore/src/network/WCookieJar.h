//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WCOOKIEJAR_H
#define WCOOKIEJAR_H

// Qt includes
#include <QNetworkCookieJar>
#include <QStringList>

// Sk includes
#include <Sk>

#ifndef SK_NO_COOKIEJAR

class WCookieJarPrivate;

class SK_CORE_EXPORT WCookieJar : public QNetworkCookieJar, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(PolicyAccept)
    Q_ENUMS(PolicyKeep)

    Q_PROPERTY(PolicyAccept policyAccept READ policyAccept WRITE setPolicyAccept)
    Q_PROPERTY(PolicyKeep   policyKeep   READ policyKeep   WRITE setPolicyKeep)

    Q_PROPERTY(QStringList cookiesBlocked READ cookiesBlocked WRITE setCookiesBlocked)
    Q_PROPERTY(QStringList cookiesAllowed READ cookiesAllowed WRITE setCookiesAllowed)
    Q_PROPERTY(QStringList cookiesSession READ cookiesSession WRITE setCookiesSession)

public: // Enums
    enum PolicyAccept
    {
        AcceptAlways,
        AcceptNever,
        AcceptOnlyFromSitesNavigatedTo
    };

    enum PolicyKeep
    {
        KeepUntilExpire,
        KeepUntilExit,
        KeepUntilTimeLimit
    };

public:
    WCookieJar(const QString & path, QObject * parent = NULL);

    explicit WCookieJar(QObject * parent = NULL);

public: // Interface
    void clear();

public: // QNetworkCookieJar reimplementation
    /* virtual */ QList<QNetworkCookie> cookiesForUrl(const QUrl & url) const;

    /* virtual */ bool setCookiesFromUrl(const QList<QNetworkCookie> & cookies,
                                         const QUrl                  & url);

signals:
    void cookiesChanged();

protected: // Events
    /* virtual */ void timerEvent(QTimerEvent * event);

public: // Properties
    PolicyAccept policyAccept() const;
    void         setPolicyAccept(PolicyAccept accept);

    PolicyKeep policyKeep() const;
    void       setPolicyKeep(PolicyKeep keep);

    QStringList cookiesBlocked() const;
    void        setCookiesBlocked(const QStringList & cookies);

    QStringList cookiesAllowed() const;
    void        setCookiesAllowed(const QStringList & cookies);

    QStringList cookiesSession() const;
    void        setCookiesSession(const QStringList & cookies);

private:
    W_DECLARE_PRIVATE(WCookieJar)
};

#endif // SK_NO_COOKIEJAR
#endif // WCOOKIEJAR_H
