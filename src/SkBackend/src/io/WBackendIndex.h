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

public:
    WBackendIndex(const QString & url, QObject * parent = NULL);

public: // WBackendLoader reimplementation
    /* Q_INVOKABLE virtual */ void createFolderItems(WLibraryFolder * folder) const;

protected: // WBackendLoader reimplementation
    /* Q_INVOKABLE virtual */ WBackendNet * createBackend(const QString & id) const;

    /* Q_INVOKABLE virtual */ QString matchBackend(const QString & source) const;

    /* Q_INVOKABLE virtual */ QStringList getCoverIds() const;

signals:
    void loaded();

private:
    W_DECLARE_PRIVATE(WBackendIndex)

    Q_PRIVATE_SLOT(d_func(), void onLoaded())

    Q_PRIVATE_SLOT(d_func(), void onData(const WBackendIndexData & data))
};

#include <private/WBackendIndex_p>

#endif // SK_NO_BACKENDINDEX
#endif // WBACKENDINDEX_H
