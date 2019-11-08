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

#ifndef WBACKENDOPENSUBTITLES_H
#define WBACKENDOPENSUBTITLES_H

// Sk includes
#include <WBackendNet>

#ifndef SK_NO_BACKENDOPENSUBTITLES

// Forward declarations
class WBackendOpenSubtitlesPrivate;

class SK_BACKEND_EXPORT WBackendOpenSubtitles : public WBackendNet
{
    Q_OBJECT

public:
    WBackendOpenSubtitles();

public: // WBackendNet implementation
    /* Q_INVOKABLE virtual */ QString getId   () const;
    /* Q_INVOKABLE virtual */ QString getTitle() const;

public: // WBackendNet reimplementation
    /* Q_INVOKABLE virtual */ QString validate() const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */ WBackendNetQuery getQueryItem(const QString & url) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */
    WBackendNetQuery createQuery(const QString & method,
                                 const QString & label, const QString & q) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */
    WBackendNetFolder extractFolder(const QByteArray       & data,
                                    const WBackendNetQuery & query) const;

    /* Q_INVOKABLE virtual */
    WBackendNetItem extractItem(const QByteArray       & data,
                                const WBackendNetQuery & query) const;

private:
    W_DECLARE_PRIVATE(WBackendOpenSubtitles)
};

#endif // SK_NO_BACKENDOPENSUBTITLES
#endif // WBACKENDOPENSUBTITLES_H
