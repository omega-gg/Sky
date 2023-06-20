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

#ifndef WABSTRACTLOADER_H
#define WABSTRACTLOADER_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

#ifndef SK_NO_ABSTRACTLOADER

// Forward declarations
class QIODevice;
class WAbstractLoaderPrivate;
class WRemoteData;

class SK_CORE_EXPORT WAbstractLoader : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(Scope)

    Q_PROPERTY(int maxRedirect READ maxRedirect WRITE setMaxRedirect NOTIFY maxRedirectChanged)

public: // Enums
    enum Scope
    {
        ScopeDefault,
        ScopeText
    };

public:
    explicit WAbstractLoader(QObject * parent = NULL);
protected:
    WAbstractLoader(WAbstractLoaderPrivate * p, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void get(WRemoteData * data);

protected: // Abstract functions
    virtual QIODevice * load(WRemoteData * data) = 0;

protected: // Virtual functions
    virtual void abort(QIODevice * reply); // {}

protected: // Functions
    WRemoteData * getData(QIODevice * reply) const;

    void redirect(QIODevice * reply, const QString & url);
    void complete(QIODevice * reply);

    void applyUrl(WRemoteData * data, const QString & url);

    void setError(WRemoteData * data, const QString & error);

signals:
    void maxRedirectChanged();

public: // Properties
    int  maxRedirect() const;
    void setMaxRedirect(int max);

private:
    W_DECLARE_PRIVATE(WAbstractLoader)

    friend class WRemoteData;
};

#endif // SK_NO_ABSTRACTLOADER
#endif // WABSTRACTLOADER_H
