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

#include "WDeclarativeText.h"

#ifndef SK_NO_DECLARATIVETEXT

// Qt includes
#include <qmath.h>
#include <QApplication>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeInfo>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QTextBlock>

// Private includes
#include <private/qdeclarativeglobal_p.h>
#include <private/qdeclarativestyledtext_p.h>
#include <private/qdeclarativepixmapcache_p.h>

//=================================================================================================
// WTextDocumentWithImageResources
//=================================================================================================

class WTextDocumentWithImageResources : public QTextDocument
{
    Q_OBJECT

public:
    WTextDocumentWithImageResources(WDeclarativeText * parent);

    /* virtual */ ~WTextDocumentWithImageResources();

public: // Interface
    void setText(const QString & text);

protected:
    QVariant loadResource(int type, const QUrl &name);

private slots:
    void requestFinished();

private:
    QHash<QUrl, QDeclarativePixmap *> resources;

    int outstanding;

    static QSet<QUrl> errors;
};

QSet<QUrl> WTextDocumentWithImageResources::errors;

//=================================================================================================
// WDeclarativeTextPrivate
//=================================================================================================

#include <private/WDeclarativeItem_p>

class SK_GUI_EXPORT WDeclarativeTextPrivate : public WDeclarativeItemPrivate
{
public:
    WDeclarativeTextPrivate(WDeclarativeText * p);

    void init();

public: // Functions
    void updateSize  ();
    void updateLayout();

    bool determineHorizontalAlignment();

    bool setHAlign(WDeclarativeText::HAlignment alignment, bool forceAlign = false);

    void invalidateImageCache();
    void checkImageCache     ();

    void ensureDoc();

    QPixmap textDocumentImage(bool drawStyle);

    QRect setupTextLayout();

    QPixmap textLayoutImage(bool drawStyle);

    void drawTextLayout(QPainter * painter, const QPointF &pos, bool drawStyle);

public: // Static functions
    static QPixmap drawOutline(const QPixmap & source, const QPixmap & style);
    static QPixmap drawStyle  (const QPixmap & source, const QPixmap & style, int size, int pos);

    static inline WDeclarativeTextPrivate * get(WDeclarativeText * q);

public: // Variables
    QString text;

    QFont font;
    QFont sourceFont;

    QColor color;

    WDeclarativeText::TextStyle style;

    QColor styleColor;
    int    styleSize;

    QString activeLink;

    WDeclarativeText::HAlignment hAlign;
    WDeclarativeText::VAlignment vAlign;

    WDeclarativeText::TextFormat    format;
    WDeclarativeText::TextElideMode elideMode;

    WDeclarativeText::WrapMode wrapMode;

    qreal lineHeight;

    WDeclarativeText::LineHeightMode lineHeightMode;

    int lineCount;

    bool truncated;

    int maximumLineCount;
    int maximumLineCountValid;

    QPointF elidePos;

    static QString elideChar;

    QPixmap imageCache;

    bool imageCacheDirty : 1;

    bool updateOnComponentComplete : 1;

    bool richText : 1;

    bool singleline : 1;

    bool cacheAllTextAsImage : 1;

    bool internalWidthUpdate : 1;

    bool requireImplicitWidth : 1;

    bool hAlignImplicit : 1;

    bool rightToLeftText : 1;

    bool layoutTextElided : 1;

    QRect layedOutTextRect;

    QSize paintedSize;

    qreal naturalWidth;

    WTextDocumentWithImageResources * doc;

    QTextLayout layout;

protected:
    W_DECLARE_PUBLIC(WDeclarativeText)
};

DEFINE_BOOL_CONFIG_OPTION(enableImageCache, QML_ENABLE_TEXT_IMAGE_CACHE);

QString WDeclarativeTextPrivate::elideChar = QString(0x2026);

//=================================================================================================
// WTextDocumentWithImageResources
//=================================================================================================

WTextDocumentWithImageResources::WTextDocumentWithImageResources(WDeclarativeText * parent)
    : QTextDocument(parent)
{
    outstanding = 0;
}

