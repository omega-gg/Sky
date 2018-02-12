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

#ifndef WDECLARATIVETEXT_H
#define WDECLARATIVETEXT_H

// Qt includes
#include <QTextOption>

// Sk includes
#ifdef QT_4
#include <WDeclarativeItem>
#else
#include <WDeclarativeItemPaint>
#endif

#ifndef SK_NO_DECLARATIVETEXT

class WDeclarativeTextPrivate;

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeText : public WDeclarativeItem
#else
class SK_GUI_EXPORT WDeclarativeText : public WDeclarativeItemPaint
#endif
{
    Q_OBJECT

    Q_ENUMS(HAlignment)
    Q_ENUMS(VAlignment)

    Q_ENUMS(TextStyle)
    Q_ENUMS(TextFormat)
    Q_ENUMS(TextElideMode)

    Q_ENUMS(WrapMode)

    Q_ENUMS(LineHeightMode)

    Q_PROPERTY(qreal implicitWidth  READ implicitWidth  NOTIFY implicitWidthChanged  /* REVISION 1 */)
    Q_PROPERTY(qreal implicitHeight READ implicitHeight NOTIFY implicitHeightChanged /* REVISION 1 */)

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    Q_PROPERTY(TextStyle style READ style WRITE setStyle NOTIFY styleChanged)

    Q_PROPERTY(QColor styleColor READ styleColor WRITE setStyleColor NOTIFY styleColorChanged)
    Q_PROPERTY(int    styleSize  READ styleSize  WRITE setStyleSize  NOTIFY styleSizeChanged)

    Q_PROPERTY(HAlignment horizontalAlignment READ hAlign WRITE setHAlign RESET resetHAlign
               NOTIFY horizontalAlignmentChanged)

    Q_PROPERTY(VAlignment verticalAlignment READ vAlign WRITE setVAlign
               NOTIFY verticalAlignmentChanged)

    Q_PROPERTY(WrapMode wrapMode READ wrapMode WRITE setWrapMode NOTIFY wrapModeChanged)

    Q_PROPERTY(int lineCount READ lineCount NOTIFY lineCountChanged /* REVISION 1 */)

    Q_PROPERTY(bool truncated READ truncated NOTIFY truncatedChanged /* REVISION 1 */)

    Q_PROPERTY(int maximumLineCount READ maximumLineCount WRITE setMaximumLineCount
               NOTIFY maximumLineCountChanged RESET resetMaximumLineCount /* REVISION 1 */)

    Q_PROPERTY(TextFormat textFormat READ textFormat WRITE setTextFormat NOTIFY textFormatChanged)

    Q_PROPERTY(TextElideMode elide READ elideMode WRITE setElideMode NOTIFY elideModeChanged)

    Q_PROPERTY(qreal paintedWidth  READ paintedWidth  NOTIFY paintedSizeChanged)
    Q_PROPERTY(qreal paintedHeight READ paintedHeight NOTIFY paintedSizeChanged)

    Q_PROPERTY(qreal lineHeight READ lineHeight WRITE setLineHeight NOTIFY lineHeightChanged
               /* REVISION 1 */)

    Q_PROPERTY(LineHeightMode lineHeightMode READ lineHeightMode WRITE setLineHeightMode
               NOTIFY lineHeightModeChanged /* REVISION 1 */)

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

    enum TextStyle { Normal, Outline, Raised, Sunken };

    enum TextFormat
    {
        PlainText  = Qt::PlainText,
        RichText   = Qt::RichText,
        AutoText   = Qt::AutoText
        /* StyledText = 4 */
    };

    enum TextElideMode
    {
        ElideLeft   = Qt::ElideLeft,
        ElideRight  = Qt::ElideRight,
        ElideMiddle = Qt::ElideMiddle,
        ElideNone   = Qt::ElideNone
    };

    enum WrapMode
    {
        NoWrap                       = QTextOption::NoWrap,
        WordWrap                     = QTextOption::WordWrap,
        WrapAnywhere                 = QTextOption::WrapAnywhere,
        WrapAtWordBoundaryOrAnywhere = QTextOption::WrapAtWordBoundaryOrAnywhere,
        Wrap                         = QTextOption::WrapAtWordBoundaryOrAnywhere
    };

    enum LineHeightMode { ProportionalHeight, FixedHeight };

public:
#ifdef QT_4
    explicit WDeclarativeText(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeText(QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE QString linkAt(const QPoint & pos) const;

    Q_INVOKABLE bool setBlockForeground(const QPoint & pos, const QColor & color);

public: // QDeclarativeItem / QQuickItem reimplementation
    /* virtual */ void componentComplete();

public: // QGraphicsItem / QQuickPaintedItem reimplementation
    /* virtual */ QRectF boundingRect() const;

#ifdef QT_4
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);
#else
    /* virtual */ void paint(QPainter * painter);
#endif

protected: // QGraphicsItem / QQuickItem reimplementation
    /* virtual */ void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);

protected: // Events
#ifdef QT_4
    /* virtual */ void mousePressEvent  (QGraphicsSceneMouseEvent * event);
    /* virtual */ void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
#else
    /* virtual */ void mousePressEvent  (QMouseEvent * event);
    /* virtual */ void mouseReleaseEvent(QMouseEvent * event);
#endif

signals:
    void linkActivated(const QString & link);

    /* Q_REVISION(1) */ void implicitWidthChanged ();
    /* Q_REVISION(1) */ void implicitHeightChanged();

    void textChanged();

    void fontChanged();

    void colorChanged();

    void styleChanged();

    void styleColorChanged();
    void styleSizeChanged ();

    void horizontalAlignmentChanged();
    void verticalAlignmentChanged  ();

    void wrapModeChanged();

    /* Q_REVISION(1) */ void lineCountChanged();

    /* Q_REVISION(1) */ void truncatedChanged();

    /* Q_REVISION(1) */ void maximumLineCountChanged();

    void textFormatChanged();

    void elideModeChanged();

    void paintedSizeChanged();

    /* Q_REVISION(1) */ void lineHeightChanged();

    /* Q_REVISION(1) */ void lineHeightModeChanged();

public: // Properties
    qreal implicitWidth () const;
    qreal implicitHeight() const;

    QString text() const;
    void    setText(const QString & text);

    QFont font() const;
    void  setFont(const QFont & font);

    QColor color() const;
    void   setColor(const QColor & color);

    TextStyle style() const;
    void      setStyle(TextStyle style);

    QColor styleColor() const;
    void   setStyleColor(const QColor & color);

    int  styleSize() const;
    void setStyleSize(int size);

    HAlignment hAlign() const;
    void       setHAlign(HAlignment align);
    void       resetHAlign();

    HAlignment effectiveHAlign() const;

    VAlignment vAlign() const;
    void       setVAlign(VAlignment align);

    WrapMode wrapMode() const;
    void     setWrapMode(WrapMode mode);

    int lineCount() const;

    bool truncated() const;

    int  maximumLineCount() const;
    void setMaximumLineCount(int lines);
    void resetMaximumLineCount();

    TextFormat textFormat() const;
    void       setTextFormat(TextFormat format);

    TextElideMode elideMode() const;
    void          setElideMode(TextElideMode mode);

    qreal paintedWidth () const;
    qreal paintedHeight() const;

    qreal lineHeight() const;
    void  setLineHeight(qreal lineHeight);

    LineHeightMode lineHeightMode() const;
    void           setLineHeightMode(LineHeightMode mode);

private:
    W_DECLARE_PRIVATE(WDeclarativeText)
};

#endif // SK_NO_DECLARATIVETEXT
#endif // WDECLARATIVETEXT_H
