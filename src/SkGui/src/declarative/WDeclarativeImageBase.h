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

#ifndef WDECLARATIVEIMAGEBASE_H
#define WDECLARATIVEIMAGEBASE_H

// Sk includes
#ifdef QT_4
#include <WDeclarativeItem>
#else
#include <WDeclarativeTexture>
#endif

#ifndef SK_NO_DECLARATIVEIMAGEBASE

// Forward declarations
#ifdef QT_NEW
class QSGInternalImageNode;
#endif
class WDeclarativeImageBasePrivate;
class WImageFilter;

#ifdef QT_6
Q_MOC_INCLUDE("WImageFilter")
#endif

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeImageBase : public WDeclarativeItem
#else
class SK_GUI_EXPORT WDeclarativeImageBase : public WDeclarativeTexture
#endif
{
    Q_OBJECT

    Q_ENUMS(Status)
    Q_ENUMS(LoadMode)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

    Q_PROPERTY(bool isNull    READ isNull    NOTIFY statusChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY statusChanged)
    Q_PROPERTY(bool isReady   READ isReady   NOTIFY statusChanged)

    Q_PROPERTY(bool isSourceDefault READ isSourceDefault NOTIFY defaultChanged)
    Q_PROPERTY(bool isExplicitSize  READ isExplicitSize  NOTIFY explicitSizeChanged)

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(QString sourceDefault READ sourceDefault WRITE setSourceDefault
               NOTIFY sourceDefaultChanged)

    Q_PROPERTY(QSize sourceSize READ sourceSize WRITE setSourceSize RESET resetSourceSize
               NOTIFY sourceSizeChanged)

    Q_PROPERTY(QSize defaultSize READ defaultSize WRITE setDefaultSize RESET resetDefaultSize
               NOTIFY defaultSizeChanged)

    Q_PROPERTY(QSize sourceArea READ sourceArea WRITE setSourceArea RESET resetSourceArea
               NOTIFY sourceAreaChanged)

    Q_PROPERTY(LoadMode loadMode READ loadMode WRITE setLoadMode NOTIFY loadModeChanged)

    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous
               NOTIFY asynchronousChanged)

    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

    Q_PROPERTY(WImageFilter * filter READ filter WRITE setFilter NOTIFY filterChanged)

    Q_PROPERTY(qreal ratioWidth  READ ratioWidth  NOTIFY sourceSizeChanged)
    Q_PROPERTY(qreal ratioHeight READ ratioHeight NOTIFY sourceSizeChanged)

public: // Enums
    enum Status { Null, Loading, Ready, Error };

    enum LoadMode { LoadAlways, LoadVisible };

public:
#ifdef QT_4
    explicit WDeclarativeImageBase(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeImageBase(QQuickItem * parent = NULL);
#endif
protected:
#ifdef QT_4
    WDeclarativeImageBase(WDeclarativeImageBasePrivate * p, QDeclarativeItem * parent = NULL);
#else
    WDeclarativeImageBase(WDeclarativeImageBasePrivate * p, QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE void loadSource(const QString & url, bool force = false);

    Q_INVOKABLE void loadNow(const QString & url = QString());

    Q_INVOKABLE void reload();

    Q_INVOKABLE void applyImage(const QImage & image);

#ifdef QT_4
    Q_INVOKABLE void applyItemShot(QGraphicsObject * object);
#else
    Q_INVOKABLE void applyItemShot(QQuickItem * item);
#endif

    Q_INVOKABLE QImage toImage() const;

public: // QDeclarativeItem / QQuickItem reimplementation
    /* virtual */ void componentComplete();

protected: // Functions
    const QPixmap & currentPixmap() const;

    // NOTE: The current pixmap size divided by the pixel ratio.
    QSize currentSize() const;

protected: // Virtual functions
    virtual void load();

    virtual void applyUrl(const QString & url, bool asynchronous);

    virtual void clearUrl(WDeclarativeImageBase::Status status);

#ifdef QT_4
    virtual const QPixmap & getPixmap();
#endif

    virtual void pixmapChange();
    virtual void pixmapClear (); // {}

protected slots:
    virtual void requestFinished();
    virtual void requestProgress(qint64 received, qint64 total);

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_4
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);
#else
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & value);

#ifdef QT_OLD
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);
#else
    /* virtual */ void geometryChange(const QRectF & newGeometry, const QRectF & oldGeometry);
#endif

protected: // WDeclarativeItem reimplementation
    /* virtual */ void updateRatioPixel();

protected: // WDeclarativeTexture implementation
    /* virtual */ const QPixmap & getPixmap();
#endif

signals:
    void loaded();

    void statusChanged();

    void defaultChanged     ();
    void explicitSizeChanged();

    void sourceChanged       ();
    void sourceDefaultChanged();

    void sourceSizeChanged ();
    void defaultSizeChanged();

    void sourceAreaChanged();

    void loadModeChanged();

    void asynchronousChanged();
    void cacheChanged       ();

    void progressChanged();

    void filterChanged();

public: // Properties
    QPixmap pixmap() const;
    void    setPixmap(const QPixmap & pixmap);

    Status status() const;

    bool isNull   () const;
    bool isLoading() const;
    bool isReady  () const;

    bool isSourceDefault() const;
    bool isExplicitSize () const;

    QString source() const;
    void    setSource(const QString & url);

    QString sourceDefault() const;
    void    setSourceDefault(const QString & url);

    QSize sourceSize() const;
    void  setSourceSize(const QSize & size);
    void  resetSourceSize();

    QSize defaultSize() const;
    void  setDefaultSize(const QSize & size);
    void  resetDefaultSize();

    QSize sourceArea() const;
    void  setSourceArea(const QSize & size);
    void  resetSourceArea();

    LoadMode loadMode() const;
    void     setLoadMode(LoadMode mode);

    bool asynchronous() const;
    void setAsynchronous(bool enabled);

    bool cache() const;
    void setCache(bool enabled);

    qreal progress() const;

    WImageFilter * filter() const;
    void           setFilter(WImageFilter * filter);

    qreal ratioWidth () const;
    qreal ratioHeight() const;

private:
    W_DECLARE_PRIVATE(WDeclarativeImageBase)

    Q_PRIVATE_SLOT(d_func(), void onLoaded(WCacheFile *))

    Q_PRIVATE_SLOT(d_func(), void onFilterUpdated())
    Q_PRIVATE_SLOT(d_func(), void onFilterClear  ())

    Q_PRIVATE_SLOT(d_func(), void onFilesRemoved(const QStringList &, const QStringList &))
    Q_PRIVATE_SLOT(d_func(), void onFilesCleared())
};

#include <private/WDeclarativeImageBase_p>

#endif // SK_NO_DECLARATIVEIMAGEBASE
#endif // WDECLARATIVEIMAGEBASE_H
