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

#ifndef WDECLARATIVETEXTSVG_P_H
#define WDECLARATIVETEXTSVG_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QTimer>

// Private includes
#include <private/WDeclarativeItem_p>

#ifndef SK_NO_DECLARATIVETEXTSVG

// Forward declarations
class QSvgRenderer;

//-------------------------------------------------------------------------------------------------
// WDeclarativeTextSvgPrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeTextSvgPrivate : public WDeclarativeItemPrivate
{
protected:
    WDeclarativeTextSvgPrivate(WDeclarativeTextSvg * p);

    void init();

public: // Functions
    void setTextSize(int width, int height);

    QRectF getRect(qreal width, qreal height);

    void load       ();
    void loadVisible();

    void loadSvg();

    QString addGradient(QString * item) const;

    void addText(QString * item, const QString & x,
                                 const QString & y,
                                 const QString & family,
                                 const QString & style,
                                 const QString & weight,
                                 const QString & size,
                                 const QString & color, const QString & extra = QString()) const;

    int getWidth(const QFontMetrics & metrics, const QString & text) const;

    QString getStyle () const;
    QString getWeight() const;

    QString getOutline(const QString & color, int size) const;

public: // Slots
    void onLoad  ();
    void onUpdate();

public: // Variables
    QSvgRenderer * renderer;

    WDeclarativeTextSvg::LoadMode loadMode;

    bool loadLater : 1;

    int textWidth;
    int textHeight;

    int marginWidth;
    int marginHeight;

    QString text;

    QFont font;

    QColor color;

    WDeclarativeGradient * gradient;

    WDeclarativeTextSvg::TextStyle   style;
    WDeclarativeTextSvg::TextOutline outline;

    QColor styleColor;
    int    styleSize;

    WDeclarativeText::HAlignment hAlign;
    WDeclarativeText::VAlignment vAlign;

    qreal zoom;

protected:
    W_DECLARE_PUBLIC(WDeclarativeTextSvg)
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeTextSvgScalePrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeTextSvgScalePrivate : public WDeclarativeTextSvgPrivate
{
protected:
    WDeclarativeTextSvgScalePrivate(WDeclarativeTextSvgScale * p);

    void init();

public: // Functions
    void restore();

public: // Slots
    void onScale();

public: // Variables
    QPixmap scalePixmap;
    QSize   scaleSize;

    bool scaling  : 1;
    bool scalable : 1;
    bool scaled   : 1;

    bool scaleDelayed : 1;
    int  scaleDelay;

    QTimer timer;

protected:
    W_DECLARE_PUBLIC(WDeclarativeTextSvgScale)
};

#endif // SK_NO_DECLARATIVETEXTSVG
#endif // WDECLARATIVETEXTSVG_P_H
