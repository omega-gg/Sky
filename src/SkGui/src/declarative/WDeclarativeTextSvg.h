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

#ifndef WDECLARATIVETEXTSVG_H
#define WDECLARATIVETEXTSVG_H

// Sk includes
#include <WDeclarativeItem>

#ifndef SK_NO_DECLARATIVETEXTSVG

class WDeclarativeTextSvgPrivate;

class SK_GUI_EXPORT WDeclarativeTextSvg : public WDeclarativeItem
{
    Q_OBJECT

public:
    explicit WDeclarativeTextSvg(QDeclarativeItem * parent = NULL);

public: // QGraphicsItem reimplementation
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);

private:
    W_DECLARE_PRIVATE(WDeclarativeTextSvg)

    Q_PRIVATE_SLOT(d_func(), void onRepaintNeeded())
};

QML_DECLARE_TYPE(WDeclarativeTextSvg)

#include <private/WDeclarativeTextSvg_p>

#endif // SK_NO_DECLARATIVETEXTSVG
#endif // WDECLARATIVETEXTSVG_H
