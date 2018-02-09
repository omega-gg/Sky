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

#ifndef WDECLARATIVETEXTSVG_H
#define WDECLARATIVETEXTSVG_H

// Sk includes
#include <WDeclarativeText>

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
class SK_GUI_EXPORT WDeclarativeTextSvg : public WDeclarativeItemPaint
#endif
{
    Q_OBJECT

    Q_ENUMS(LoadMode)
    Q_ENUMS(TextStyle)
    Q_ENUMS(TextOutline)

    Q_PROPERTY(int textWidth  READ textWidth  NOTIFY textWidthChanged)
    Q_PROPERTY(int textHeight READ textHeight NOTIFY textHeightChanged)

    Q_PROPERTY(int marginWidth  READ marginWidth  WRITE setMarginWidth  NOTIFY marginWidthChanged)
    Q_PROPERTY(int marginHeight READ marginHeight WRITE setMarginHeight NOTIFY marginHeightChanged)

    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

    Q_PROPERTY(WDeclarativeText::HAlignment horizontalAlignment READ hAlign WRITE setHAlign
               NOTIFY horizontalAlignmentChanged)

    Q_PROPERTY(WDeclarativeText::VAlignment verticalAlignment READ vAlign WRITE setVAlign
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

public: // Enums
    enum LoadMode { LoadAlways, LoadVisible };

    enum TextStyle
    {
        Normal  = WDeclarativeText::Normal,
        Outline = WDeclarativeText::Outline,
        Raised  = WDeclarativeText::Raised,
        Sunken  = WDeclarativeText::Sunken,
        Glow
    };

    enum TextOutline { OutlineNormal, OutlineRound };

public:
#ifdef QT_4
    explicit WDeclarativeTextSvg(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeTextSvg(QQuickItem * parent = NULL);
#endif

protected:
    WDeclarativeTextSvg(WDeclarativeTextSvgPrivate * p, QDeclarativeItem * parent = NULL);

public: // QDeclarativeItem reimplementation
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

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_4
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);
#else
    /* virtual */ QVariant itemChange(ItemChange change, const ItemChangeData & data);
#endif

signals:
    void textWidthChanged ();
    void textHeightChanged();

    void marginWidthChanged ();
    void marginHeightChanged();

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

public: // Properties
    int textWidth () const;
    int textHeight() const;

    int  marginWidth() const;
    void setMarginWidth(int width);

    int  marginHeight() const;
    void setMarginHeight(int height);

    qreal zoom() const;
    void  setZoom(qreal zoom);

    WDeclarativeText::HAlignment hAlign() const;
    void                         setHAlign(WDeclarativeText::HAlignment align);

    WDeclarativeText::VAlignment vAlign() const;
    void                         setVAlign(WDeclarativeText::VAlignment align);

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

private:
    W_DECLARE_PRIVATE(WDeclarativeTextSvg)

    Q_PRIVATE_SLOT(d_func(), void onLoad  ())
    Q_PRIVATE_SLOT(d_func(), void onUpdate())
};

QML_DECLARE_TYPE(WDeclarativeTextSvg)

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
#ifdef QT_4
    explicit WDeclarativeTextSvgScale(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeTextSvgScale(QQuickItem * parent = NULL);
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

#include <private/WDeclarativeTextSvg_p>

#endif // SK_NO_DECLARATIVETEXTSVG
#endif // WDECLARATIVETEXTSVG_H
