//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkMedia module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WBACKENDSUBTITLE_P_H
#define WBACKENDSUBTITLE_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/Sk_p>

#ifndef SK_NO_BACKENDSUBTITLE

// Forward declarations
class WLibraryItem;

class SK_MEDIA_EXPORT WBackendSubtitlePrivate : public WPrivate
{
public:
    WBackendSubtitlePrivate(WBackendSubtitle * p);

    void init();

public: // Functions
    WLibraryItem * getItem();

public: // Slots
    void onQueryData(const QByteArray & data, const QString & extension);

    void onQueryCompleted();

public: // Variables
    WLibraryItem * item;

    QString source;

    QString text;

protected:
    W_DECLARE_PUBLIC(WBackendSubtitle)
};

#endif // SK_NO_BACKENDSUBTITLE
#endif // WBACKENDSUBTITLE_P_H
