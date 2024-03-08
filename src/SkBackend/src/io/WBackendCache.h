//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkBackend.

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
    //---------------------------------------------------------------------------------------------
    // NOTE: These function should be called from the same thread to avoid removing scripts from
    //       the cache while using them in another thread.

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
