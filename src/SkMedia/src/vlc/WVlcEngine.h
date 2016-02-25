//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkMedia module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WVLCENGINE_H
#define WVLCENGINE_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

#ifndef SK_NO_VLCENGINE

class  WVlcEnginePrivate;
struct libvlc_instance_t;

class SK_MEDIA_EXPORT WVlcEngine : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(libvlc_instance_t * instance READ instance CONSTANT)

public:
    WVlcEngine(QThread * thread = NULL, QObject * parent = NULL);

protected: // Events
    bool event(QEvent * event);

public: // Properties
    libvlc_instance_t * instance() const;

private:
    W_DECLARE_PRIVATE(WVlcEngine)
};

#endif // SK_NO_VLCENGINE
#endif // WVLCENGINE_H
