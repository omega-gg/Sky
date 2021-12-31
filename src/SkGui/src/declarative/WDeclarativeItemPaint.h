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

#ifdef QT_6
Q_MOC_INCLUDE("WView")
#endif

class SK_GUI_EXPORT WDeclarativeItemPaint : public QQuickPaintedItem, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(WView * view READ view NOTIFY viewChanged)

public:
    explicit WDeclarativeItemPaint(QQuickItem * parent = NULL);

protected:
    WDeclarativeItemPaint(WDeclarativeItemPaintPrivate * p, QQuickItem * parent = NULL);

protected: // QQuickItem reimplementation
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & value);

signals:
    void viewChanged();

public: // Properties
    WView * view() const;

private:
    W_DECLARE_PRIVATE(WDeclarativeItemPaint)
};

#endif // SK_NO_DECLARATIVEITEMPAINT
#endif // WDECLARATIVEITEMPAINT_H
