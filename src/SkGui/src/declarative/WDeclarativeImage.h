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

#ifndef WDECLARATIVEIMAGE_H
#define WDECLARATIVEIMAGE_H

#include <WDeclarativeImageBase>

#ifndef SK_NO_DECLARATIVEIMAGE

// Forward declarations
class WDeclarativeImagePrivate;
class WDeclarativeImageScalePrivate;

//-------------------------------------------------------------------------------------------------
// WDeclarativeImage
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeImage : public WDeclarativeImageBase
{
    Q_OBJECT

    Q_ENUMS(FillMode)

    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)

    Q_PROPERTY(qreal paintedWidth  READ paintedWidth  NOTIFY paintedGeometryChanged)
    Q_PROPERTY(qreal paintedHeight READ paintedHeight NOTIFY paintedGeometryChanged)

public: // Enums
    enum FillMode
    {
        Stretch,
        PreserveAspectFit,
        PreserveAspectCrop,
        Tile,
        TileVertically,
        TileHorizontally
    };

public:
#ifdef QT_4
    explicit WDeclarativeImage(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeImage(QQuickItem * parent = NULL);
#endif
protected:
#ifdef QT_4
    WDeclarativeImage(WDeclarativeImagePrivate * p, QDeclarativeItem * parent = NULL);
#else
    WDeclarativeImage(WDeclarativeImagePrivate * p, QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE QRectF getRect() const;

#ifdef QT_4
public: // QGraphicsItem reimplementation
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);
#endif

protected: // Functions
    void updatePaintedGeometry();

protected: // Virtual functions
    virtual void updatePixmap(); // {}

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_OLD
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);
#else
    /* virtual */ void geometryChange(const QRectF & newGeometry, const QRectF & oldGeometry);
#endif

protected: // WDeclarativeImageBase reimplementation
    /* virtual */ void pixmapChange();

#ifdef QT_NEW
protected: // WDeclarativeTexture reimplementation
    /* virtual */ void applyGeometry(QSGInternalImageNode * node, const QPixmap & pixmap);
#endif

signals:
    void fillModeChanged();

    void paintedGeometryChanged();

public: // Properties
    FillMode fillMode() const;
    void     setFillMode(FillMode fillMode);

    qreal paintedWidth () const;
    qreal paintedHeight() const;

private:
    W_DECLARE_PRIVATE(WDeclarativeImage)
};

QML_DECLARE_TYPE(WDeclarativeImage)

//-------------------------------------------------------------------------------------------------
// WDeclarativeImageScale
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeImageScale : public WDeclarativeImage
{
    Q_OBJECT

    Q_PROPERTY(bool scaling READ isScaling WRITE setScaling NOTIFY scalingChanged)

    Q_PROPERTY(bool scaleDelayed READ scaleDelayed WRITE setScaleDelayed
               NOTIFY scaleDelayedChanged)

    Q_PROPERTY(int scaleDelay READ scaleDelay WRITE setScaleDelay NOTIFY scaleDelayChanged)

public:
#ifdef QT_4
    explicit WDeclarativeImageScale(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeImageScale(QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE void applyScale();

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_OLD
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);
#else
    /* virtual */ void geometryChange(const QRectF & newGeometry, const QRectF & oldGeometry);
#endif

protected: // WDeclarativeImageBase reimplementation
    /* virtual */ const QPixmap & getPixmap();

    /* virtual */ void pixmapClear();

protected: // WDeclarativeImage reimplementation
    /* virtual */ void updatePixmap();

signals:
    void scalingChanged();

    void scaleDelayedChanged();
    void scaleDelayChanged  ();

public: // Properties
    bool isScaling() const;
    void setScaling(bool scaling);

    bool scaleDelayed() const;
    void setScaleDelayed(bool delayed);

    int  scaleDelay() const;
    void setScaleDelay(int delay);

private:
    W_DECLARE_PRIVATE(WDeclarativeImageScale)

#ifdef QT_NEW
    Q_PRIVATE_SLOT(d_func(), void onStart())
#endif

    Q_PRIVATE_SLOT(d_func(), void onScale())

    Q_PRIVATE_SLOT(d_func(), void onLoaded(const QImage &))
};

QML_DECLARE_TYPE(WDeclarativeImageScale)

#include <private/WDeclarativeImage_p>

#endif // SK_NO_DECLARATIVEIMAGE
#endif // WDECLARATIVEIMAGE_H
