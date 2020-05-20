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

#ifndef WABSTRACTTHREADACTION_H
#define WABSTRACTTHREADACTION_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

#ifndef SK_NO_ABSTRACTTHREADACTION

// Forward declarations
class WAbstractThreadActionPrivate;
class WAbstractThreadReply;

class SK_CORE_EXPORT WAbstractThreadAction : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(WAbstractThreadReply * reply READ reply NOTIFY replyChanged)

public:
    WAbstractThreadAction();
protected:
    /* virtual */ ~WAbstractThreadAction();

public: // Interface
    void start();

    void abortAndDelete();

protected: // Abstract functions
    virtual bool run() = 0;

protected: // Virtual functions
    virtual WAbstractThreadReply * createReply() const;

signals:
    void replyChanged();

protected: // Properties
    WAbstractThreadReply * reply() const;

private:
    W_DECLARE_PRIVATE(WAbstractThreadAction)

    friend class WThreadActions;
    friend class WThreadActionsPrivate;
    friend class WThreadActionsThread;
    friend class WAbstractThreadReply;
    friend class WAbstractThreadReplyPrivate;
};

#endif // SK_NO_ABSTRACTTHREADACTION
#endif // WABSTRACTTHREADACTION_H
