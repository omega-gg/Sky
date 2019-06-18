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

#ifndef WBACKENDUNIVERSAL_H
#define WBACKENDUNIVERSAL_H

// Sk includes
#include <WBackendNet>

#ifndef SK_NO_BACKENDUNIVERSAL

// Forward declarations
class WBackendUniversalPrivate;

class SK_BACKEND_EXPORT WBackendUniversal : public WBackendNet
{
    Q_OBJECT

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

public:
    WBackendUniversal();

signals:
    void sourceChanged();

public: // Properties
    QString source() const;
    void    setSource(const QString & source);

private:
    W_DECLARE_PRIVATE(WBackendUniversal)
};

#endif // SK_NO_BACKENDUNIVERSAL
#endif // WBACKENDUNIVERSAL_H
