//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#ifndef WMODELCOMPLETIONGOOGLE_P_H
#define WMODELCOMPLETIONGOOGLE_P_H

// Qt includes
#include <QUrl>
#ifdef QT_4
#include <QStringList>
#endif

// Private includes
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

    QStringList listCompletion;

    WRemoteData * data;

    bool ready;
    bool loading;

protected:
    W_DECLARE_PUBLIC(WModelCompletionGoogle)
};

#endif // SK_NO_MODELCOMPLETIONGOOGLE
#endif // WMODELCOMPLETIONGOOGLE_P_H
