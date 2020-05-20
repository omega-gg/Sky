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

#ifndef WPIXMAPCACHE_H
#define WPIXMAPCACHE_H

// Qt includes
#include <QSize>

// Sk includes
#include <Sk>

#ifndef SK_NO_PIXMAPCACHE

// Forward declarations
class QObject;
class QImageReader;
class QImage;
class QPixmap;
class QRect;
class WPixmapCachePrivate;
class WAbstractThreadAction;

//-------------------------------------------------------------------------------------------------
// WPixmapCache
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WPixmapCache : public WPrivatable
{
public: // Enums
    enum Status { Null, Default, Loading };

public:
    WPixmapCache();

public: // Interface
    void load(const QString & path, const QSize & size         = QSize(),
                                    const QSize & area         = QSize(),
                                    bool          asynchronous = false,
                                    bool          cache        = true,
                                    QObject     * receiver     = NULL,
                                    const char  * method       = "");

    void applyPixmap(const QPixmap & pixmap, const QString & path);

    void changePixmap(const QPixmap & pixmap);

    void disconnect(QObject * receiver);
    void clear     (QObject * receiver = NULL);

public: // Static functions
    static bool imageIsLocal(const QString & path);

    static WAbstractThreadAction * loadImage(const QString & path, QObject    * receiver,
                                                                   const char * method);

    static WAbstractThreadAction * loadImage(const QString & path, const QSize & size,
                                                                   QObject     * receiver,
                                                                   const char  * method);

    static WAbstractThreadAction * loadImage(const QString & path, const QSize & size,
                                                                   const QSize & area,
                                                                   QObject     * receiver,
                                                                   const char  * method);

    //static QSize getSize(const QImageReader & reader, const QSize & size);

    static QSize getSize(const QSize & sizeA, const QSize & sizeB);

    static QSize getArea(const QSize & size, const QSize & area);

#ifdef QT_4
    static QSize scaleSize(const QSize & size, int width, int height, Qt::AspectRatioMode mode);
#endif

    //static void applySize(QImageReader * reader, const QSize & size);

    static QPixmap getPixmapScaled(const QPixmap & pixmap, const QSize & size);

    static bool readImage(QImage * image, const QString & path, const QSize & size,
                                                                const QSize & area);

    static bool readPixmap(QPixmap * pixmap, const QString & path, const QSize & size,
                                                                   const QSize & area);

    static bool scaleImage (QImage  * image,  const QString & path, const QSize & size);
    static bool scalePixmap(QPixmap * pixmap, const QString & path, const QSize & size);

    static void registerPixmap   (const QString & id, const QPixmap & pixmap);
    static void unregisterPixmap (const QString & id);
    static void unregisterPixmaps();

    static void clearCache();

public: // Properties
    bool isNull   () const;
    bool isLoading() const;
    bool isLoaded () const;

    const QPixmap & pixmap() const;
    void            setPixmap(const QPixmap & pixmap, QObject * receiver = NULL);

    const QString & path() const;
    const QSize   & size() const;

    int width () const;
    int height() const;

    QRect rect() const;

public: // Static properties
    static qint64 sizeMax();
    static void   setSizeMax(qint64 max);

private:
    W_DECLARE_PRIVATE(WPixmapCache)

    friend class WPixmapCacheStore;
    friend class WPixmapCacheReply;
};

#endif // SK_NO_PIXMAPCACHE
#endif // WPIXMAPCACHE_H
