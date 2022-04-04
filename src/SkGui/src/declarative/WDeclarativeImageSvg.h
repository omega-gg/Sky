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

#ifndef WDECLARATIVEIMAGESVG_H
#define WDECLARATIVEIMAGESVG_H

// Sk includes
#ifdef QT_4
#include <WDeclarativeItem>
#else
#include <WDeclarativeTexture>
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
class SK_GUI_EXPORT WDeclarativeImageSvg : public WDeclarativeTexture
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

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(LoadMode loadMode READ loadMode WRITE setLoadMode NOTIFY loadModeChanged)
    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

#ifdef QT_NEW
    Q_PROPERTY(bool scaleDelayed READ scaleDelayed WRITE setScaleDelayed
               NOTIFY scaleDelayedChanged)

    Q_PROPERTY(int scaleDelay READ scaleDelay WRITE setScaleDelay NOTIFY scaleDelayChanged)

    Q_PROPERTY(bool scaleLater READ scaleLater WRITE setScaleLater NOTIFY scaleLaterChanged)
#endif

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

public: // Interface
    Q_INVOKABLE void reload();

protected:
#ifdef QT_4
    WDeclarativeImageSvg(WDeclarativeImageSvgPrivate * p, QDeclarativeItem * parent = NULL);
#else
    WDeclarativeImageSvg(WDeclarativeImageSvgPrivate * p, QQuickItem * parent = NULL);
#endif

public: // QDeclarativeItem / QQuickItem reimplementation
    /* virtual */ void componentComplete();

#ifdef QT_4
public: // QGraphicsItem reimplementation
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);
#endif

protected: // Functions
    virtual void svgChange();
    virtual void svgClear (); // {}

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

protected: // WDeclarativeTexture implementation
    /* virtual */ const QPixmap & getPixmap();

protected: // WDeclarativeTexture reimplementation
    /*virtual */ void applyGeometry(QSGInternalImageNode * node, const QPixmap & pixmap);
#endif

signals:
    void loaded();

    void statusChanged();

    void sourceChanged();

    void loadModeChanged();
    void fillModeChanged();

    void progressChanged();

#ifdef QT_NEW
    void scaleDelayedChanged();
    void scaleDelayChanged  ();
    void scaleLaterChanged  ();
#endif

public: // Properties
    Status status() const;

    bool isNull   () const;
    bool isLoading() const;
    bool isReady  () const;

    QString source() const;
    void    setSource(const QString & url);

    LoadMode loadMode() const;
    void     setLoadMode(LoadMode mode);

    FillMode fillMode() const;
    void     setFillMode(FillMode fillMode);

    qreal progress() const;

#ifdef QT_NEW
    bool scaleDelayed() const;
    void setScaleDelayed(bool delayed);

    int  scaleDelay() const;
    void setScaleDelay(int delay);

    bool scaleLater() const;
    void setScaleLater(bool enabled);
#endif

private:
    W_DECLARE_PRIVATE(WDeclarativeImageSvg)

#ifdef QT_4
    Q_PRIVATE_SLOT(d_func(), void onUpdate())
#else
    Q_PRIVATE_SLOT(d_func(), void onTimeout())
#endif
};

QML_DECLARE_TYPE(WDeclarativeImageSvg)

#ifdef QT_4

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
    explicit WDeclarativeImageSvgScale(QDeclarativeItem * parent = NULL);

public: // Interface
    Q_INVOKABLE void applyScale();

public: // QGraphicsItem reimplementation
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);

protected: // QGraphicsItem reimplementation
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

#endif // QT_4

#include <private/WDeclarativeImageSvg_p>

#endif // SK_NO_DECLARATIVEIMAGESVG
#endif // WDECLARATIVEIMAGESVG_H
