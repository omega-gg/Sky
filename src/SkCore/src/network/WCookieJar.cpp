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

#include "WCookieJar.h"

#ifndef SK_NO_COOKIEJAR

// Qt includes
#include <QCoreApplication>
#include <QDataStream>
#include <QNetworkCookie>
#include <QSettings>
#include <QMetaEnum>
#include <QDir>

// Sk includes
#include <WControllerFile>

//-------------------------------------------------------------------------------------------------
// Static variables

static const unsigned int COOKIEJAR_JAR_VERSION = 23;

static const int COOKIEJAR_TIMER =  5000;
static const int COOKIEJAR_WAIT  = 15000;

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

QDataStream & operator<<(QDataStream & stream, const QList<QNetworkCookie> & list)
{
    stream << COOKIEJAR_JAR_VERSION;

    stream << quint32(list.count());

    for (int i = 0; i < list.count(); ++i)
    {
        stream << list.at(i).toRawForm();
    }

    return stream;
}

QDataStream & operator>>(QDataStream & stream, QList<QNetworkCookie> & list)
{
    list.clear();

    quint32 version;

    stream >> version;

    if (version != COOKIEJAR_JAR_VERSION)
    {
        return stream;
    }

    quint32 count;

    stream >> count;

    for (quint32 i = 0; i < count; ++i)
    {
        QByteArray value;

        stream >> value;

        QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(value);

        if (cookies.count() == 0 && value.length())
        {
            qWarning("WCookieJar operator>>: Unable to parse cookies.");
        }

        for (int j = 0; j < cookies.count(); ++j)
        {
            list.append(cookies.at(j));
        }

        if (stream.atEnd()) break;
    }

    return stream;
}

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WCookieJar_p.h"

WCookieJarPrivate::WCookieJarPrivate(WCookieJar * p) : WPrivate(p) {}

/* virtual */ WCookieJarPrivate::~WCookieJarPrivate()
{
    if (policyKeep == WCookieJar::KeepUntilExit)
    {
        Q_Q(WCookieJar); q->clear();
    }

    if (timer.isActive()) saveNow();
}

//-------------------------------------------------------------------------------------------------

