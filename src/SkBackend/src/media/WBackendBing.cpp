//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendBing.h"

#ifndef SK_NO_BACKENDBING

#ifdef QT_LATEST
#include <QUrlQuery>
#endif

// Sk includes
#include <WControllerNetwork>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_BACKEND_EXPORT WBackendBingPrivate : public WBackendNetPrivate
{
public:
    WBackendBingPrivate(WBackendBing * p);

    void init();

protected:
    W_DECLARE_PUBLIC(WBackendBing)
};

//-------------------------------------------------------------------------------------------------

WBackendBingPrivate::WBackendBingPrivate(WBackendBing * p) : WBackendNetPrivate(p) {}

void WBackendBingPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendBing::WBackendBing() : WBackendNet(new WBackendBingPrivate(this))
{
    Q_D(WBackendBing); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendBing::getId() const
{
    return "bing";
}

/* Q_INVOKABLE virtual */ QString WBackendBing::getTitle() const
{
    return "Bing";
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendBing::checkValidUrl(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    return source.startsWith("bing.com");
}

#endif // SK_NO_BACKENDBING
