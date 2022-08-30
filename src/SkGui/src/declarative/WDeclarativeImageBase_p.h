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
#ifdef QT_NEW
#include <QPixmap>
#endif

// Sk includes
#include <WPixmapCache>

// Private includes
#ifdef QT_4
#include <private/WDeclarativeItem_p>
#else
#include <private/WDeclarativeTexture_p>
#endif

#ifndef SK_NO_DECLARATIVEIMAGEBASE

// Forward declarations
#ifdef QT_NEW
class QSGContext;
class QSGTexture;
#endif
class WCacheFile;

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeImageBasePrivate : public WDeclarativeItemPrivate
#else
class SK_GUI_EXPORT WDeclarativeImageBasePrivate : public WDeclarativeTexturePrivate
#endif
{
public:
    WDeclarativeImageBasePrivate(WDeclarativeImageBase * p);

    void init();

public: // Functions
    void loadVisible();

    void loadUrl();

    void loadDefault  ();
    void reloadDefault();

    void readDefault();

    void applyRequest();
    void applyFilter ();

    void setSourceDefault(bool sourceDefault);

    void applySourceDefault();

    void setExplicitSize(bool explicitSize);

    void clearFile();

public: // Slots
    void onLoaded(WCacheFile * file);

    void onFilterUpdated();
    void onFilterClear  ();

    void onFilesRemoved(const QStringList & urls, const QStringList & urlsCache);
    void onFilesCleared();

public: // Variables
    WCacheFile * file;

    WPixmapCache pix;

    QPixmap pixmapDefault;

    WDeclarativeImageBase::Status status;

    QString url;
    QString urlDefault;

    QSize sourceSize;
    QSize defaultSize;

    QSize sourceArea;

    WDeclarativeImageBase::LoadMode loadMode;

    bool asynchronous;

    qreal progress;

    WImageFilter * filter;

    bool sourceDefault: 1;

    bool explicitSize: 1;

    bool loadLater: 1;

    bool cache : 1;

protected:
    W_DECLARE_PUBLIC(WDeclarativeImageBase)
};

#endif // SK_NO_DECLARATIVEIMAGEBASE
#endif // WDECLARATIVEIMAGEBASE_P_H
