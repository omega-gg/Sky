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

#ifndef WDECLARATIVETEXTSVG_H
#define WDECLARATIVETEXTSVG_H

// Sk includes
#ifdef QT_4
#include <WDeclarativeItem>
#else
#include <WDeclarativeTexture>
#endif

#ifndef SK_NO_DECLARATIVETEXTSVG

// Forward declarations
class WDeclarativeTextSvgPrivate;
class WDeclarativeTextSvgScalePrivate;
class WDeclarativeGradient;

//-------------------------------------------------------------------------------------------------
// WDeclarativeTextSvg
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeTextSvg : public WDeclarativeItem
#else
class SK_GUI_EXPORT WDeclarativeTextSvg : public WDeclarativeTexture
#endif
{
    Q_OBJECT

    Q_ENUMS(HAlignment)
    Q_ENUMS(VAlignment)

    Q_ENUMS(LoadMode)
    Q_ENUMS(TextStyle)
    Q_ENUMS(TextOutline)

    Q_PROPERTY(int textWidth  READ textWidth  NOTIFY textWidthChanged)
    Q_PROPERTY(int textHeight READ textHeight NOTIFY textHeightChanged)

    Q_PROPERTY(int marginWidth  READ marginWidth  WRITE setMarginWidth  NOTIFY marginWidthChanged)
    Q_PROPERTY(int marginHeight READ marginHeight WRITE setMarginHeight NOTIFY marginHeightChanged)

    Q_PROPERTY(qreal multiplier READ multiplier WRITE setMultiplier NOTIFY multiplierChanged)

    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

    Q_PROPERTY(HAlignment horizontalAlignment READ hAlign WRITE setHAlign
               NOTIFY horizontalAlignmentChanged)

    Q_PROPERTY(VAlignment verticalAlignment READ vAlign WRITE setVAlign
               NOTIFY verticalAlignmentChanged)

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

    Q_PROPERTY(LoadMode loadMode READ loadMode WRITE setLoadMode NOTIFY loadModeChanged)

    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    Q_PROPERTY(WDeclarativeGradient * gradient READ gradient WRITE setGradient
               NOTIFY gradientChanged)

    Q_PROPERTY(TextStyle   style   READ style   WRITE setStyle   NOTIFY styleChanged)
    Q_PROPERTY(TextOutline outline READ outline WRITE setOutline NOTIFY outlineChanged)

    Q_PROPERTY(QColor styleColor READ styleColor WRITE setStyleColor NOTIFY styleColorChanged)
    Q_PROPERTY(int    styleSize  READ styleSize  WRITE setStyleSize  NOTIFY styleSizeChanged)

#ifdef QT_LATEST
    Q_PROPERTY(bool scaleDelayed READ scaleDelayed WRITE setScaleDelayed
               NOTIFY scaleDelayedChanged)

    Q_PROPERTY(int scaleDelay READ scaleDelay WRITE setScaleDelay NOTIFY scaleDelayChanged)

    Q_PROPERTY(bool scaleLater READ scaleLater WRITE setScaleLater NOTIFY scaleLaterChanged)
#endif

public: // Enums
    enum HAlignment
    {
        AlignLeft    = Qt::AlignLeft,
        AlignRight   = Qt::AlignRight,
        AlignHCenter = Qt::AlignHCenter,
        AlignJustify = Qt::AlignJustify
    };

    enum VAlignment
    {
        AlignTop     = Qt::AlignTop,
        AlignBottom  = Qt::AlignBottom,
        AlignVCenter = Qt::AlignVCenter
    };

    enum LoadMode { LoadAlways, LoadVisible };

    enum TextStyle { Normal, Outline, Raised, Sunken, Glow };

    enum TextOutline { OutlineNormal, OutlineRound };

public:
#ifdef QT_4
    explicit WDeclarativeTextSvg(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeTextSvg(QQuickItem * parent = NULL);
#endif

protected:
#ifdef QT_4
    WDeclarativeTextSvg(WDeclarativeTextSvgPrivate * p, QDeclarativeItem * parent = NULL);
#else
    WDeclarativeTextSvg(WDeclarativeTextSvgPrivate * p, QQuickItem * parent = NULL);
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

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_4
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);
#else
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & value);

    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);

