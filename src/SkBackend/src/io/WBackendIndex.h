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

#ifndef WBACKENDINDEX_H
#define WBACKENDINDEX_H

// Sk includes
#include <WBackendLoader>

#ifndef SK_NO_BACKENDINDEX

// Forward declarations
class WBackendIndexPrivate;
class WLibraryFolder;

class SK_BACKEND_EXPORT WBackendIndex : public WBackendLoader
{
    Q_OBJECT

    Q_PROPERTY(bool isLoaded READ isLoaded NOTIFY loadedChanged)

public:
    WBackendIndex(const QString & url, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void update();
    Q_INVOKABLE void reload();

public: // WBackendLoader reimplementation
    /* Q_INVOKABLE virtual */ bool checkId(const QString & id) const;

    /* Q_INVOKABLE virtual */ void createFolderItems(WLibraryFolder * folder,
                                                     WLibraryItem::Type type) const;

    /* Q_INVOKABLE virtual */ QString searchId() const;

    /* Q_INVOKABLE virtual */ QString coverFromId(const QString & id) const;
    /* Q_INVOKABLE virtual */ QString hubFromId  (const QString & id) const;

protected: // WBackendLoader reimplementation
    /* Q_INVOKABLE virtual */ WBackendNet * createBackend(const QString & id) const;

    /* Q_INVOKABLE virtual */ void checkBackend(WBackendNet * backend) const;

    /* Q_INVOKABLE virtual */ QString getId(const QString & url) const;

    /* Q_INVOKABLE virtual */ QStringList getCoverIds() const;

signals:
    void loaded ();
    void updated();

    void backendUpdated(const QString & id);

    void loadedChanged();

public: // Properties
    bool isLoaded() const;

private:
    W_DECLARE_PRIVATE(WBackendIndex)

    Q_PRIVATE_SLOT(d_func(), void onLoad  ())
    Q_PRIVATE_SLOT(d_func(), void onUpdate())

    Q_PRIVATE_SLOT(d_func(), void onData(const WBackendIndexData &))

    Q_PRIVATE_SLOT(d_func(), void onBackendUpdate())

    Q_PRIVATE_SLOT(d_func(), void onItemLoad(WRemoteData *))

    Q_PRIVATE_SLOT(d_func(), void onActionComplete())
};

#include <private/WBackendIndex_p>

#endif // SK_NO_BACKENDINDEX
#endif // WBACKENDINDEX_H
