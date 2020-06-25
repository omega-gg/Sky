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

#ifndef WVIEWDRAG_H
#define WVIEWDRAG_H

// Sk includes
#include <WDeclarativeMouseArea>

#ifndef SK_NO_VIEWDRAG

// Forward declarations
class WViewDragPrivate;

class SK_GUI_EXPORT WViewDrag : public WDeclarativeMouseArea
{
    Q_OBJECT

    Q_PROPERTY(bool dragEnabled READ dragEnabled WRITE setDragEnabled NOTIFY dragEnabledChanged)

public:
#ifdef QT_4
    explicit WViewDrag(QDeclarativeItem * parent = NULL);
#else
    explicit WViewDrag(QQuickItem * parent = NULL);
#endif

protected: // Events
#ifdef QT_4
    /* virtual */ void mousePressEvent  (QGraphicsSceneMouseEvent * event);
    /* virtual */ void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

    /* virtual */ void mouseMoveEvent(QGraphicsSceneMouseEvent * event);

    /* virtual */ bool sceneEvent(QEvent * event);
#else
    /* virtual */ void mousePressEvent  (QMouseEvent * event);
    /* virtual */ void mouseReleaseEvent(QMouseEvent * event);

    /* virtual */ void mouseMoveEvent(QMouseEvent * event);

    /* virtual */ void mouseUngrabEvent();
#endif

signals:
    void dragEnabledChanged(bool enabled);

public: // Properties
    bool dragEnabled() const;
    void setDragEnabled(bool enabled);

private:
    W_DECLARE_PRIVATE(WViewDrag)
};

#endif // SK_NO_VIEWDRAG
#endif // WVIEWDRAG_H
