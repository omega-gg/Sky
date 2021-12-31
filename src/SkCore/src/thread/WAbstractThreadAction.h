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

#ifdef QT_6
Q_MOC_INCLUDE("WAbstractThreadReply")
#endif

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
