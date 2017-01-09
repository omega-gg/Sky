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

#ifndef WMODELCOMPLETIONGOOGLE_P_H
#define WMODELCOMPLETIONGOOGLE_P_H

// Qt includes
#include <QUrl>

// Sk includes
#include <private/Sk_p>

#ifndef SK_NO_MODELCOMPLETIONGOOGLE

class WModelCompletionGooglePrivate : public WPrivate
{
public:
    WModelCompletionGooglePrivate(WModelCompletionGoogle * p);

    void init();

public: // Functions
    void setLoading(bool loading);

public: // Slots
    void onLoaded(WRemoteData * data);

public: // Variables
    QUrl    url;
    QString query;

    QList<QString> listCompletion;

    WRemoteData * data;

    bool ready;
    bool loading;

protected:
    W_DECLARE_PUBLIC(WModelCompletionGoogle)
};

#endif // SK_NO_MODELCOMPLETIONGOOGLE
#endif // WMODELCOMPLETIONGOOGLE_P_H
