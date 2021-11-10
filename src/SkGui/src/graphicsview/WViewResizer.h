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

#ifndef WVIEWRESIZER_H
#define WVIEWRESIZER_H

// Sk includes
#include <WDeclarativeItem>

#ifndef SK_NO_VIEWRESIZER

class WViewResizerPrivate;

class SK_GUI_EXPORT WViewResizer : public WDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)

public:
#ifdef QT_4
    explicit WViewResizer(QDeclarativeItem * parent = NULL);
#else
    explicit WViewResizer(QQuickItem * parent = NULL);
#endif

protected: // QDeclarativeItem / QQuickItem reimplementation
#ifdef QT_OLD
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);
#else
    /* virtual */ void geometryChange(const QRectF & newGeometry, const QRectF & oldGeometry);
#endif

signals:
    void sizeChanged();

public: // Properties
    int  size() const;
    void setSize(int size);

private:
    W_DECLARE_PRIVATE(WViewResizer)
};

#endif // SK_NO_VIEWRESIZER
#endif // WVIEWRESIZER_H