void WCookieJarPrivate::init()
{
    loaded = false;

    policyAccept = WCookieJar::AcceptOnlyFromSitesNavigatedTo;
    policyKeep   = WCookieJar::KeepUntilExpire;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WCookieJarPrivate::load()
{
    Q_Q(WCookieJar);

    qRegisterMetaTypeStreamOperators<QList<QNetworkCookie> >("QList<QNetworkCookie>");

    QSettings cookieSettings(getPath() + QLatin1String("/cookies.ini"), QSettings::IniFormat);

    QList<QNetworkCookie> cookies
        = qvariant_cast<QList<QNetworkCookie> >(cookieSettings.value(QLatin1String("cookies")));

    q->setAllCookies(cookies);

    cookieSettings.beginGroup(QLatin1String("Exceptions"));

    cookiesBlocked = cookieSettings.value(QLatin1String("block"))          .toStringList();
    cookiesAllowed = cookieSettings.value(QLatin1String("allow"))          .toStringList();
    cookiesSession = cookieSettings.value(QLatin1String("allowForSession")).toStringList();

    std::sort(cookiesBlocked.begin(), cookiesBlocked.end());
    std::sort(cookiesAllowed.begin(), cookiesAllowed.end());
    std::sort(cookiesSession.begin(), cookiesSession.end());

    loadSettings();
}

void WCookieJarPrivate::loadSettings()
{
    Q_Q(WCookieJar);

    QSettings settings;

    settings.beginGroup(QLatin1String("cookies"));

    int index = WCookieJar::staticMetaObject.indexOfEnumerator("PolicyAccept");

    QMetaEnum accept = WCookieJar::staticMetaObject.enumerator(index);

    QByteArray value
             = settings.value(QLatin1String("acceptCookies"),
                              QLatin1String("AcceptOnlyFromSitesNavigatedTo")).toByteArray();

    if (accept.keyToValue(value) == -1)
    {
         policyAccept = WCookieJar::AcceptOnlyFromSitesNavigatedTo;
    }
    else policyAccept = static_cast<WCookieJar::PolicyAccept> (accept.keyToValue(value));

    index = WCookieJar::staticMetaObject.indexOfEnumerator("PolicyKeep");

    QMetaEnum keep = WCookieJar::staticMetaObject.enumerator(index);

    value = settings.value(QLatin1String("keepCookiesUntil"),
                           QLatin1String("KeepUntilExpire")).toByteArray();

    if (keep.keyToValue(value) == -1)
    {
         policyKeep = WCookieJar::KeepUntilExpire;
    }
    else policyKeep = static_cast<WCookieJar::PolicyKeep> (keep.keyToValue(value));

    if (policyKeep == WCookieJar::KeepUntilExit)
    {
        q->setAllCookies(QList<QNetworkCookie>());
    }

    loaded = true;

    emit q->cookiesChanged();
}

//-------------------------------------------------------------------------------------------------

void WCookieJarPrivate::save()
{
    Q_Q(WCookieJar);

    purgeCookies();

    QString path = getPath();

    if (QFile::exists(path) == false)
    {
        QDir dir;

        dir.mkpath(path);
    }

    QSettings cookieSettings(path + QLatin1String("/cookies.ini"), QSettings::IniFormat);

    QList<QNetworkCookie> cookies = q->allCookies();

    for (int i = cookies.count() - 1; i >= 0; --i)
    {
        if (cookies.at(i).isSessionCookie())
        {
            cookies.removeAt(i);
        }
    }

    cookieSettings.setValue(QLatin1String("cookies"),
                            QVariant::fromValue<QList<QNetworkCookie> >(cookies));

    cookieSettings.beginGroup(QLatin1String("Exceptions"));

    cookieSettings.setValue(QLatin1String("block"),           cookiesBlocked);
    cookieSettings.setValue(QLatin1String("allow"),           cookiesAllowed);
    cookieSettings.setValue(QLatin1String("allowForSession"), cookiesSession);

    QSettings settings;

    settings.beginGroup(QLatin1String("cookies"));

    int index = WCookieJar::staticMetaObject.indexOfEnumerator("PolicyAccept");

    QMetaEnum accept = WCookieJar::staticMetaObject.enumerator(index);

    settings.setValue(QLatin1String("acceptCookies"),
                      QLatin1String(accept.valueToKey(policyAccept)));

    index = WCookieJar::staticMetaObject.indexOfEnumerator("PolicyKeep");

    QMetaEnum keep = WCookieJar::staticMetaObject.enumerator(index);

    settings.setValue(QLatin1String("keepCookiesUntil"),
                      QLatin1String(keep.valueToKey(policyKeep)));
}

void WCookieJarPrivate::saveLater()
{
    if (time.isNull()) time.start();

    if (time.elapsed() <= COOKIEJAR_WAIT)
    {
        Q_Q(WCookieJar);

        timer.start(COOKIEJAR_TIMER, q);
    }
    else if (timer.isActive() == false)
    {
        saveNow();
    }
}

void WCookieJarPrivate::saveNow()
{
    timer.stop();

    time = QTime();

    save();
}

//-------------------------------------------------------------------------------------------------

void WCookieJarPrivate::purgeCookies()
{
    Q_Q(WCookieJar);

    QList<QNetworkCookie> cookies = q->allCookies();

    if (cookies.isEmpty()) return;

    int oldCount = cookies.count();

    QDateTime now = QDateTime::currentDateTime();

    for (int i = cookies.count() - 1; i >= 0; --i)
    {
        if (cookies.at(i).isSessionCookie() == false && cookies.at(i).expirationDate() < now)
        {
            cookies.removeAt(i);
        }
    }

    if (oldCount == cookies.count()) return;

    q->setAllCookies(cookies);

    emit q->cookiesChanged();
}

//-------------------------------------------------------------------------------------------------

QString WCookieJarPrivate::getPath()
{
    if (path.isEmpty())
    {
         return WControllerFile::pathWritable();
    }
    else return path;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WCookieJar::WCookieJar(const QString & path, QObject * parent)
    : QNetworkCookieJar(parent), WPrivatable(new WCookieJarPrivate(this))
{
    Q_D(WCookieJar);

    d->init();

    d->path = path;
}

/* explicit */ WCookieJar::WCookieJar(QObject * parent)
    : QNetworkCookieJar(parent), WPrivatable(new WCookieJarPrivate(this))
{
    Q_D(WCookieJar); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WCookieJar::clear()
{
    if (allCookies().isEmpty()) return;

    Q_D(WCookieJar);

    setAllCookies(QList<QNetworkCookie>());

    d->saveLater();

    emit cookiesChanged();
}

//-------------------------------------------------------------------------------------------------
// QNetworkCookieJar reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QList<QNetworkCookie> WCookieJar::cookiesForUrl(const QString & url) const
{
    Q_D(const WCookieJar);

    if (d->loaded == false)
    {
        const_cast<WCookieJarPrivate *> (d)->load();
    }

    return QNetworkCookieJar::cookiesForUrl(url);
}

/* virtual */ bool WCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> & cookies,
                                                 const QString               & url)
{
    Q_D(WCookieJar);

    if (d->loaded == false) d->load();

    QString host = QUrl(url).host();

    bool block;

    if (qBinaryFind(d->cookiesBlocked.begin(),
                    d->cookiesBlocked.end  (), host) == d->cookiesBlocked.end())
    {
         block = false;
    }
    else block = true;

    bool allow;

    if (qBinaryFind(d->cookiesAllowed.begin(),
                    d->cookiesAllowed.end  (), host) == d->cookiesAllowed.end())
    {
         allow = false;
    }
    else allow = true;

    bool session;

    if (qBinaryFind(d->cookiesSession.begin(),
                    d->cookiesSession.end  (), host) == d->cookiesSession.end())
    {
         session = false;
    }
    else session = true;

    bool accept;

    if (d->policyAccept == AcceptNever)
    {
         accept = false;
    }
    else accept = true;

    bool added = false;

    if ((accept && block == false) || (accept == false && (allow || session)))
    {
        QDateTime soon = QDateTime::currentDateTime();

        soon = soon.addDays(90);

        foreach (QNetworkCookie cookie, cookies)
        {
            QList<QNetworkCookie> list;

            if (d->policyKeep == KeepUntilTimeLimit
                &&
                cookie.isSessionCookie() == false && cookie.expirationDate() > soon)
            {
                cookie.setExpirationDate(soon);
            }

            list.append(cookie);

            if (QNetworkCookieJar::setCookiesFromUrl(list, url) == false)
            {
                if (d->policyAccept == AcceptAlways)
                {
                    QList<QNetworkCookie> list = allCookies();

                    list.append(cookie);

                    setAllCookies(list);

                    added = true;
                }
            }
            else added = true;
        }
    }

    if (added)
    {
        d->saveLater();

        emit cookiesChanged();
    }

    return added;
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WCookieJar::timerEvent(QTimerEvent *)
{
    Q_D(WCookieJar); d->saveNow();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WCookieJar::PolicyAccept WCookieJar::policyAccept() const
{
    Q_D(const WCookieJar);

    if (d->loaded == false)
    {
        const_cast<WCookieJarPrivate *> (d)->load();
    }

    return d->policyAccept;
}

void WCookieJar::setPolicyAccept(PolicyAccept accept)
{
    Q_D(WCookieJar);

    if (d->loaded == false) d->load();

    if (d->policyAccept == accept) return;

    d->policyAccept = accept;

    d->saveLater();
}

//-------------------------------------------------------------------------------------------------

WCookieJar::PolicyKeep WCookieJar::policyKeep() const
{
    Q_D(const WCookieJar);

    if (d->loaded == false)
    {
        const_cast<WCookieJarPrivate *> (d)->load();
    }

    return d->policyKeep;
}

void WCookieJar::setPolicyKeep(PolicyKeep keep)
{
    Q_D(WCookieJar);

    if (d->loaded == false) d->load();

    if (d->policyKeep == keep) return;

    d->policyKeep = keep;

    d->saveLater();
}

//-------------------------------------------------------------------------------------------------

QStringList WCookieJar::cookiesBlocked() const
{
    Q_D(const WCookieJar);

    if (d->loaded == false)
    {
        const_cast<WCookieJarPrivate *> (d)->load();
    }

    return d->cookiesBlocked;
}

void WCookieJar::setCookiesBlocked(const QStringList & cookies)
{
    Q_D(WCookieJar);

    if (d->loaded == false) d->load();

    d->cookiesBlocked = cookies;

    std::sort(d->cookiesBlocked.begin(), d->cookiesBlocked.end());

    d->saveLater();
}

//-------------------------------------------------------------------------------------------------

QStringList WCookieJar::cookiesAllowed() const
{
    Q_D(const WCookieJar);

    if (d->loaded == false)
    {
        const_cast<WCookieJarPrivate *> (d)->load();
    }

    return d->cookiesAllowed;
}

void WCookieJar::setCookiesAllowed(const QStringList & cookies)
{
    Q_D(WCookieJar);

    if (d->loaded == false) d->load();

    d->cookiesAllowed = cookies;

    std::sort(d->cookiesAllowed.begin(), d->cookiesAllowed.end());

    d->saveLater();
}

//-------------------------------------------------------------------------------------------------

QStringList WCookieJar::cookiesSession() const
{
    Q_D(const WCookieJar);

    if (d->loaded == false)
    {
        const_cast<WCookieJarPrivate *> (d)->load();
    }

    return d->cookiesSession;
}

void WCookieJar::setCookiesSession(const QStringList & cookies)
{
    Q_D(WCookieJar);

    if (d->loaded == false) d->load();

    d->cookiesSession = cookies;

    std::sort(d->cookiesSession.begin(), d->cookiesSession.end());

    d->saveLater();
}

#endif // SK_NO_COOKIEJAR
