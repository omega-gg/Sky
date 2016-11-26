//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WDECLARATIVEITEM_H
#define WDECLARATIVEITEM_H

// Qt includes
#include <QDeclarativeItem>

// Sk includes
#include <Sk>

#ifndef SK_NO_DECLARATIVEITEM

// Forward declarations
class WDeclarativeItemPrivate;
class WView;

class SK_GUI_EXPORT WDeclarativeItem : public QDeclarativeItem, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(WView * view READ view NOTIFY viewChanged)

    Q_PROPERTY(QDeclarativeItem * viewport READ viewport NOTIFY viewChanged)

public:
    explicit WDeclarativeItem(QDeclarativeItem * parent = NULL);
protected:
    WDeclarativeItem(WDeclarativeItemPrivate * p, QDeclarativeItem * parent = NULL);

protected: // QGraphicsItem reimplementation
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);

signals:
    void viewChanged();

public: // Properties
    WView * view() const;

    QDeclarativeItem * viewport() const;

private:
    W_DECLARE_PRIVATE(WDeclarativeItem)
};

#endif // SK_NO_DECLARATIVEITEM
#endif // WDECLARATIVEITEM_H
