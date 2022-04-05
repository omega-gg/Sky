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

#ifndef WDECLARATIVEITEM_H
#define WDECLARATIVEITEM_H

// Qt includes
#ifdef QT_4
#include <QDeclarativeItem>
#else
#include <QQuickItem>
#endif

// Sk includes
#include <Sk>

#ifndef SK_NO_DECLARATIVEITEM

// Forward declarations
class WDeclarativeItemPrivate;
class WView;

#ifdef QT_6
Q_MOC_INCLUDE("WView")
#endif

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeItem : public QDeclarativeItem, public WPrivatable
#else
class SK_GUI_EXPORT WDeclarativeItem : public QQuickItem, public WPrivatable
#endif
{
    Q_OBJECT

    Q_PROPERTY(WView * view READ view NOTIFY viewChanged)

#ifdef QT_4
    Q_PROPERTY(QDeclarativeItem * viewport READ viewport NOTIFY viewChanged)
#endif

public:
#ifdef QT_4
    explicit WDeclarativeItem(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeItem(QQuickItem * parent = NULL);
#endif

protected:
#ifdef QT_4
    WDeclarativeItem(WDeclarativeItemPrivate * p, QDeclarativeItem * parent = NULL);
#else
    WDeclarativeItem(WDeclarativeItemPrivate * p, QQuickItem * parent = NULL);
#endif

protected: // Virtual functions
    virtual void updateRatioPixel(); // {}

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_4
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);
#else
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & value);
#endif

signals:
    void viewChanged();

public: // Properties
    WView * view() const;

#ifdef QT_4
    QDeclarativeItem * viewport() const;
#endif

private:
    W_DECLARE_PRIVATE(WDeclarativeItem)

    Q_PRIVATE_SLOT(d_func(), void onRatioChanged())
};

#include <private/WDeclarativeItem_p>

#endif // SK_NO_DECLARATIVEITEM
#endif // WDECLARATIVEITEM_H
