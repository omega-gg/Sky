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

#ifndef WMAINVIEWRESIZER_H
#define WMAINVIEWRESIZER_H

// Sk includes
#include <WDeclarativeItem>

#ifndef SK_NO_MAINVIEWRESIZER

class WMainViewResizerPrivate;

class SK_GUI_EXPORT WMainViewResizer : public WDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)

public:
    explicit WMainViewResizer(QDeclarativeItem * parent = NULL);

protected: // QDeclarativeItem reimplementation
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);

signals:
    void sizeChanged();

public: // Properties
    int  size() const;
    void setSize(int size);

private:
    W_DECLARE_PRIVATE(WMainViewResizer)
};

#endif // SK_NO_MAINVIEWRESIZER
#endif // WMAINVIEWRESIZER_H
