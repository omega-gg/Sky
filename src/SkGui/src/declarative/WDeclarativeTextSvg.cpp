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

#include "WDeclarativeTextSvg.h"

#ifndef SK_NO_DECLARATIVETEXTSVG

// Qt includes
#include <QSvgRenderer>
#include <QPainter>

// Sk includes
#include <WControllerView>
#ifdef QT_LATEST
#include <WView>
#endif
#include <WImageColorFilter>

//=================================================================================================
// WDeclarativeTextSvgPrivate
//=================================================================================================

WDeclarativeTextSvgPrivate::WDeclarativeTextSvgPrivate(WDeclarativeTextSvg * p)
#ifdef QT_4
    : WDeclarativeItemPrivate(p) {}
#else
    : WDeclarativeItemPaintPrivate(p) {}
#endif

void WDeclarativeTextSvgPrivate::init()
{
    Q_Q(WDeclarativeTextSvg);

    renderer = new QSvgRenderer(q);

    loadMode = static_cast<WDeclarativeTextSvg::LoadMode> (wControllerView->loadMode());

    loadLater = false;

    textWidth  = 0;
    textHeight = 0;

    marginWidth  = 0;
    marginHeight = 0;

    zoom = 1.0;

    gradient = NULL;

    style   = WDeclarativeTextSvg::Normal;
    outline = WDeclarativeTextSvg::OutlineNormal;

    styleSize = 1;

    hAlign = WDeclarativeText::AlignLeft;
    vAlign = WDeclarativeText::AlignTop;

#ifdef QT_4
    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif

    QObject::connect(renderer, SIGNAL(repaintNeeded()), q, SLOT(onUpdate()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeTextSvgPrivate::setTextSize(int width, int height)
{
    Q_Q(WDeclarativeTextSvg);

    if (textWidth != width)
    {
        textWidth = width;

        emit q->textWidthChanged();
    }

    if (textHeight != height)
    {
        textHeight = height;

        emit q->textHeightChanged();
    }
}

//-------------------------------------------------------------------------------------------------

QRectF WDeclarativeTextSvgPrivate::getRect(qreal width, qreal height)
{
    qreal x;
    qreal y;

    int textWidth  = this->textWidth  * zoom;
    int textHeight = this->textHeight * zoom;

    if (hAlign == WDeclarativeText::AlignRight)
    {
        x = width - textWidth - marginWidth;
    }
    else if (hAlign == WDeclarativeText::AlignHCenter)
    {
        x = (width - textWidth) / 2;
    }
    else x = marginWidth;

    if (vAlign == WDeclarativeText::AlignBottom)
    {
        y = height - textHeight - marginHeight;
    }
    else if (vAlign == WDeclarativeText::AlignVCenter)
    {
        y = (height - textHeight) / 2;
    }
    else y = marginHeight;

    return QRectF(x, y, textWidth, textHeight);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeTextSvgPrivate::load()
{
    if (loadMode == WDeclarativeTextSvg::LoadVisible && q_func()->isVisible() == false)
    {
        loadLater = true;
    }
    else loadSvg();
}

void WDeclarativeTextSvgPrivate::loadVisible()
{
    if (loadLater == false) return;

    loadLater = false;

    loadSvg();
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeTextSvgPrivate::loadSvg()
{
    Q_Q(WDeclarativeTextSvg);

    if (text.isEmpty())
    {
        setTextSize(0, 0);

        renderer->load(QByteArray());
    }
    else
    {
        QFontMetrics metrics(font);

        int pixelSize = font.pixelSize();

        int pixelY = metrics.ascent();

        QString family = font.family();

        QString style  = getStyle ();
        QString weight = getWeight();

        QString size = QString::number(pixelSize);

        QString item;

        QString color = addGradient(&item);

        if (this->style == WDeclarativeTextSvg::Outline)
        {
            int sizeOutline = styleSize * pixelSize / 200;

            QString x = QString::number(sizeOutline);
            QString y = QString::number(pixelY + sizeOutline);

            QString colorStyle = styleColor.name();

            QString extra = getOutline(colorStyle, styleSize);

            addText(&item, x, y, family, style, weight, size, color, extra);

            sizeOutline *= 2;

            setTextSize(getWidth(metrics, text) + sizeOutline, metrics.height() + sizeOutline);
        }
        else if (this->style == WDeclarativeTextSvg::Raised)
        {
            QString y      = QString::number(pixelY);
            QString yStyle = QString::number(pixelY + styleSize);

            QString colorStyle = styleColor.name();

            addText(&item, "0", yStyle, family, style, weight, size, colorStyle);
            addText(&item, "0", y,      family, style, weight, size, color);

            setTextSize(getWidth(metrics, text), metrics.height());
        }
        else if (this->style == WDeclarativeTextSvg::Sunken)
        {
            QString y      = QString::number(pixelY);
            QString yStyle = QString::number(pixelY - styleSize);

            QString colorStyle = styleColor.name();

            addText(&item, "0", yStyle, family, style, weight, size, colorStyle);
            addText(&item, "0", y,      family, style, weight, size, color);

            setTextSize(getWidth(metrics, text), metrics.height());
        }
        else if (this->style == WDeclarativeTextSvg::Glow)
        {
            int sizeOutline = styleSize * pixelSize / 100;

            QString x = QString::number(sizeOutline);
            QString y = QString::number(pixelY + sizeOutline);

            QString colorStyle = styleColor.name();

            QString extra = getOutline(colorStyle, styleSize * 2);

            addText(&item, x, y, family, style, weight, size, colorStyle, extra);
            addText(&item, x, y, family, style, weight, size, color);

            sizeOutline *= 2;

            setTextSize(getWidth(metrics, text) + sizeOutline, metrics.height() + sizeOutline);
        }
        else
        {
            QString y = QString::number(pixelY);

            addText(&item, "0", y, family, style, weight, size, color);

            setTextSize(getWidth(metrics, text), metrics.height());
        }

        QString stringWidth  = QString::number(textWidth);
        QString stringHeight = QString::number(textHeight);

        QByteArray content;

        content.append("<svg width=\"" + stringWidth + "\" height=\"" + stringHeight + "\">"
                       +
                       item + "</svg>");

        renderer->load(content);
    }

    q->svgChange();
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativeTextSvgPrivate::addGradient(QString * item) const
{
    if (gradient == NULL)
    {
        return color.name();
    }

    if (gradient->type() == WDeclarativeGradient::LinearVertical)
    {
         item->append("<defs><linearGradient id=\"gradient\" "
                      "x1=\"0%\" y1=\"0%\" x2=\"0%\" y2=\"100%\">");
    }
    else item->append("<defs><linearGradient id=\"gradient\" "
                      "x1=\"0%\" y1=\"0%\" x2=\"100%\" y2=\"0%\">");

    QList<WDeclarativeGradientStop *> stops = gradient->getStops();

    foreach (const WDeclarativeGradientStop * stop, stops)
    {
        QString position = QString::number(stop->position() * 100);

        item->append("<stop offset=\"" + position + "%\" stop-color=\"" + stop->color().name()
                     +
                     "\"/>");
    }

    item->append("</linearGradient></defs>");

    return "url(#gradient)";
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeTextSvgPrivate::addText(QString * item, const QString & x,
                                                         const QString & y,
                                                         const QString & family,
                                                         const QString & style,
                                                         const QString & weight,
                                                         const QString & size,
                                                         const QString & color,
                                                         const QString & extra) const
{
    item->append("<text x=\"" + x + "\" y=\"" + y + "\" font-family=\"" + family
                 +
                 "\" font-style=\"" + style + "\" font-weight=\"" + weight
                 +
                 "\" font-size=\"" + size + "\" fill=\"" + color + "\"" + extra + ">"
                 +
                 text.toUtf8() + "</text>");
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeTextSvgPrivate::getWidth(const QFontMetrics & metrics, const QString & text) const
{
    // FIXME: Workaround to fix the width of the arial font.
    if (font.family().toLower() == "arial")
    {
         return metrics.width(text) * 1.01;
    }
    else return metrics.width(text);
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativeTextSvgPrivate::getStyle() const
{
    QFont::Style style = font.style();

    if      (style == QFont::StyleItalic)  return "italic";
    else if (style == QFont::StyleOblique) return "oblique";
    else                                   return "normal";
}

QString WDeclarativeTextSvgPrivate::getWeight() const
{
    int weight = font.weight();

    if      (weight == QFont::Light) return "lighter";
    else if (weight == QFont::Bold)  return "bold";
    else if (weight == QFont::Black) return "bolder";
    else                             return "normal";
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativeTextSvgPrivate::getOutline(const QString & color, int size) const
{
    QString string = " stroke=\"" + color + "\" stroke-width=\"" + QString::number(size) + "\"";

    if (outline == WDeclarativeTextSvg::OutlineRound)
    {
         return string + " stroke-linejoin=\"round\"";
    }
    else return string;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeTextSvgPrivate::onLoad()
{
    Q_Q(WDeclarativeTextSvg);

    if (q->isComponentComplete()) load();
}

void WDeclarativeTextSvgPrivate::onUpdate()
{
    Q_Q(WDeclarativeTextSvg); q->update();
}

//=================================================================================================
// WDeclarativeTextSvg
//=================================================================================================

#ifdef QT_4
/* explicit */ WDeclarativeTextSvg::WDeclarativeTextSvg(QDeclarativeItem * parent)
    : WDeclarativeItem(new WDeclarativeTextSvgPrivate(this), parent)
#else
/* explicit */ WDeclarativeTextSvg::WDeclarativeTextSvg(QQuickItem * parent)
    : WDeclarativeItemPaint(new WDeclarativeTextSvgPrivate(this), parent)
#endif
{
    Q_D(WDeclarativeTextSvg); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

#ifdef QT_4
WDeclarativeTextSvg::WDeclarativeTextSvg(WDeclarativeTextSvgPrivate * p,
                                         QDeclarativeItem           * parent)
    : WDeclarativeItem(p, parent)
#else
WDeclarativeTextSvg::WDeclarativeTextSvg(WDeclarativeTextSvgPrivate * p, QQuickItem * parent)
    : WDeclarativeItemPaint(p, parent)
#endif
{
    Q_D(WDeclarativeTextSvg); d->init();
}

//-------------------------------------------------------------------------------------------------
// QDeclarativeItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeTextSvg::componentComplete()
{
    Q_D(WDeclarativeTextSvg);

#ifdef QT_4
    WDeclarativeItem::componentComplete();
#else
    WDeclarativeItemPaint::componentComplete();
#endif

    d->load();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem / QQuickPaintedItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeTextSvg::paint(QPainter * painter,
                                              const QStyleOptionGraphicsItem *, QWidget *)
#else
/* virtual */ void WDeclarativeTextSvg::paint(QPainter * painter)
#endif
{
#ifdef QT_LATEST
    if (isVisible() == false) return;
#endif

    Q_D(WDeclarativeTextSvg);

    qreal width  = this->width ();
    qreal height = this->height();

    QRectF rect = d->getRect(width, height);

    if (clip())
    {
        painter->save();

        painter->setClipRect(QRectF(0, 0, width, height), Qt::IntersectClip);

        d->renderer->render(painter, rect);

        painter->restore();
    }
    else d->renderer->render(painter, rect);
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeTextSvg::svgChange()
{
    Q_D(WDeclarativeTextSvg);

    setImplicitWidth (d->textWidth  * d->zoom + d->marginWidth  * 2);
    setImplicitHeight(d->textHeight * d->zoom + d->marginHeight * 2);
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ QVariant WDeclarativeTextSvg::itemChange(GraphicsItemChange change,
                                                       const QVariant &   value)
#else
/* virtual */ void WDeclarativeTextSvg::itemChange(ItemChange change, const ItemChangeData & value)
#endif
{
#ifdef QT_4
    if (change == ItemVisibleHasChanged)
    {
        Q_D(WDeclarativeTextSvg);

#else
    Q_D(WDeclarativeTextSvg);

    if (d->view && change == ItemVisibleHasChanged && value.boolValue)
    {
#endif
        d->loadVisible();
    }

#ifdef QT_4
    return WDeclarativeItem::itemChange(change, value);
#else
    WDeclarativeItemPaint::itemChange(change, value);
#endif
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

int WDeclarativeTextSvg::textWidth() const
{
    Q_D(const WDeclarativeTextSvg); return d->textWidth;
}

int WDeclarativeTextSvg::textHeight() const
{
    Q_D(const WDeclarativeTextSvg); return d->textHeight;
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeTextSvg::marginWidth() const
{
    Q_D(const WDeclarativeTextSvg); return d->marginWidth;
}

void WDeclarativeTextSvg::setMarginWidth(int width)
{
    Q_D(WDeclarativeTextSvg);

    if (d->marginWidth == width) return;

    d->marginWidth = width;

    svgChange();

    update();

    emit marginWidthChanged();
}

int WDeclarativeTextSvg::marginHeight() const
{
    Q_D(const WDeclarativeTextSvg); return d->marginHeight;
}

void WDeclarativeTextSvg::setMarginHeight(int height)
{
    Q_D(WDeclarativeTextSvg);

    if (d->marginHeight == height) return;

    d->marginHeight = height;

    svgChange();

    update();

    emit marginHeightChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeTextSvg::zoom() const
{
    Q_D(const WDeclarativeTextSvg); return d->zoom;
}

void WDeclarativeTextSvg::setZoom(qreal zoom)
{
    Q_D(WDeclarativeTextSvg);

    if (d->zoom == zoom) return;

    d->zoom = zoom;

    svgChange();

    update();

    emit zoomChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeText::HAlignment WDeclarativeTextSvg::hAlign() const
{
    Q_D(const WDeclarativeTextSvg); return d->hAlign;
}

void WDeclarativeTextSvg::setHAlign(WDeclarativeText::HAlignment align)
{
    Q_D(WDeclarativeTextSvg);

    if (d->hAlign == align) return;

    d->hAlign = align;

    update();

    emit horizontalAlignmentChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeText::VAlignment WDeclarativeTextSvg::vAlign() const
{
    Q_D(const WDeclarativeTextSvg); return d->vAlign;
}

void WDeclarativeTextSvg::setVAlign(WDeclarativeText::VAlignment align)
{
    Q_D(WDeclarativeTextSvg);

    if (d->vAlign == align) return;

    d->vAlign = align;

    update();

    emit verticalAlignmentChanged();
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativeTextSvg::text() const
{
    Q_D(const WDeclarativeTextSvg); return d->text;
}

void WDeclarativeTextSvg::setText(const QString & text)
{
    Q_D(WDeclarativeTextSvg);

    if (d->text == text) return;

    d->text = text;

    if (isComponentComplete()) d->load();

    emit textChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeTextSvg::LoadMode WDeclarativeTextSvg::loadMode() const
{
    Q_D(const WDeclarativeTextSvg); return d->loadMode;
}

void WDeclarativeTextSvg::setLoadMode(LoadMode mode)
{
    Q_D(WDeclarativeTextSvg);

    if (d->loadMode == mode) return;

    d->loadMode = mode;

    if (mode != LoadVisible)
    {
        d->loadVisible();
    }

    emit loadModeChanged();
}

//-------------------------------------------------------------------------------------------------

QFont WDeclarativeTextSvg::font() const
{
    Q_D(const WDeclarativeTextSvg); return d->font;
}

void WDeclarativeTextSvg::setFont(const QFont & font)
{
    Q_D(WDeclarativeTextSvg);

    if (d->font == font) return;

    d->font = font;

    if (isComponentComplete()) d->load();

    emit fontChanged();
}

//-------------------------------------------------------------------------------------------------

QColor WDeclarativeTextSvg::color() const
{
    Q_D(const WDeclarativeTextSvg); return d->color;
}

void WDeclarativeTextSvg::setColor(const QColor & color)
{
    Q_D(WDeclarativeTextSvg);

    if (d->color == color) return;

    d->color = color;

    if (isComponentComplete()) d->load();

    emit colorChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeGradient * WDeclarativeTextSvg::gradient() const
{
    Q_D(const WDeclarativeTextSvg); return d->gradient;
}

void WDeclarativeTextSvg::setGradient(WDeclarativeGradient * gradient)
{
    Q_D(WDeclarativeTextSvg);

    if (d->gradient == gradient) return;

    if (d->gradient)
    {
        disconnect(d->gradient, 0, this, 0);
    }

    d->gradient = gradient;

    if (d->gradient)
    {
        connect(d->gradient, SIGNAL(updated()), this, SLOT(onLoad()));
    }

    if (isComponentComplete()) d->load();

    emit gradientChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeTextSvg::TextStyle WDeclarativeTextSvg::style() const
{
    Q_D(const WDeclarativeTextSvg); return d->style;
}

void WDeclarativeTextSvg::setStyle(TextStyle style)
{
    Q_D(WDeclarativeTextSvg);

    if (d->style == style) return;

    d->style = style;

    if (isComponentComplete()) d->load();

    emit styleChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeTextSvg::TextOutline WDeclarativeTextSvg::outline() const
{
    Q_D(const WDeclarativeTextSvg); return d->outline;
}

void WDeclarativeTextSvg::setOutline(TextOutline outline)
{
    Q_D(WDeclarativeTextSvg);

    if (d->outline == outline) return;

    d->outline = outline;

    if (isComponentComplete()) d->load();

    emit outlineChanged();
}

//-------------------------------------------------------------------------------------------------

QColor WDeclarativeTextSvg::styleColor() const
{
    Q_D(const WDeclarativeTextSvg); return d->styleColor;
}

void WDeclarativeTextSvg::setStyleColor(const QColor & color)
{
    Q_D(WDeclarativeTextSvg);

    if (d->styleColor == color) return;

    d->styleColor = color;

    if (isComponentComplete()) d->load();

    emit styleColorChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeTextSvg::styleSize() const
{
    Q_D(const WDeclarativeTextSvg); return d->styleSize;
}

void WDeclarativeTextSvg::setStyleSize(int size)
{
    Q_D(WDeclarativeTextSvg);

    if (d->styleSize == size) return;

    d->styleSize = size;

    if (isComponentComplete()) d->load();

    emit styleSizeChanged();
}

//=================================================================================================
// WDeclarativeTextSvgScalePrivate
//=================================================================================================

WDeclarativeTextSvgScalePrivate::WDeclarativeTextSvgScalePrivate(WDeclarativeTextSvgScale * p)
    : WDeclarativeTextSvgPrivate(p) {}

void WDeclarativeTextSvgScalePrivate::init()
{
    Q_Q(WDeclarativeTextSvgScale);

    scaling  = true;
    scalable = false;
    scaled   = false;

    scaleDelayed = true;
    scaleDelay   = wControllerView->scaleDelay();

    timer.setInterval(scaleDelay);

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), q, SLOT(onScale()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeTextSvgScalePrivate::update()
{
    if (textWidth > 0)
    {
         scalable = true;
    }
    else scalable = false;
}

void WDeclarativeTextSvgScalePrivate::restore()
{
    timer.stop();

    scalePixmap = QPixmap();
    scaleSize   = QSize  ();

    scaled = false;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeTextSvgScalePrivate::onScale()
{
    Q_Q(WDeclarativeTextSvgScale);

    qreal width  = q->width ();
    qreal height = q->height();

    scalePixmap = QPixmap(width, height);

    scalePixmap.fill(Qt::transparent);

    QPainter painter(&scalePixmap);

    QRectF rect = getRect(width, height);

    renderer->render(&painter, rect);

    scaled = true;

    q->update();
}

//=================================================================================================
// WDeclarativeTextSvgScale
//=================================================================================================

#ifdef QT_4
/* explicit */ WDeclarativeTextSvgScale::WDeclarativeTextSvgScale(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeTextSvgScale::WDeclarativeTextSvgScale(QQuickItem * parent)
#endif
    : WDeclarativeTextSvg(new WDeclarativeTextSvgScalePrivate(this), parent)
{
    Q_D(WDeclarativeTextSvgScale); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WDeclarativeTextSvgScale::applyScale()
{
    Q_D(WDeclarativeTextSvgScale);

    if (d->scalable == false || d->scaled) return;

    d->timer.stop();

    d->scaleSize = QSize(width(), height());

    d->onScale();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem / QQuickPaintedItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ void WDeclarativeTextSvgScale::paint(QPainter                       * painter,
                                                   const QStyleOptionGraphicsItem * option,
                                                   QWidget                        * widget)
#else
/* virtual */ void WDeclarativeTextSvgScale::paint(QPainter * painter)
#endif
{
#ifdef QT_LATEST
    if (isVisible() == false) return;
#endif

    Q_D(WDeclarativeTextSvgScale);

    if (d->scalable)
    {
        if (d->scaled)
        {
            bool smooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);

            painter->setRenderHint(QPainter::SmoothPixmapTransform);

            if (clip())
            {
                painter->save();

                painter->setClipRect(QRectF(0, 0, width(), height()), Qt::IntersectClip);

                painter->drawPixmap(0, 0, d->scalePixmap);

                painter->restore();
            }
            else painter->drawPixmap(0, 0, d->scalePixmap);

            painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth);

            return;
        }

        QSize size = QSize(width(), height());

        if (d->scaleSize != size)
        {
            if (d->scaleDelayed)
            {
#ifdef QT_4
                if (d->viewport->scale() == 1.0)
#else
                if (d->view->item()->scale() == 1.0)
#endif
                {
                    d->scaleSize = size;

#ifdef QT_4
                    d->timer.start();
#else
                    QTimer::singleShot(0, &d->timer, SLOT(start()));
#endif
                }
            }
            else
            {
                d->scaleSize = size;

                d->onScale();
            }
        }
    }

#ifdef QT_4
    WDeclarativeTextSvg::paint(painter, option, widget);
#else
    WDeclarativeTextSvg::paint(painter);
#endif
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeTextSvgScale::geometryChanged(const QRectF & newGeometry,
                                                             const QRectF & oldGeometry)
{
    Q_D(WDeclarativeTextSvgScale);

    WDeclarativeTextSvg::geometryChanged(newGeometry, oldGeometry);

    if (d->scalable && oldGeometry.size() != newGeometry.size())
    {
        d->restore();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected WDeclarativeImageSvg reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeTextSvgScale::svgChange()
{
    Q_D(WDeclarativeTextSvgScale);

    WDeclarativeTextSvg::svgChange();

    if (d->scaling == false) return;

    d->restore();
    d->update ();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WDeclarativeTextSvgScale::isScaling() const
{
    Q_D(const WDeclarativeTextSvgScale); return d->scaling;
}

void WDeclarativeTextSvgScale::setScaling(bool scaling)
{
    Q_D(WDeclarativeTextSvgScale);

    if (d->scaling == scaling) return;

    d->scaling = scaling;

    if (scaling == false)
    {
        d->restore();

        d->scalable = false;
    }
    else d->update();

    update();

    emit scalingChanged();
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeTextSvgScale::scaleDelayed() const
{
    Q_D(const WDeclarativeTextSvgScale); return d->scaleDelayed;
}

void WDeclarativeTextSvgScale::setScaleDelayed(bool delayed)
{
    Q_D(WDeclarativeTextSvgScale);

    if (d->scaleDelayed == delayed) return;

    d->scaleDelayed = delayed;

    if (delayed == false && d->timer.isActive())
    {
        d->timer.stop();

        d->onScale();
    }

    emit scaleDelayedChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeTextSvgScale::scaleDelay() const
{
    Q_D(const WDeclarativeTextSvgScale); return d->scaleDelay;
}

void WDeclarativeTextSvgScale::setScaleDelay(int delay)
{
    Q_D(WDeclarativeTextSvgScale);

    if (d->scaleDelay == delay) return;

    d->scaleDelay = delay;

    d->timer.setInterval(delay);

    emit scaleDelayChanged();
}

#endif // SK_NO_DECLARATIVETEXTSVG
