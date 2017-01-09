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
    WResizer(ResizeType type, QDeclarativeItem * parent = NULL);

protected: // Events
    /* virtual */ void mousePressEvent(QGraphicsSceneMouseEvent * event);
    /* virtual */ void mouseMoveEvent (QGraphicsSceneMouseEvent * event);

private:
    W_DECLARE_PRIVATE(WResizer)
};

#endif // SK_NO_RESIZER
#endif // WRESIZER_H
