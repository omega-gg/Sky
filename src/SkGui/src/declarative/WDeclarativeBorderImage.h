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

#ifndef WDECLARATIVEBORDERIMAGE_H
#define WDECLARATIVEBORDERIMAGE_H

// Sk includes
#include <WDeclarativeImageBase>

#ifndef SK_NO_DECLARATIVEBORDERIMAGE

// Forward declarations
class WDeclarativeBorderImagePrivate;
class WDeclarativeBorderImageScalePrivate;

//-------------------------------------------------------------------------------------------------
// WDeclarativeBorderGrid
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeBorderGrid : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int left   READ left   WRITE setLeft   NOTIFY borderChanged)
    Q_PROPERTY(int right  READ right  WRITE setRight  NOTIFY borderChanged)
    Q_PROPERTY(int top    READ top    WRITE setTop    NOTIFY borderChanged)
    Q_PROPERTY(int bottom READ bottom WRITE setBottom NOTIFY borderChanged)

public:
    explicit WDeclarativeBorderGrid(QObject * parent = NULL);

signals:
    void borderChanged();

public: // Properties
    int  left() const;
    void setLeft(int size);

    int  right() const;
    void setRight(int size);

    int  top() const;
    void setTop(int size);

    int  bottom() const;
    void setBottom(int size);

private: // Variables
    int _left;
    int _right;
    int _top;
    int _bottom;
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeBorderImage
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeBorderImage : public WDeclarativeImageBase
{
    Q_OBJECT

    Q_ENUMS(TileMode)

    Q_PROPERTY(WDeclarativeBorderGrid * border READ border CONSTANT)

    Q_PROPERTY(TileMode horizontalTileMode READ horizontalTileMode WRITE setHorizontalTileMode
               NOTIFY horizontalTileModeChanged)

    Q_PROPERTY(TileMode verticalTileMode READ verticalTileMode WRITE setVerticalTileMode
               NOTIFY verticalTileModeChanged)

public: // Enums
    enum TileMode
    {
        Stretch = Qt::StretchTile,
        Repeat  = Qt::RepeatTile,
        Round   = Qt::RoundTile
    };

public:
#ifdef QT_4
    explicit WDeclarativeBorderImage(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeBorderImage(QQuickItem * parent = NULL);
#endif
protected:
#ifdef QT_4
    WDeclarativeBorderImage(WDeclarativeBorderImagePrivate * p, QDeclarativeItem * parent = NULL);
#else
    WDeclarativeBorderImage(WDeclarativeBorderImagePrivate * p, QQuickItem * parent = NULL);
#endif

#ifdef QT_4
public: // QGraphicsItem reimplementation
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);
#endif

protected: // Virtual functions
    virtual const QMargins & getMargins() const;

#ifdef QT_LATEST
protected: // WDeclarativeTexture reimplementation
    /* virtual */ void applyGeometry(QSGInternalImageNode * node, const QPixmap & pixmap);
#endif

signals:
    void horizontalTileModeChanged();
    void verticalTileModeChanged  ();

public: // Properties
    WDeclarativeBorderGrid * border();

    TileMode horizontalTileMode() const;
    void     setHorizontalTileMode(TileMode mode);

    TileMode verticalTileMode() const;
    void     setVerticalTileMode(TileMode mode);

private:
    W_DECLARE_PRIVATE(WDeclarativeBorderImage)

    Q_PRIVATE_SLOT(d_func(), void onUpdate())
};

QML_DECLARE_TYPE(WDeclarativeBorderImage)

//-------------------------------------------------------------------------------------------------
// WDeclarativeBorderImageScale
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeBorderImageScale : public WDeclarativeBorderImage
{
    Q_OBJECT

    Q_PROPERTY(bool scaling READ isScaling WRITE setScaling NOTIFY scalingChanged)

    Q_PROPERTY(bool scaleDelayed READ scaleDelayed WRITE setScaleDelayed
               NOTIFY scaleDelayedChanged)

    Q_PROPERTY(int scaleDelay READ scaleDelay WRITE setScaleDelay NOTIFY scaleDelayChanged)

public:
#ifdef QT_4
    explicit WDeclarativeBorderImageScale(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeBorderImageScale(QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE void applyScale();

protected: // QGraphicsItem / QQuickItem reimplementation
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);

protected: // WDeclarativeImageBase reimplementation
    /* virtual */ const QPixmap & getPixmap();

    /* virtual */ void pixmapChange();
    /* virtual */ void pixmapClear ();

protected: // WDeclarativeBorderImage reimplementation
    virtual const QMargins & getMargins() const;

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
    W_DECLARE_PRIVATE(WDeclarativeBorderImageScale)

#ifdef QT_LATEST
    Q_PRIVATE_SLOT(d_func(), void onStart())
#endif

    Q_PRIVATE_SLOT(d_func(), void onScale())

    Q_PRIVATE_SLOT(d_func(), void onLoaded(const QImage &))
};

#include <private/WDeclarativeBorderImage_p>

#endif // SK_NO_DECLARATIVEBORDERIMAGE
#endif // WDECLARATIVEBORDERIMAGE_H
