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

#ifndef WDECLARATIVETEXTSVG_H
#define WDECLARATIVETEXTSVG_H

// Sk includes
#include <WDeclarativeText>

#ifndef SK_NO_DECLARATIVETEXTSVG

class WDeclarativeTextSvgPrivate;

class SK_GUI_EXPORT WDeclarativeTextSvg : public WDeclarativeItem
{
    Q_OBJECT

    Q_ENUMS(TextStyle)
    Q_ENUMS(TextOutline)

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    Q_PROPERTY(TextStyle   style   READ style   WRITE setStyle   NOTIFY styleChanged)
    Q_PROPERTY(TextOutline outline READ outline WRITE setOutline NOTIFY outlineChanged)

    Q_PROPERTY(QColor styleColor READ styleColor WRITE setStyleColor NOTIFY styleColorChanged)
    Q_PROPERTY(int    styleSize  READ styleSize  WRITE setStyleSize  NOTIFY styleSizeChanged)

public: // Enums
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
    explicit WDeclarativeTextSvg(QDeclarativeItem * parent = NULL);

public: // QDeclarativeItem reimplementation
    /* virtual */ void componentComplete();

public: // QGraphicsItem reimplementation
    /* virtual */ void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                                                 QWidget                        * widget);

signals:
    void textChanged();

    void fontChanged();

    void colorChanged();

    void styleChanged  ();
    void outlineChanged();

    void styleColorChanged();
    void styleSizeChanged ();

public: // Properties
    QString text() const;
    void    setText(const QString & text);

    QFont font() const;
    void  setFont(const QFont & font);

    QColor color() const;
    void   setColor(const QColor & color);

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

    Q_PRIVATE_SLOT(d_func(), void onRepaintNeeded())
};

QML_DECLARE_TYPE(WDeclarativeTextSvg)

#include <private/WDeclarativeTextSvg_p>

#endif // SK_NO_DECLARATIVETEXTSVG
#endif // WDECLARATIVETEXTSVG_H
