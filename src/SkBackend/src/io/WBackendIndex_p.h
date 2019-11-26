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

#ifndef WBACKENDINDEX_P_H
#define WBACKENDINDEX_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QMetaMethod>

// Private includes
#include <private/WBackendLoader_p>

#ifndef SK_NO_BACKENDINDEX

// Forward declarations
class WRemoteData;

//=================================================================================================
// WBackendIndexItem
//=================================================================================================

struct WBackendIndexItem
{
    QString id;
    QString title;

    QString version;

    QString validate;
};

//=================================================================================================
// WBackendIndexData
//=================================================================================================

struct WBackendIndexData
{
    QString source;

    QString api;
    QString version;

    QStringList backendSearch;
    QStringList backendTrack;
    QStringList backendCover;
    QStringList backendSubtitle;

    QList<WBackendIndexItem> backends;

    QHash<QString, const WBackendIndexItem *> hash;
};

//=================================================================================================
// WBackendIndexPrivate
//=================================================================================================

class SK_BACKEND_EXPORT WBackendIndexPrivate : public WBackendLoaderPrivate
{
public:
    WBackendIndexPrivate(WBackendIndex * p);

    void init(const QString & source);

public: // Functions
    void load();

public: // Events
    void onLoaded();

    void onData(const WBackendIndexData & data);

public: // Variables
    QThread * thread;

    WRemoteData * remote;

    WBackendIndexData data;

    QString url;

    QMetaMethod method;

protected:
    W_DECLARE_PUBLIC(WBackendIndex)
};

#endif // SK_NO_BACKENDINDEX
#endif // WBACKENDINDEX_P_H
