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
public:
    WBackendIndexData()
    {
        valid = true;
    }

public: // Variables
    bool valid;

    QString origin;

    QString api;
    QString version;

    QStringList backendsSearch;
    QStringList backendsTrack;
    QStringList backendsCover;
    QStringList backendsSubtitle;

    QList<WBackendIndexItem> backends;

    QHash<QString, QString> covers;

    QHash<QString, const WBackendIndexItem *> hash;
};

//=================================================================================================
// WBackendIndexRemote
//=================================================================================================

struct WBackendIndexFile
{
    QString id;
    QString name;
};

//=================================================================================================
// WBackendIndexPrivate
//=================================================================================================

class SK_BACKEND_EXPORT WBackendIndexPrivate : public WBackendLoaderPrivate
{
public:
    WBackendIndexPrivate(WBackendIndex * p);

    void init(const QString & url);

public: // Functions
    void load();

    void loadData(const QByteArray & array);

    void applyLoaded();

public: // Events
    void onLoad  ();
    void onUpdate();

    void onData(const WBackendIndexData & data);

    void onBackendUpdate();

    void onItemLoad(WRemoteData * data);

    void onActionComplete();

public: // Variables
    WRemoteData * remote;

    WBackendIndexData data;

    QString url;
    QString urlBase;

    QMetaMethod method;

    QHash<WRemoteData *, WBackendIndexFile> jobs;

    QList<QString> ids;

    bool loaded;

protected:
    W_DECLARE_PUBLIC(WBackendIndex)
};

#endif // SK_NO_BACKENDINDEX
#endif // WBACKENDINDEX_P_H
