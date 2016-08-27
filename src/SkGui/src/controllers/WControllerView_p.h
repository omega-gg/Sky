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

#ifndef WCONTROLLERVIEW_P_H
#define WCONTROLLERVIEW_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/WController_p>

// Qt includes
#include <QSize>
#include <QList>

#ifndef SK_NO_CONTROLLERVIEW

// Forward declarations
class WMainView;
class QPainter;
class QGraphicsObject;

class SK_GUI_EXPORT WControllerViewPrivate : public WControllerPrivate
{
public:
    WControllerViewPrivate(WControllerView * p);

    /* virtual */ ~WControllerViewPrivate();

    void init();

public: // Functions
    void registerMainView  (WMainView * mainView);
    void unregisterMainView(WMainView * mainView);

    void paintRecursive(QPainter * painter, QGraphicsObject * item, bool forceVisible) const;
    void paintChild    (QPainter * painter, QGraphicsObject * item, bool forceVisible) const;
    void paintItem     (QPainter * painter, QGraphicsObject * item)                    const;

public: // Variables
    QList<WMainView *> mainViews;

    WControllerView::LoadMode loadMode;

    int scaleDelay;

protected:
    W_DECLARE_PUBLIC(WControllerView)
};

#endif // SK_NO_CONTROLLERVIEW
#endif // WCONTROLLERVIEW_P_H
