//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WABSTRACTTHREADREPLY_H
#define WABSTRACTTHREADREPLY_H

// Qt includes
#include <QObject>
#include <QEvent>

// Sk includes
#include <Sk>

#ifndef SK_NO_ABSTRACTTHREADREPLY

// Forward declarations
class WAbstractThreadReplyPrivate;
class WAbstractThreadAction;

class SK_CORE_EXPORT WAbstractThreadReply : public QObject, public WPrivatable
{
    Q_OBJECT

protected:
    WAbstractThreadReply();

    /* virtual */ ~WAbstractThreadReply();

public: // Virtual interface
    virtual void postReply(WAbstractThreadAction * action, bool ok);

protected: // Virtual functions
    virtual void onCompleted(bool ok);

protected: // Events
    bool event(QEvent * event);

private:
    W_DECLARE_PRIVATE(WAbstractThreadReply)

    friend class WThreadActions;
    friend class WThreadActionsPrivate;
    friend class WAbstractThreadAction;
    friend class WAbstractThreadActionPrivate;
};

#endif // SK_NO_ABSTRACTTHREADREPLY
#endif // WABSTRACTTHREADREPLY_H
