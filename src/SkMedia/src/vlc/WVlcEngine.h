//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

#ifndef WVLCENGINE_H
#define WVLCENGINE_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

#ifndef SK_NO_VLCENGINE

class WVlcEnginePrivate;

class SK_MEDIA_EXPORT WVlcEngine : public QObject, public WPrivatable
{
    Q_OBJECT

public:
    WVlcEngine(QThread * thread = NULL, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void deleteInstance();

protected: // Events
    /* virtual */ bool event(QEvent * event);

private:
    W_DECLARE_PRIVATE(WVlcEngine)

    friend class WVlcPlayer;
    friend class WVlcPlayerPrivate;
};

#endif // SK_NO_VLCENGINE
#endif // WVLCENGINE_H
