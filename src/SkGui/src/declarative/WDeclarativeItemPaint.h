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

#ifndef WDECLARATIVEITEMPAINT_H
#define WDECLARATIVEITEMPAINT_H

// Qt includes
#include <QQuickPaintedItem>

// Sk includes
#include <Sk>

#ifndef SK_NO_DECLARATIVEITEMPAINT

// Forward declarations
class WDeclarativeItemPaintPrivate;
class WView;

class SK_GUI_EXPORT WDeclarativeItemPaint : public QQuickPaintedItem, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(WView * view READ view NOTIFY viewChanged)

    Q_PROPERTY(bool frameBuffer READ frameBuffer WRITE setFrameBuffer NOTIFY frameBufferChanged)

public:
    explicit WDeclarativeItemPaint(QQuickItem * parent = NULL);
protected:
    WDeclarativeItemPaint(WDeclarativeItemPaintPrivate * p, QQuickItem * parent = NULL);

protected: // QQuickItem reimplementation
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & value);

signals:
    void viewChanged();

    void frameBufferChanged();

public: // Properties
    WView * view() const;

    bool frameBuffer() const;
    void setFrameBuffer(bool enabled);

private:
    W_DECLARE_PRIVATE(WDeclarativeItemPaint)
};

#endif // SK_NO_DECLARATIVEITEMPAINT
#endif // WDECLARATIVEITEMPAINT_H
