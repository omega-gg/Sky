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

#ifndef WDECLARATIVEIMAGEBASE_P_H
#define WDECLARATIVEIMAGEBASE_P_H

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

// Sk includes
#include <WPixmapCache>

// Private includes
#ifdef QT_4
#include <private/WDeclarativeItem_p>
#else
#include <private/WDeclarativeItemPaint_p>
#endif

#ifndef SK_NO_DECLARATIVEIMAGEBASE

// Forward declarations
class WCacheFile;
class WImageFilter;

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeImageBasePrivate : public WDeclarativeItemPrivate
#else
class SK_GUI_EXPORT WDeclarativeImageBasePrivate : public WDeclarativeItemPaintPrivate
#endif
{
protected:
    WDeclarativeImageBasePrivate(WDeclarativeImageBase * p);

    void init();

public: // Functions
    void loadVisible();

    void loadUrl();

    void loadDefault();
    void readDefault();

    void reload();

    void applyRequest();
    void applyFilter ();

    void setSourceDefault(bool sourceDefault);

    void applySourceDefault();

    void setExplicitSize(bool explicitSize);

    void clearFile();

public: // Slots
    void onLoaded(WCacheFile * file);

    void onFilterUpdated();

public: // Variables
    WCacheFile * file;

    WPixmapCache pix;

    QPixmap pixmapDefault;

    WDeclarativeImageBase::Status status;

    QUrl url;
    QUrl urlDefault;

    QSize sourceSize;
    QSize sourceArea;

    WDeclarativeImageBase::LoadMode loadMode;

    bool asynchronous;

    qreal progress;

    WImageFilter * filter;

    bool sourceDefault: 1;

    bool explicitSize: 1;

    bool loadLater: 1;

    bool cache : 1;

    bool smooth : 1;

protected:
    W_DECLARE_PUBLIC(WDeclarativeImageBase)
};

#endif // SK_NO_DECLARATIVEIMAGEBASE
#endif // WDECLARATIVEIMAGEBASE_P_H