WTextDocumentWithImageResources::~WTextDocumentWithImageResources()
{
    if (resources.isEmpty() == false)
    {
        qDeleteAll(resources);
    }
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WTextDocumentWithImageResources::setText(const QString &text)
{
    if (resources.isEmpty() == false)
    {
        qDeleteAll(resources);

        resources.clear();

        outstanding = 0;
    }

#ifndef QT_NO_TEXTHTMLPARSER
    setHtml(text);
#else
    setPlainText(text);
#endif
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

QVariant WTextDocumentWithImageResources::loadResource(int type, const QUrl & name)
{
    QDeclarativeContext * context = qmlContext(parent());

    QUrl url = context->resolvedUrl(name);

    if (type == QTextDocument::ImageResource)
    {
        QHash<QUrl, QDeclarativePixmap *>::Iterator iter = resources.find(url);

        if (iter == resources.end())
        {
            QDeclarativePixmap * p = new QDeclarativePixmap(context->engine(), url);

            iter = resources.insert(name, p);

            if (p->isLoading())
            {
                p->connectFinished(this, SLOT(requestFinished()));

                outstanding++;
            }
        }

        QDeclarativePixmap * p = *iter;

        if (p->isReady())
        {
            return p->pixmap();
        }
        else if (p->isError())
        {
            if (errors.contains(url) == false)
            {
                errors.insert(url);

                qmlInfo(parent()) << p->error();
            }
        }
    }

    return QTextDocument::loadResource(type,url);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WTextDocumentWithImageResources::requestFinished()
{
    outstanding--;

    if (outstanding != 0) return;

    WDeclarativeText * item = static_cast<WDeclarativeText *> (parent());

    QString text = item->text();

#ifndef QT_NO_TEXTHTMLPARSER
    setHtml(text);
#else
    setPlainText(text);
#endif

    WDeclarativeTextPrivate * d = WDeclarativeTextPrivate::get(item);

    d->updateLayout();
}

//=================================================================================================
// WDeclarativeTextPrivate
//=================================================================================================

WDeclarativeTextPrivate::WDeclarativeTextPrivate(WDeclarativeText * p)
    : WDeclarativeItemPrivate(p) {}

void WDeclarativeTextPrivate::init()
{
    Q_Q(WDeclarativeText);

    style = WDeclarativeText::Normal;

    styleSize = 1;

    hAlign = WDeclarativeText::AlignLeft;
    vAlign = WDeclarativeText::AlignTop;

    format    = WDeclarativeText::AutoText;
    elideMode = WDeclarativeText::ElideNone;

    wrapMode = WDeclarativeText::NoWrap;

    lineHeight = 1;

    lineHeightMode = WDeclarativeText::ProportionalHeight;

    lineCount = 1;

    truncated = false;

    maximumLineCount      = INT_MAX;
    maximumLineCountValid = false;

    imageCacheDirty = true;

    updateOnComponentComplete = true;

    richText = false;

    singleline = false;

    cacheAllTextAsImage = true;

    internalWidthUpdate = false;

    requireImplicitWidth = false;

    hAlignImplicit = true;

    rightToLeftText = false;

    layoutTextElided = false;

    naturalWidth = 0.0;

    doc = NULL;

    cacheAllTextAsImage = enableImageCache();

    q->setAcceptedMouseButtons(Qt::LeftButton);

    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeTextPrivate::updateSize()
{
    Q_Q(WDeclarativeText);

    if (q->isComponentComplete() == false)
    {
        updateOnComponentComplete = true;

        return;
    }

    if (requireImplicitWidth == false)
    {
        emit q->implicitWidthChanged();

        if (requireImplicitWidth) return;
    }

    invalidateImageCache();

    QFontMetrics fm(font);

    if (text.isEmpty())
    {
        q->setImplicitWidth(0);

        q->setImplicitHeight(fm.height());

        paintedSize = QSize(0, fm.height());

        emit q->paintedSizeChanged();

        q->update();

        return;
    }

    int dy = q->height();

    QSize size(0, 0);

    if (richText == false)
    {
        QRect textRect = setupTextLayout();

        if (layedOutTextRect.size() != textRect.size())
        {
            q->prepareGeometryChange();
        }

        layedOutTextRect = textRect;

        size = textRect.size();

        dy -= size.height();
    }
    else
    {
        singleline = false;

        ensureDoc();

        doc->setDefaultFont(font);

        WDeclarativeText::HAlignment horizontalAlignment = q->effectiveHAlign();

        if (rightToLeftText)
        {
            if (horizontalAlignment == WDeclarativeText::AlignLeft)
            {
                horizontalAlignment = WDeclarativeText::AlignRight;
            }
            else if (horizontalAlignment == WDeclarativeText::AlignRight)
            {
                horizontalAlignment = WDeclarativeText::AlignLeft;
            }
        }

        QTextOption option;

        option.setAlignment((Qt::Alignment) int(horizontalAlignment | vAlign));

        option.setWrapMode(QTextOption::WrapMode(wrapMode));

        doc->setDefaultTextOption(option);

        if (requireImplicitWidth && q->widthValid())
        {
            doc->setTextWidth(-1);

            naturalWidth = doc->idealWidth();
        }

        if (q->widthValid())
        {
             doc->setTextWidth(q->width());
        }
        else doc->setTextWidth(doc->idealWidth());

        dy -= (int) doc->size().height();

        QSize dsize = doc->size().toSize();

        if (dsize != layedOutTextRect.size())
        {
            q->prepareGeometryChange();

            layedOutTextRect = QRect(QPoint(0, 0), dsize);
        }

        size = QSize(int(doc->idealWidth()), dsize.height());
    }

    int yoff = 0;

    if (q->heightValid())
    {
        if (vAlign == WDeclarativeText::AlignBottom)
        {
            yoff = dy;
        }
        else if (vAlign == WDeclarativeText::AlignVCenter)
        {
            yoff = dy / 2;
        }
    }

    q->setBaselineOffset(fm.ascent() + yoff);

    internalWidthUpdate = true;

    if (q->widthValid() == false)
    {
        q->setImplicitWidth(size.width());
    }
    else if (requireImplicitWidth)
    {
        q->setImplicitWidth(naturalWidth);
    }

    internalWidthUpdate = false;

    q->setImplicitHeight(size.height());

    if (paintedSize != size)
    {
        paintedSize = size;

        emit q->paintedSizeChanged();
    }

    q->update();
}

void WDeclarativeTextPrivate::updateLayout()
{
    Q_Q(WDeclarativeText);

    if (q->isComponentComplete() == false)
    {
        updateOnComponentComplete = true;

        return;
    }

    layoutTextElided = false;

    if (richText)
    {
        updateSize();

        return;
    }

    bool oldTruncated = truncated;

    layout.clearLayout();

    layout.setFont(font);

    //if (format != WDeclarativeText::StyledText)
    {
        QString tmp = text;

        tmp.replace(QLatin1Char('\n'), QChar::LineSeparator);

        if (tmp.contains(QChar::LineSeparator))
        {
             singleline = false;
        }
        else singleline = true;

        if (singleline && maximumLineCountValid == false
            &&
            elideMode != WDeclarativeText::ElideNone && q->widthValid())
        {
            QFontMetrics fm(font);

            tmp = fm.elidedText(tmp, (Qt::TextElideMode) elideMode, q->width());

            if (tmp != text)
            {
                layoutTextElided = true;

                truncated = true;
            }
            else truncated = false;
        }
        else truncated = false;

        layout.setText(tmp);
    }
    /*else
    {
        singleline = false;

        QDeclarativeStyledText::parse(text, layout);
    }*/

    updateSize();

    if (truncated != oldTruncated)
    {
        emit q->truncatedChanged();
    }
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeTextPrivate::determineHorizontalAlignment()
{
    Q_Q(WDeclarativeText);

    if (hAlignImplicit == false || q->isComponentComplete() == false)
    {
        return false;
    }

    bool alignToRight;

    if (text.isEmpty())
    {
#ifdef QT_4
        if (QApplication::keyboardInputDirection() == Qt::RightToLeft)
#else
        if (QApplication::inputMethod()->inputDirection() == Qt::RightToLeft)
#endif
        {
            alignToRight = true;

            return setHAlign(WDeclarativeText::AlignRight);
        }
        else
        {
            alignToRight = false;

            return setHAlign(WDeclarativeText::AlignLeft);
        }
    }
    else
    {
        alignToRight = rightToLeftText;

        if (alignToRight)
        {
             return setHAlign(WDeclarativeText::AlignRight);
        }
        else return setHAlign(WDeclarativeText::AlignLeft);
    }
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeTextPrivate::setHAlign(WDeclarativeText::HAlignment alignment, bool forceAlign)
{
    Q_Q(WDeclarativeText);

    if (hAlign != alignment || forceAlign)
    {
        hAlign = alignment;

        emit q->horizontalAlignmentChanged();

        return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeTextPrivate::invalidateImageCache()
{
    Q_Q(WDeclarativeText);

    //if (cacheAllTextAsImage || style != WDeclarativeText::Normal)
    {
        if (imageCacheDirty) return;

        imageCacheDirty = true;

        imageCache = QPixmap();
    }

    if (q->isComponentComplete())
    {
        q->update();
    }
}

void WDeclarativeTextPrivate::checkImageCache()
{
    if (imageCacheDirty == false) return;

    if (text.isEmpty() == false)
    {
        QPixmap pixmap;
        QPixmap pixmapStyle;

        if (richText)
        {
            pixmap = textDocumentImage(false);

            if (style != WDeclarativeText::Normal)
            {
                pixmapStyle = textDocumentImage(true);
            }
        }
        else
        {
            pixmap = textLayoutImage(false);

            if (style != WDeclarativeText::Normal)
            {
                pixmapStyle = textLayoutImage(true);
            }
        }

        if (style == WDeclarativeText::Outline)
        {
            imageCache = drawOutline(pixmap, pixmapStyle);
        }
        else if (style == WDeclarativeText::Sunken)
        {
            imageCache = drawStyle(pixmap, pixmapStyle, styleSize, -styleSize);
        }
        else if (style == WDeclarativeText::Raised)
        {
            imageCache = drawStyle(pixmap, pixmapStyle, styleSize, styleSize);
        }
        else imageCache = pixmap;
    }
    else imageCache = QPixmap();

    imageCacheDirty = false;
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeTextPrivate::ensureDoc()
{
    if (doc) return;

    Q_Q(WDeclarativeText);

    doc = new WTextDocumentWithImageResources(q);

    doc->setDocumentMargin(0);
}

//-------------------------------------------------------------------------------------------------

QPixmap WDeclarativeTextPrivate::textDocumentImage(bool drawStyle)
{
    QSize size = doc->size().toSize();

    QPixmap pixmap(size);

    pixmap.fill(Qt::transparent);

#ifdef Q_OS_MAC
    bool oldSmooth = qt_applefontsmoothing_enabled;

    qt_applefontsmoothing_enabled = false;
#endif

    QPainter painter(&pixmap);

#ifdef Q_OS_MAC
    qt_applefontsmoothing_enabled = oldSmooth;
#endif

    QAbstractTextDocumentLayout::PaintContext context;

    QTextOption oldOption(doc->defaultTextOption());

    if (drawStyle)
    {
        context.palette.setColor(QPalette::Text, styleColor);

        QTextOption colorOption(doc->defaultTextOption());

        colorOption.setFlags(QTextOption::SuppressColors);

        doc->setDefaultTextOption(colorOption);
    }
    else context.palette.setColor(QPalette::Text, color);

    doc->documentLayout()->draw(&painter, context);

    if (drawStyle)
    {
        doc->setDefaultTextOption(oldOption);
    }

    return pixmap;
}

//-------------------------------------------------------------------------------------------------

QRect WDeclarativeTextPrivate::setupTextLayout()
{
    Q_Q(WDeclarativeText);

    layout.setCacheEnabled(true);

    qreal lineWidth = 0;

    int visibleCount = 0;

    if (q->widthValid())
    {
        lineWidth = q->width();
    }

    QTextOption textOption = layout.textOption();

    textOption.setAlignment(Qt::Alignment(q->effectiveHAlign()));

    textOption.setWrapMode(QTextOption::WrapMode(wrapMode));

    layout.setTextOption(textOption);

    bool elideText = false;
    bool truncate  = false;

    QFontMetrics fm(layout.font());

    elidePos = QPointF();

    if (requireImplicitWidth && q->widthValid())
    {
        QString elidedText;

        if (layoutTextElided)
        {
            elidedText = layout.text();

            layout.setText(text);
        }

        layout.beginLayout();

        forever
        {
            QTextLine line = layout.createLine();

            if (line.isValid() == false) break;
        }

        layout.endLayout();

        QRectF br;

        for (int i = 0; i < layout.lineCount(); ++i)
        {
            QTextLine line = layout.lineAt(i);

            br = br.united(line.naturalTextRect());
        }

        naturalWidth = br.width();

        if (layoutTextElided)
        {
            layout.setText(elidedText);
        }
    }

    if (maximumLineCountValid)
    {
        layout.beginLayout();

        if (lineWidth == false)
        {
            lineWidth = INT_MAX;
        }

        int linesLeft = maximumLineCount;

        int visibleTextLength = 0;

        while (linesLeft > 0)
        {
            QTextLine line = layout.createLine();

            if (line.isValid() == false) break;

            visibleCount++;

            if (lineWidth)
            {
                line.setLineWidth(lineWidth);
            }

            visibleTextLength += line.textLength();

            if (--linesLeft == 0)
            {
                if (visibleTextLength < text.length())
                {
                    truncate = true;

                    if (elideMode == WDeclarativeText::ElideRight && q->widthValid())
                    {
                        qreal elideWidth = fm.width(elideChar);

                        line.setLineWidth(lineWidth - elideWidth);

                        if (layout.text().mid(line.textStart (),
                                              line.textLength()).isRightToLeft())
                        {
                            line.setPosition(QPointF(line.position().x() + elideWidth,
                                                     line.position().y()));

                            elidePos.setX(line.naturalTextRect().left() - elideWidth);
                        }
                        else elidePos.setX(line.naturalTextRect().right());

                        elideText = true;
                    }
                }
            }
        }

        layout.endLayout();

        if (truncated != truncate)
        {
            truncated = truncate;

            emit q->truncatedChanged();
        }
    }
    else
    {
        layout.beginLayout();

        forever
        {
            QTextLine line = layout.createLine();

            if (line.isValid() == false) break;

            visibleCount++;

            if (lineWidth)
            {
                line.setLineWidth(lineWidth);
            }
        }

        layout.endLayout();
    }

    qreal height = 0;

    QRectF br;

    for (int i = 0; i < layout.lineCount(); ++i)
    {
        QTextLine line = layout.lineAt(i);

        line.setPosition(QPointF(line.position().x(), height));

        if (elideText && i == layout.lineCount() - 1)
        {
            elidePos.setY(height + fm.ascent());

            br = br.united(QRectF(elidePos, QSizeF(fm.width(elideChar), fm.ascent())));
        }

        br = br.united(line.naturalTextRect());

        if (lineHeightMode == WDeclarativeText::FixedHeight)
        {
             height += lineHeight;
        }
        else height += line.height() * lineHeight;
    }

    br.setHeight(height);

    if (q->widthValid() == false)
    {
        naturalWidth = br.width();
    }

    if (lineCount != visibleCount)
    {
        lineCount = visibleCount;

        emit q->lineCountChanged();
    }

    return QRect(br.x(), br.y(), br.width(), br.height());
}

//-------------------------------------------------------------------------------------------------

QPixmap WDeclarativeTextPrivate::textLayoutImage(bool drawStyle)
{
    QSize size = layedOutTextRect.size();

    QPixmap pixmap(size);

    if (size.isEmpty())
    {
        return pixmap;
    }

    pixmap.fill(Qt::transparent);

#ifdef Q_OS_MAC
    bool oldSmooth = qt_applefontsmoothing_enabled;

    qt_applefontsmoothing_enabled = false;
#endif

    QPainter painter(&pixmap);

#ifdef Q_OS_MAC
    qt_applefontsmoothing_enabled = oldSmooth;
#endif

    drawTextLayout(&painter, QPointF(-layedOutTextRect.x(), 0), drawStyle);

    return pixmap;
}

//-------------------------------------------------------------------------------------------------

void WDeclarativeTextPrivate::drawTextLayout(QPainter      * painter,
                                             const QPointF & pos, bool drawStyle)
{
    if (drawStyle)
    {
         painter->setPen(styleColor);
    }
    else painter->setPen(color);

    painter->setFont(font);

    layout.draw(painter, pos);

    if (elidePos.isNull() == false)
    {
        painter->drawText(pos + elidePos, elideChar);
    }
}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ QPixmap WDeclarativeTextPrivate::drawOutline(const QPixmap & source,
                                                          const QPixmap & style)
{
    QPixmap pixmap = QPixmap(style.width() + 2, style.height() + 2);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    QPoint pos(0, 0);

    pos += QPoint(-1, 0);

    painter.drawPixmap(pos, style);

    pos += QPoint(2, 0);

    painter.drawPixmap(pos, style);

    pos += QPoint(-1, -1);

    painter.drawPixmap(pos, style);

    pos += QPoint(0, 2);

    painter.drawPixmap(pos, style);

    pos += QPoint(0, -1);

    painter.drawPixmap(pos, source);

    painter.end();

    return pixmap;
}

/* static */ QPixmap WDeclarativeTextPrivate::drawStyle(const QPixmap & source,
                                                        const QPixmap & style, int size, int pos)
{
    int size2x = size * 2;

    QPixmap pixmap = QPixmap(style.width() + size2x, style.height() + size2x);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.drawPixmap(QPoint(0, pos), style);

    painter.drawPixmap(0, 0, source);

    painter.end();

    return pixmap;
}

//-------------------------------------------------------------------------------------------------

/* static inline */ WDeclarativeTextPrivate * WDeclarativeTextPrivate::get(WDeclarativeText * q)
{
    return q->d_func();
}

//=================================================================================================
// WDeclarativeText
//=================================================================================================

/* explicit */ WDeclarativeText::WDeclarativeText(QDeclarativeItem * parent)
    : WDeclarativeItem(new WDeclarativeTextPrivate(this), parent)
{
    Q_D(WDeclarativeText); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WDeclarativeText::linkAt(const QPoint & pos) const
{
    Q_D(const WDeclarativeText);

    if (d->doc)
    {
         return d->doc->documentLayout()->anchorAt(pos);
    }
    else return QString();
}

/* Q_INVOKABLE */ bool WDeclarativeText::setBlockForeground(const QPoint & pos,
                                                            const QColor & color)
{
    Q_D(WDeclarativeText);

    if (d->doc == NULL) return false;

    int at = d->doc->documentLayout()->hitTest(pos, Qt::ExactHit);

    if (at == -1) return false;

    QTextCursor cursor(d->doc);

    QTextBlock block = d->doc->findBlock(at);

    at = block.position();

    cursor.setPosition(at);
    cursor.setPosition(at + block.length() - 1, QTextCursor::KeepAnchor);

    QTextCharFormat format;

    format.setForeground(color);

    cursor.mergeCharFormat(format);

    d->updateSize();

    return true;
}

//-------------------------------------------------------------------------------------------------
// QDeclarativeItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeText::componentComplete()
{
    Q_D(WDeclarativeText);

    WDeclarativeItem::componentComplete();

    if (d->updateOnComponentComplete == false) return;

    d->updateOnComponentComplete = false;

    if (d->richText)
    {
        d->ensureDoc();

        d->doc->setText(d->text);

        d->rightToLeftText = d->doc->toPlainText().isRightToLeft();
    }
    else d->rightToLeftText = d->text.isRightToLeft();

    d->determineHorizontalAlignment();

    d->updateLayout();
}

//-------------------------------------------------------------------------------------------------
// QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QRectF WDeclarativeText::boundingRect() const
{
    Q_D(const WDeclarativeText);

    QRect rect = d->layedOutTextRect;

    if (d->style != Normal)
    {
        rect.adjust(-1, 0, 1, 2);
    }

    if (d->vAlign == AlignBottom)
    {
        rect.moveTop(height() - rect.height());
    }
    else if (d->vAlign == AlignVCenter)
    {
        rect.moveTop((height() - rect.height()) / 2);
    }

    return QRectF(rect);
}

/* virtual */ void WDeclarativeText::paint(QPainter * painter,
                                           const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(WDeclarativeText);

    //if (d->cacheAllTextAsImage || d->style != Normal)
    {
        d->checkImageCache();

        if (d->imageCache.isNull()) return;

        QRect br = boundingRect().toRect();

        bool needClip = clip() && (d->imageCache.width() > width()
                                   ||
                                   d->imageCache.height() > height());

        if (needClip)
        {
             painter->drawPixmap(0, 0, width(), height(), d->imageCache, -br.x(), -br.y(),
                                 width(), height());
        }
        else painter->drawPixmap(br.x(), br.y(), d->imageCache);
    }
    /*else
    {
        QRectF bounds = boundingRect();

        if (clip()
            &&
            (d->layedOutTextRect.width() > width() || d->layedOutTextRect.height() > height()))
        {
            painter->save();

            painter->setClipRect(0, 0, width(), height(), Qt::IntersectClip);

            if (d->richText)
            {
                QAbstractTextDocumentLayout::PaintContext context;

                context.palette.setColor(QPalette::Text, d->color);

                painter->translate(bounds.x(), bounds.y());

                d->doc->documentLayout()->draw(painter, context);

                painter->translate(-bounds.x(), -bounds.y());
            }
            else d->drawTextLayout(painter, QPointF(0, bounds.y()), false);

            painter->restore();
        }
        else if (d->richText)
        {
            QAbstractTextDocumentLayout::PaintContext context;

            context.palette.setColor(QPalette::Text, d->color);

            painter->translate(bounds.x(), bounds.y());

            d->doc->documentLayout()->draw(painter, context);

            painter->translate(-bounds.x(), -bounds.y());
        }
        else d->drawTextLayout(painter, QPointF(0, bounds.y()), false);
    }*/
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeText::geometryChanged(const QRectF & newGeometry,
                                                     const QRectF & oldGeometry)
{
    Q_D(WDeclarativeText);

    if (d->internalWidthUpdate || newGeometry.width() == oldGeometry.width())
    {
        WDeclarativeItem::geometryChanged(newGeometry, oldGeometry);

        return;
    }

    WDeclarativeText::TextElideMode elide;

    if (d->elideMode != WDeclarativeText::ElideNone && d->singleline && d->truncated == false)
    {
        if (newGeometry.width() < oldGeometry.width())
        {
            QFontMetrics metrics(d->font);

            if (metrics.width(d->text) <= newGeometry.width())
            {
                 elide = WDeclarativeText::ElideNone;
            }
            else elide = d->elideMode;
        }
        else elide = WDeclarativeText::ElideNone;
    }
    else elide = d->elideMode;

    if (elide != WDeclarativeText::ElideNone)
    {
        if ((d->singleline || d->maximumLineCountValid) && widthValid())
        {
            d->updateLayout();
        }
        else d->updateSize();
    }
    else if (d->wrapMode != WDeclarativeText::NoWrap || d->hAlign != WDeclarativeText::AlignLeft)
    {
        if ((d->singleline || d->maximumLineCountValid)
            &&
            elide != WDeclarativeText::ElideNone && widthValid())
        {
            d->updateLayout();
        }
        else d->updateSize();
    }

    WDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeText::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    Q_D(WDeclarativeText);

    if (d->richText == false || d->doc == NULL
        ||
        d->doc->documentLayout()->anchorAt(event->pos()).isEmpty())
    {
        event->setAccepted(false);

        d->activeLink.clear();
    }
    else d->activeLink = d->doc->documentLayout()->anchorAt(event->pos());

    if (event->isAccepted() == false)
    {
        WDeclarativeItem::mousePressEvent(event);
    }
}

/* virtual */ void WDeclarativeText::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    Q_D(WDeclarativeText);

    if (d->richText && d->doc
        &&
        d->activeLink == d->doc->documentLayout()->anchorAt(event->pos()))
    {
        emit linkActivated(d->activeLink);
    }
    else event->setAccepted(false);

    if (event->isAccepted() == false)
    {
        WDeclarativeItem::mouseReleaseEvent(event);
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

qreal WDeclarativeText::implicitWidth() const
{
    Q_D(const WDeclarativeText);

    if (d->requireImplicitWidth == false)
    {
        WDeclarativeTextPrivate * p = const_cast<WDeclarativeTextPrivate*> (d);

        p->requireImplicitWidth = true;

        p->updateSize();
    }

    return WDeclarativeItem::implicitWidth();
}

qreal WDeclarativeText::implicitHeight() const
{
    return WDeclarativeItem::implicitHeight();
}

//-------------------------------------------------------------------------------------------------

QString WDeclarativeText::text() const
{
    Q_D(const WDeclarativeText); return d->text;
}

void WDeclarativeText::setText(const QString & text)
{
    Q_D(WDeclarativeText);

    if (d->text == text) return;

    if (d->format == RichText || (d->format == AutoText && Qt::mightBeRichText(text)))
    {
         d->richText = true;
    }
    else d->richText = false;

    d->text = text;

    if (isComponentComplete())
    {
        if (d->richText)
        {
            d->ensureDoc();

            d->doc->setText(text);

            d->rightToLeftText = d->doc->toPlainText().isRightToLeft();
        }
        else d->rightToLeftText = d->text.isRightToLeft();

        d->determineHorizontalAlignment();
    }

    d->updateLayout();

    emit textChanged();
}

//-------------------------------------------------------------------------------------------------

QFont WDeclarativeText::font() const
{
    Q_D(const WDeclarativeText); return d->sourceFont;
}

void WDeclarativeText::setFont(const QFont & font)
{
    Q_D(WDeclarativeText);

    if (d->sourceFont == font) return;

    d->sourceFont = font;

    QFont oldFont = d->font;

    d->font = font;

    if (d->font.pointSizeF() != -1)
    {
        qreal size = qRound(d->font.pointSizeF() * 2.0);

        d->font.setPointSizeF(size / 2.0);
    }

    if (oldFont != d->font)
    {
        d->updateLayout();
    }

    emit fontChanged();
}

//-------------------------------------------------------------------------------------------------

QColor WDeclarativeText::color() const
{
    Q_D(const WDeclarativeText); return d->color;
}

void WDeclarativeText::setColor(const QColor & color)
{
    Q_D(WDeclarativeText);

    if (d->color == color) return;

    d->color = color;

    d->invalidateImageCache();

    emit colorChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeText::TextStyle WDeclarativeText::style() const
{
    Q_D(const WDeclarativeText); return d->style;
}

void WDeclarativeText::setStyle(WDeclarativeText::TextStyle style)
{
    Q_D(WDeclarativeText);

    if (d->style == style) return;

    if (isComponentComplete() && (d->style == Normal || style == Normal))
    {
        prepareGeometryChange();
    }

    d->style = style;

    d->invalidateImageCache();

    emit styleChanged();
}

//-------------------------------------------------------------------------------------------------

QColor WDeclarativeText::styleColor() const
{
    Q_D(const WDeclarativeText); return d->styleColor;
}

void WDeclarativeText::setStyleColor(const QColor & color)
{
    Q_D(WDeclarativeText);

    if (d->styleColor == color) return;

    d->styleColor = color;

    d->invalidateImageCache();

    emit styleColorChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeText::styleSize() const
{
    Q_D(const WDeclarativeText); return d->styleSize;
}

void WDeclarativeText::setStyleSize(int size)
{
    Q_D(WDeclarativeText);

    if (d->styleSize == size) return;

    d->styleSize = size;

    d->invalidateImageCache();

    emit styleSizeChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeText::HAlignment WDeclarativeText::hAlign() const
{
    Q_D(const WDeclarativeText); return d->hAlign;
}

void WDeclarativeText::setHAlign(HAlignment align)
{
    Q_D(WDeclarativeText);

    bool forceAlign = false;

    /*if (d->hAlignImplicit && d->effectiveLayoutMirror)
    {
         forceAlign = true;
    }
    else forceAlign = false;*/

    d->hAlignImplicit = false;

    if (d->setHAlign(align, forceAlign) && isComponentComplete())
    {
        d->updateLayout();
    }
}

void WDeclarativeText::resetHAlign()
{
    Q_D(WDeclarativeText);

    d->hAlignImplicit = true;

    if (d->determineHorizontalAlignment() && isComponentComplete())
    {
        d->updateLayout();
    }
}

//-------------------------------------------------------------------------------------------------

WDeclarativeText::HAlignment WDeclarativeText::effectiveHAlign() const
{
    Q_D(const WDeclarativeText);

    WDeclarativeText::HAlignment effectiveAlignment = d->hAlign;

    /*if (d->hAlignImplicit == false && d->effectiveLayoutMirror)
    {
        if (d->hAlign == WDeclarativeText::AlignLeft)
        {
            effectiveAlignment = WDeclarativeText::AlignRight;
        }
        else if (d->hAlign == WDeclarativeText::AlignRight)
        {
            effectiveAlignment = WDeclarativeText::AlignLeft;
        }
    }*/

    return effectiveAlignment;
}

//-------------------------------------------------------------------------------------------------

WDeclarativeText::VAlignment WDeclarativeText::vAlign() const
{
    Q_D(const WDeclarativeText); return d->vAlign;
}

void WDeclarativeText::setVAlign(VAlignment align)
{
    Q_D(WDeclarativeText);

    if (d->vAlign == align) return;

    if (isComponentComplete())
    {
        prepareGeometryChange();
    }

    d->vAlign = align;

    emit verticalAlignmentChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeText::WrapMode WDeclarativeText::wrapMode() const
{
    Q_D(const WDeclarativeText); return d->wrapMode;
}

void WDeclarativeText::setWrapMode(WrapMode mode)
{
    Q_D(WDeclarativeText);

    if (mode == d->wrapMode) return;

    d->wrapMode = mode;

    d->updateLayout();

    emit wrapModeChanged();
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeText::lineCount() const
{
    Q_D(const WDeclarativeText); return d->lineCount;
}

//-------------------------------------------------------------------------------------------------

bool WDeclarativeText::truncated() const
{
    Q_D(const WDeclarativeText); return d->truncated;
}

//-------------------------------------------------------------------------------------------------

int WDeclarativeText::maximumLineCount() const
{
    Q_D(const WDeclarativeText); return d->maximumLineCount;
}

void WDeclarativeText::setMaximumLineCount(int lines)
{
    Q_D(WDeclarativeText);

    if (lines == INT_MAX) d->maximumLineCountValid = false;
    else                  d->maximumLineCountValid = true;

    if (d->maximumLineCount != lines)
    {
        d->maximumLineCount = lines;

        d->updateLayout();

        emit maximumLineCountChanged();
    }
}

void WDeclarativeText::resetMaximumLineCount()
{
    Q_D(WDeclarativeText);

    setMaximumLineCount(INT_MAX);

    d->elidePos = QPointF();

    if (d->truncated != false)
    {
        d->truncated = false;

        emit truncatedChanged();
    }
}

//-------------------------------------------------------------------------------------------------

WDeclarativeText::TextFormat WDeclarativeText::textFormat() const
{
    Q_D(const WDeclarativeText); return d->format;
}

void WDeclarativeText::setTextFormat(TextFormat format)
{
    Q_D(WDeclarativeText);

    if (format == d->format) return;

    d->format = format;

    bool wasRich = d->richText;

    if (format == RichText || (format == AutoText && Qt::mightBeRichText(d->text)))
    {
         d->richText = true;
    }
    else d->richText = false;

    if (wasRich == false && d->richText && isComponentComplete())
    {
        d->ensureDoc();

        d->doc->setText(d->text);
    }

    d->updateLayout();

    emit textFormatChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeText::TextElideMode WDeclarativeText::elideMode() const
{
    Q_D(const WDeclarativeText); return d->elideMode;
}

void WDeclarativeText::setElideMode(WDeclarativeText::TextElideMode mode)
{
    Q_D(WDeclarativeText);

    if (mode == d->elideMode) return;

    d->elideMode = mode;

    d->updateLayout();

    emit elideModeChanged();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeText::paintedWidth() const
{
    Q_D(const WDeclarativeText); return d->paintedSize.width();
}

qreal WDeclarativeText::paintedHeight() const
{
    Q_D(const WDeclarativeText); return d->paintedSize.height();
}

//-------------------------------------------------------------------------------------------------

qreal WDeclarativeText::lineHeight() const
{
    Q_D(const WDeclarativeText); return d->lineHeight;
}

void WDeclarativeText::setLineHeight(qreal lineHeight)
{
    Q_D(WDeclarativeText);

    if (d->lineHeight == lineHeight || lineHeight < 0.0) return;

    d->lineHeight = lineHeight;

    d->updateLayout();

    emit lineHeightChanged();
}

//-------------------------------------------------------------------------------------------------

WDeclarativeText::LineHeightMode WDeclarativeText::lineHeightMode() const
{
    Q_D(const WDeclarativeText); return d->lineHeightMode;
}

void WDeclarativeText::setLineHeightMode(LineHeightMode mode)
{
    Q_D(WDeclarativeText);

    if (mode == d->lineHeightMode) return;

    d->lineHeightMode = mode;

    d->updateLayout();

    emit lineHeightModeChanged();
}

#endif // SK_NO_DECLARATIVETEXT

#include "WDeclarativeText.moc"
