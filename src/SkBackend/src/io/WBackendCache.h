//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WBACKENDCACHE_H
#define WBACKENDCACHE_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

#ifndef SK_NO_BACKENDCACHE

// Forward declarations
class WBackendCachePrivate;
class WBackendUniversalScript;

class SK_BACKEND_EXPORT WBackendCache : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount NOTIFY maxCountChanged)

public:
    explicit WBackendCache(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE WBackendUniversalScript * getScript(const QString & name, const QString & data);

    Q_INVOKABLE void removeScripts(const QString & pattern);

signals:
    void maxCountChanged();

public: // Properties
    int  maxCount() const;
    void setMaxCount(int max);

private:
    W_DECLARE_PRIVATE(WBackendCache)
};

#endif // SK_NO_BACKENDCACHE
#endif // WBACKENDCACHE_H
