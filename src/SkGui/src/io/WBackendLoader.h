//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WBACKENDLOADER_H
#define WBACKENDLOADER_H

// Qt includes
#include <QObject>
#include <QStringList>

// Sk includes
#include <Sk>

#ifndef SK_NO_BACKENDLOADER

// Forward declarations
class WBackendLoaderPrivate;
class WBackendNet;
class WLibraryFolder;

class SK_GUI_EXPORT WBackendLoader : public QObject, public WPrivatable
{
    Q_OBJECT

public:
    explicit WBackendLoader(QObject * parent = NULL);
protected:
    WBackendLoader(WBackendLoaderPrivate * p, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE WBackendNet * create(const QString & id) const;

    Q_INVOKABLE WBackendNet * match(const QString & url) const;

    Q_INVOKABLE WBackendNet * matchCover(const QString & label, const QString & q) const;

    Q_INVOKABLE QString idFromUrl(const QString & url) const;

    Q_INVOKABLE QString coverFromUrl(const QString & url) const;

public: // Static functions
    Q_INVOKABLE static void reloadBackends();

    Q_INVOKABLE static void clearCache();

    Q_INVOKABLE static int  getMaxCache();
    Q_INVOKABLE static void setMaxCache(int max);

public: // Virtual interface
    Q_INVOKABLE virtual bool checkId(const QString & id) const; // {}

    Q_INVOKABLE virtual void createFolderItems(WLibraryFolder * folder) const; // {}

    Q_INVOKABLE virtual QString coverFromId(const QString & id) const; // {}

protected: // Virtual functions
    Q_INVOKABLE virtual WBackendNet * createBackend(const QString & id) const; // {}

    Q_INVOKABLE virtual QString getId(const QString & url) const; // {}

    Q_INVOKABLE virtual QStringList getCoverIds() const; // {}

private: // Functions
    WBackendNet * createNow(const QString & id) const;

private:
    W_DECLARE_PRIVATE(WBackendLoader)

    friend class WControllerPlaylist;
};

#endif // SK_NO_BACKENDLOADER
#endif // WBACKENDLOADER_H
