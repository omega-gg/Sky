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

#ifndef WRESIZER_P_H
#define WRESIZER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QRect>

// Private includes
#include <private/WDeclarativeMouseArea_p>

#ifndef SK_NO_RESIZER

class SK_GUI_EXPORT WResizerPrivate : public WDeclarativeMouseAreaPrivate
{
public:
    WResizerPrivate(WResizer * p);

    void init(WResizer::ResizeType type);

public: // Functions
    void startResize(QGraphicsSceneMouseEvent * event);
    void resize     ();

public: // Variables
    WResizer::ResizeType type;

    QRect resizeArea;

    QPoint cursorPos;
    QPoint lastPos;

    bool resizing;

protected:
    W_DECLARE_PUBLIC(WResizer)
};

#endif // SK_NO_RESIZER
#endif // WRESIZER_P_H
