//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

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

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeTextSvgPrivate::WDeclarativeTextSvgPrivate(WDeclarativeTextSvg * p)
    : WDeclarativeItemPrivate(p) {}

void WDeclarativeTextSvgPrivate::init()
{
    Q_Q(WDeclarativeTextSvg);

    renderer = new QSvgRenderer(q);

    style   = WDeclarativeTextSvg::Normal;
    outline = WDeclarativeTextSvg::OutlineNormal;

    styleSize = 1;

    hAlign = WDeclarativeText::AlignLeft;
    vAlign = WDeclarativeText::AlignTop;

    QObject::connect(renderer, SIGNAL(repaintNeeded()), q, SLOT(onRepaintNeeded()));

    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QRectF WDeclarativeTextSvgPrivate::getRect(qreal width, qreal height) const
{
    qreal x;
    qreal y;

    int textWidth  = this->width;
    int textHeight = this->height;

    if (hAlign == WDeclarativeText::AlignRight)
    {
        x = width - textWidth;
    }
    else if (hAlign == WDeclarativeText::AlignHCenter)
    {
        x = (width - textWidth) / 2;
    }
    else x = 0;

    if (vAlign == WDeclarativeText::AlignBottom)
    {
        y = height - textHeight;
    }
    else if (vAlign == WDeclarativeText::AlignVCenter)
    {
        y = (height - textHeight) / 2;
    }
    else y = 0;

    return QRectF(x, y, textWidth, textHeight);
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeTextSvgPrivate::load()
{
    Q_Q(WDeclarativeTextSvg);

    if (text.isEmpty())
    {
        width  = 0;
        height = 0;

        renderer->load(QByteArray());
    }
    else
    {
        QFontMetrics metrics(font);

        width  = metrics.width (text);
        height = metrics.height();

        QString family = font.family();

        QString weight = getWeight();

        int pixelSize = font.pixelSize();

        int pixelY = pixelSize - (height - pixelSize) / 2;

        QString fontSize = QString::number(pixelSize);

        QString colorItem = color.name();

        QString item;
        QString itemStyle;

        if (style == WDeclarativeTextSvg::Outline)
        {
            QString x = QString::number(styleSize);
            QString y = QString::number(pixelY);

            QString colorStyle = styleColor.name();

            QString extra = getOutline(colorStyle, styleSize);

            item = getText(x, y, family, weight, fontSize, colorItem, extra);
        }
        else
        {
            QString y = QString::number(pixelY);

            if (style == WDeclarativeTextSvg::Raised)
            {
                QString colorStyle = styleColor.name();

                item = getText("0", y, family, weight, fontSize, colorItem);

                y = QString::number(pixelY + styleSize);

                itemStyle = getText("0", y, family, weight, fontSize, colorStyle);
            }
            else if (style == WDeclarativeTextSvg::Sunken)
            {
                QString colorStyle = styleColor.name();

                item = getText("0", y, family, weight, fontSize, colorItem);

                y = QString::number(pixelY - styleSize);

                itemStyle = getText("0", y, family, weight, fontSize, colorStyle);
            }
            else if (style == WDeclarativeTextSvg::Glow)
            {
                QString x = QString::number(styleSize);

                QString colorStyle = styleColor.name();

                item = getText(x, y, family, weight, fontSize, colorItem);

                int sizeGlow = styleSize * 2;

                QString extra = getOutline(colorStyle, sizeGlow);

                itemStyle = getText(x, y, family, weight, fontSize, colorItem, extra);

                width += sizeGlow;
            }
            else item = getText("0", y, family, weight, fontSize, colorItem);
        }

        QString stringWidth  = QString::number(width);
        QString stringHeight = QString::number(height);

        QByteArray content;

        content.append("<svg x=\"0\" y=\"0\" width=\"" + stringWidth + "\" height=\""
                       +
                       stringHeight + "\">" + itemStyle + item + "</svg>");

        renderer->load(content);
    }

    q->setImplicitWidth (width);
    q->setImplicitHeight(height);
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativeTextSvgPrivate::getText(const QString & x,
                                            const QString & y,
                                            const QString & family,
                                            const QString & weight,
                                            const QString & size,
                                            const QString & color, const QString & extra) const
{
    return "<text x=\"" + x + "\" y=\"" + y + "\" font-family=\"" + family + "\" font-weight=\""
            +
            weight + "\" font-size=\"" + size + "\" fill=\"" + color + "\"" + extra + ">"
            +
            text.toUtf8() + "</text>";
}

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

QString WDeclarativeTextSvgPrivate::getWeight() const
{
    int weight = font.weight();

    if      (weight == QFont::Light) return "lighter";
    else if (weight == QFont::Bold)  return "bold";
    else if (weight == QFont::Black) return "bolder";
    else                             return "normal";
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeTextSvgPrivate::onRepaintNeeded()
{
    Q_Q(WDeclarativeTextSvg); q->update();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WDeclarativeTextSvg::WDeclarativeTextSvg(QDeclarativeItem * parent)
    : WDeclarativeItem(new WDeclarativeTextSvgPrivate(this), parent)
{
    Q_D(WDeclarativeTextSvg); d->init();
}

//-------------------------------------------------------------------------------------------------
// QDeclarativeItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeTextSvg::componentComplete()
{
    Q_D(WDeclarativeTextSvg);

    WDeclarativeItem::componentComplete();

    d->load();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeTextSvg::paint(QPainter * painter,
                                              const QStyleOptionGraphicsItem *, QWidget *)
{
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
// Properties
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

    emit verticalAlignmentChanged();
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

#endif // SK_NO_DECLARATIVETEXTSVG
