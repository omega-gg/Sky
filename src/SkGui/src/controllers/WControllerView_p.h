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

#ifndef WCONTROLLERVIEW_P_H
#define WCONTROLLERVIEW_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QSize>
#include <QList>

// Private includes
#include <private/WController_p>

#ifndef SK_NO_CONTROLLERVIEW

// Forward declarations
#ifdef QT_4
class QPainter;
class QGraphicsObject;
#endif
class WView;

class SK_GUI_EXPORT WControllerViewPrivate : public WControllerPrivate
{
public:
    WControllerViewPrivate(WControllerView * p);

    /* virtual */ ~WControllerViewPrivate();

    void init();

public: // Functions
    void registerView  (WView * view);
    void unregisterView(WView * view);

#ifdef QT_4
public: // Static functions
    static void paintRecursive(QPainter * painter, QGraphicsObject * item);
    static void paintChild    (QPainter * painter, QGraphicsObject * item);
#endif

public: // Variables
    QList<WView *> views;

    bool opengl;

    WControllerView::LoadMode loadMode;

    int scaleDelay;

protected:
    W_DECLARE_PUBLIC(WControllerView)
};

#endif // SK_NO_CONTROLLERVIEW
#endif // WCONTROLLERVIEW_P_H