protected: // WDeclarativeTexture implementation
    /* virtual */ const QPixmap & getPixmap();

protected: // WDeclarativeTexture reimplementation
    /*virtual */ void applyGeometry(QSGInternalImageNode * node, const QPixmap & pixmap);
#endif

signals:
    void textWidthChanged ();
    void textHeightChanged();

    void marginWidthChanged ();
    void marginHeightChanged();

    void multiplierChanged();

    void zoomChanged();

    void horizontalAlignmentChanged();
    void verticalAlignmentChanged  ();

    void textChanged();

    void loadModeChanged();

    void fontChanged();

    void colorChanged   ();
    void gradientChanged();

    void styleChanged  ();
    void outlineChanged();

    void styleColorChanged();
    void styleSizeChanged ();

#ifdef QT_LATEST
    void scaleDelayedChanged();
    void scaleDelayChanged  ();
    void scaleLaterChanged  ();
#endif

public: // Properties
    int textWidth () const;
    int textHeight() const;

    int  marginWidth() const;
    void setMarginWidth(int width);

    int  marginHeight() const;
    void setMarginHeight(int height);

    qreal multiplier() const;
    void  setMultiplier(qreal multiplier);

    qreal zoom() const;
    void  setZoom(qreal zoom);

    HAlignment hAlign() const;
    void       setHAlign(HAlignment align);

    VAlignment vAlign() const;
    void       setVAlign(VAlignment align);

    QString text() const;
    void    setText(const QString & text);

    LoadMode loadMode() const;
    void     setLoadMode(LoadMode mode);

    QFont font() const;
    void  setFont(const QFont & font);

    QColor color() const;
    void   setColor(const QColor & color);

    WDeclarativeGradient * gradient() const;
    void                   setGradient(WDeclarativeGradient * gradient);

    TextStyle style() const;
    void      setStyle(TextStyle style);

    TextOutline outline() const;
    void        setOutline(TextOutline outline);

    QColor styleColor() const;
    void   setStyleColor(const QColor & color);

    int  styleSize() const;
    void setStyleSize(int size);

#ifdef QT_LATEST
    bool scaleDelayed() const;
    void setScaleDelayed(bool delayed);

    int  scaleDelay() const;
    void setScaleDelay(int delay);

    bool scaleLater() const;
    void setScaleLater(bool enabled);
#endif

private:
    W_DECLARE_PRIVATE(WDeclarativeTextSvg)

    Q_PRIVATE_SLOT(d_func(), void onLoad())

#ifdef QT_4
    Q_PRIVATE_SLOT(d_func(), void onUpdate())
#else
    Q_PRIVATE_SLOT(d_func(), void onTimeout())
#endif
};

QML_DECLARE_TYPE(WDeclarativeTextSvg)

#ifdef QT_4

//-------------------------------------------------------------------------------------------------
// WDeclarativeTextSvgScale
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeTextSvgScale : public WDeclarativeTextSvg
{
    Q_OBJECT

    Q_PROPERTY(bool scaling READ isScaling WRITE setScaling NOTIFY scalingChanged)

    Q_PROPERTY(bool scaleDelayed READ scaleDelayed WRITE setScaleDelayed
               NOTIFY scaleDelayedChanged)

    Q_PROPERTY(int scaleDelay READ scaleDelay WRITE setScaleDelay NOTIFY scaleDelayChanged)

public:
    explicit WDeclarativeTextSvgScale(QDeclarativeItem * parent = NULL);

public: // Interface
    Q_INVOKABLE void applyScale();

public: // QGraphicsItem reimplementation
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);

protected: // QGraphicsItem reimplementation
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);

protected: // WDeclarativeTextSvg reimplementation
    /* virtual */ void svgChange();

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
    W_DECLARE_PRIVATE(WDeclarativeTextSvgScale)

    Q_PRIVATE_SLOT(d_func(), void onScale())
};

QML_DECLARE_TYPE(WDeclarativeTextSvgScale)

#endif

#include <private/WDeclarativeTextSvg_p>

#endif // SK_NO_DECLARATIVETEXTSVG
#endif // WDECLARATIVETEXTSVG_H
