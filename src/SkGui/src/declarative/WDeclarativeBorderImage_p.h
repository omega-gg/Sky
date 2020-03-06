//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WDECLARATIVEBORDERIMAGE_P_H
#define WDECLARATIVEBORDERIMAGE_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QTimer>

// Private includes
#include <private/WDeclarativeImageBase_p>

#ifndef SK_NO_DECLARATIVEBORDERIMAGE

//-------------------------------------------------------------------------------------------------
// WDeclarativeBorderImagePrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeBorderImagePrivate : public WDeclarativeImageBasePrivate
{
public:
    WDeclarativeBorderImagePrivate(WDeclarativeBorderImage * p);

    void init();

public: // Functions
    WDeclarativeBorderGrid * getBorder();

public: // Slots
    virtual void onUpdate();

public: // Variables
    WDeclarativeBorderGrid * border;

    WDeclarativeBorderImage::TileMode horizontalTileMode;
    WDeclarativeBorderImage::TileMode verticalTileMode;

    QMargins margins;

protected:
    W_DECLARE_PUBLIC(WDeclarativeBorderImage)
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeBorderImageScalePrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeBorderImageScalePrivate : public WDeclarativeBorderImagePrivate
{
public:
    WDeclarativeBorderImageScalePrivate(WDeclarativeBorderImageScale * p);

    void init();

public: // Functions
    void resize(const QPixmap & pixmap);

    void update ();
    void restore();

    void abortAction();

public: // Slots
    /* virtual */ void onUpdate();

#ifdef QT_LATEST
    void onStart();
#endif

    void onScale();

    void onLoaded(const QImage & image);

public: // Variables
    WAbstractThreadAction * action;

    QPixmap  scalePixmap;
    QSize    scaleSize;
    QSize    scaleResize;
    QMargins scaleMargins;

    bool scaling  : 1;
    bool scalable : 1;
    bool scaled   : 1;

    bool scaleDelayed : 1;
    int  scaleDelay;

    QTimer timer;

protected:
    W_DECLARE_PUBLIC(WDeclarativeBorderImageScale)
};

#endif // SK_NO_DECLARATIVEBORDERIMAGE
#endif // WDECLARATIVEBORDERIMAGE_P_H
