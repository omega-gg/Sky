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

#ifndef WRESIZER_H
#define WRESIZER_H

// Qt includes
#include <WDeclarativeMouseArea>

// Sk includes
#include <Sk>

#ifndef SK_NO_RESIZER

class WResizerPrivate;

class SK_GUI_EXPORT WResizer : public WDeclarativeMouseArea
{
    Q_OBJECT

public: // Enums
    enum ResizeType
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Left,
        Right,
        Top,
        Bottom
    };

public:
#ifdef QT_4
    WResizer(ResizeType type, QDeclarativeItem * parent = NULL);
#else
    WResizer(ResizeType type, QQuickItem * parent = NULL);
#endif

protected: // Events
#ifdef QT_4
    /* virtual */ void mousePressEvent(QGraphicsSceneMouseEvent * event);
    /* virtual */ void mouseMoveEvent (QGraphicsSceneMouseEvent * event);
#else
    /* virtual */ void mousePressEvent(QMouseEvent * event);
    /* virtual */ void mouseMoveEvent (QMouseEvent * event);
#endif

private:
    W_DECLARE_PRIVATE(WResizer)
};

#endif // SK_NO_RESIZER
#endif // WRESIZER_H
