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

#ifndef WDECLARATIVEIMAGESVG_P_H
#define WDECLARATIVEIMAGESVG_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QTimer>

// Sk includes
#include <private/WDeclarativeItem_p>

#ifndef SK_NO_DECLARATIVEIMAGESVG

// Forward declarations
class QSvgRenderer;
class WCacheFile;

//-------------------------------------------------------------------------------------------------
// WDeclarativeImageSvgPrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeImageSvgPrivate : public WDeclarativeItemPrivate
{
protected:
    WDeclarativeImageSvgPrivate(WDeclarativeImageSvg * p);

    void init();

public: // Functions
    QRectF getRect(qreal width, qreal height) const;

    void load       ();
    void loadVisible();

    void loadUrl();

    void applyUrl(const QUrl & url);

    void clearUrl(WDeclarativeImageSvg::Status status);

    void clearFile();

public: // Slots
    void onProgess(qint64 received, qint64 total);

    void onLoaded(WCacheFile * file);

    void onUpdate();

public: // Variables
    WCacheFile * file;

    QSvgRenderer * renderer;

    WDeclarativeImageSvg::Status status;

    QUrl url;

    QSize size;

    WDeclarativeImageSvg::LoadMode loadMode;
    WDeclarativeImageSvg::FillMode fillMode;

    bool loadLater : 1;

    qreal progress;

protected:
    W_DECLARE_PUBLIC(WDeclarativeImageSvg)
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeImageSvgScalePrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeImageSvgScalePrivate : public WDeclarativeImageSvgPrivate
{
protected:
    WDeclarativeImageSvgScalePrivate(WDeclarativeImageSvgScale * p);

    void init();

public: // Functions
    void restore();

public: // Slots
    void onScale();

public: // Variables
    QPixmap scalePixmap;
    QSize   scaleSize;

    bool scaling  : 1;
    bool scalable : 1;
    bool scaled   : 1;

    bool scaleDelayed : 1;
    int  scaleDelay;

    QTimer timer;

protected:
    W_DECLARE_PUBLIC(WDeclarativeImageSvgScale)
};

#endif // SK_NO_DECLARATIVEIMAGESVG
#endif // WDECLARATIVEIMAGESVG_P_H
