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
#ifdef QT_LATEST
#include <QPixmap>
#endif
#include <QTimer>

// Private includes
#ifdef QT_4
#include <private/WDeclarativeItem_p>
#else
#include <private/WDeclarativeTexture_p>
#endif

#ifndef SK_NO_DECLARATIVEIMAGESVG

// Forward declarations
class QSvgRenderer;
class WCacheFile;

//-------------------------------------------------------------------------------------------------
// WDeclarativeImageSvgPrivate
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeImageSvgPrivate : public WDeclarativeItemPrivate
#else
class SK_GUI_EXPORT WDeclarativeImageSvgPrivate : public WDeclarativeTexturePrivate
#endif
{
public:
    WDeclarativeImageSvgPrivate(WDeclarativeImageSvg * p);

    void init();

public: // Functions
#ifdef QT_LATEST
    void updatePixmap(int width, int height);
#endif

    void load       ();
    void loadVisible();

    void loadUrl();

    void applyUrl(const QString & url);

    void clearUrl(WDeclarativeImageSvg::Status status);

    void clearFile();

    QRectF getRect(qreal width, qreal height) const;

public: // Slots
    void onProgress(qint64 received, qint64 total);

    void onLoaded(WCacheFile * file);

#ifdef QT_4
    void onUpdate();
#else
    void onTimeout();
#endif

public: // Variables
#ifdef QT_LATEST
    QPixmap pixmap;
#endif

    WCacheFile * file;

    QSvgRenderer * renderer;

    WDeclarativeImageSvg::Status status;

    QString url;

    QSize size;

    WDeclarativeImageSvg::LoadMode loadMode;
    WDeclarativeImageSvg::FillMode fillMode;

    bool loadLater : 1;

    qreal progress;

#ifdef QT_LATEST
    bool scaleDelayed : 1;
    int  scaleDelay;

    bool scaleLater : 1;

    QTimer timer;
#endif

protected:
    W_DECLARE_PUBLIC(WDeclarativeImageSvg)
};

#ifdef QT_4

//-------------------------------------------------------------------------------------------------
// WDeclarativeImageSvgScalePrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeImageSvgScalePrivate : public WDeclarativeImageSvgPrivate
{
public:
    WDeclarativeImageSvgScalePrivate(WDeclarativeImageSvgScale * p);

    void init();

public: // Functions
    void update ();
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

#endif

#endif // SK_NO_DECLARATIVEIMAGESVG
#endif // WDECLARATIVEIMAGESVG_P_H
