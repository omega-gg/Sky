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

#ifndef WDECLARATIVEIMAGESVG_H
#define WDECLARATIVEIMAGESVG_H

// Sk includes
#ifdef QT_4
#include <WDeclarativeItem>
#else
#include <WDeclarativeItemPaint>
#endif

#ifndef SK_NO_DECLARATIVEIMAGESVG

class WDeclarativeImageSvgPrivate;
class WDeclarativeImageSvgScalePrivate;

//-------------------------------------------------------------------------------------------------
// WDeclarativeImageSvg
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeImageSvg : public WDeclarativeItem
#else
class SK_GUI_EXPORT WDeclarativeImageSvg : public WDeclarativeItemPaint
#endif
{
    Q_OBJECT

    Q_ENUMS(Status)
    Q_ENUMS(LoadMode)
    Q_ENUMS(FillMode)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

    Q_PROPERTY(bool isNull    READ isNull    NOTIFY statusChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY statusChanged)
    Q_PROPERTY(bool isReady   READ isReady   NOTIFY statusChanged)

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(LoadMode loadMode READ loadMode WRITE setLoadMode NOTIFY loadModeChanged)
    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

public: // Enums
    enum Status { Null, Loading, Ready, Error };

    enum LoadMode { LoadAlways, LoadVisible };

    enum FillMode
    {
        Stretch,
        PreserveAspectFit,
        PreserveAspectCrop
    };

public:
#ifdef QT_4
    explicit WDeclarativeImageSvg(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeImageSvg(QQuickItem * parent = NULL);
#endif
protected:
#ifdef QT_4
    WDeclarativeImageSvg(WDeclarativeImageSvgPrivate * p, QDeclarativeItem * parent = NULL);
#else
    WDeclarativeImageSvg(WDeclarativeImageSvgPrivate * p, QQuickItem * parent = NULL);
#endif

public: // QDeclarativeItem / QQuickItem reimplementation
    /* virtual */ void componentComplete();

public: // QGraphicsItem / QQuickPaintedItem reimplementation
#ifdef QT_4
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);
#else
    /* virtual */ void paint(QPainter * painter);
#endif

protected: // Functions
    virtual void svgChange();
    virtual void svgClear (); /* {} */

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_4
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);
#else
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & data);
#endif

signals:
    void loaded();

    void statusChanged();

    void sourceChanged();

    void loadModeChanged();
    void fillModeChanged();

    void progressChanged();

public: // Properties
    Status status() const;

    bool isNull   () const;
    bool isLoading() const;
    bool isReady  () const;

    QUrl source() const;
    void setSource(const QUrl & url);

    LoadMode loadMode() const;
    void     setLoadMode(LoadMode mode);

    FillMode fillMode() const;
    void     setFillMode(FillMode fillMode);

    qreal progress() const;

private:
    W_DECLARE_PRIVATE(WDeclarativeImageSvg)

    Q_PRIVATE_SLOT(d_func(), void onUpdate())
};

QML_DECLARE_TYPE(WDeclarativeImageSvg)

//-------------------------------------------------------------------------------------------------
// WDeclarativeImageSvgScale
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeImageSvgScale : public WDeclarativeImageSvg
{
    Q_OBJECT

    Q_PROPERTY(bool scaling READ isScaling WRITE setScaling NOTIFY scalingChanged)

    Q_PROPERTY(bool scaleDelayed READ scaleDelayed WRITE setScaleDelayed
               NOTIFY scaleDelayedChanged)

    Q_PROPERTY(int scaleDelay READ scaleDelay WRITE setScaleDelay NOTIFY scaleDelayChanged)

public:
#ifdef QT_4
    explicit WDeclarativeImageSvgScale(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeImageSvgScale(QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE void applyScale();

public: // QGraphicsItem / QQuickPaintedItem reimplementation
#ifdef QT_4
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);
#else
    /* virtual */ void paint(QPainter * painter);
#endif

protected: // QGraphicsItem / QQuickItem reimplementation
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);

protected: // WDeclarativeImageSvg reimplementation
    /* virtual */ void svgChange();
    /* virtual */ void svgClear ();

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
    W_DECLARE_PRIVATE(WDeclarativeImageSvgScale)

    Q_PRIVATE_SLOT(d_func(), void onScale())
};

QML_DECLARE_TYPE(WDeclarativeImageSvgScale)

#include <private/WDeclarativeImageSvg_p>

#endif // SK_NO_DECLARATIVEIMAGESVG
#endif // WDECLARATIVEIMAGESVG_H
