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
#ifdef QT_LATEST
#include <QPixmap>
#endif
#include <QTimer>

// Private includes
#ifdef QT_4
#include <private/WDeclarativeItem_p>
#else
#include <private/WDeclarativeTexture_p>
#endif

#ifndef SK_NO_DECLARATIVETEXTSVG

// Forward declarations
class QSvgRenderer;

//-------------------------------------------------------------------------------------------------
// WDeclarativeTextSvgPrivate
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeTextSvgPrivate : public WDeclarativeItemPrivate
#else
class SK_GUI_EXPORT WDeclarativeTextSvgPrivate : public WDeclarativeTexturePrivate
#endif
{
public:
    WDeclarativeTextSvgPrivate(WDeclarativeTextSvg * p);

    void init();

public: // Functions
#ifdef QT_LATEST
    void updatePixmap(int width, int height);
#endif

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

    void setTextSize(int width, int height);

    QRectF getRect(qreal width, qreal height);

    int getWidth(const QFontMetrics & metrics, const QString & text) const;

    QString getStyle () const;
    QString getWeight() const;

    QString getOutline(const QString & color, int size) const;

public: // Slots
    void onLoad();

#ifdef QT_4
    void onUpdate();
#else
    void onTimeout();
#endif

public: // Variables
#ifdef QT_LATEST
    QPixmap pixmap;
#endif

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

#ifdef QT_LATEST
    bool scaleDelayed : 1;
    int  scaleDelay;

    bool scaleLater : 1;

    QTimer timer;
#endif

protected:
    W_DECLARE_PUBLIC(WDeclarativeTextSvg)
};

#ifdef QT_4

//-------------------------------------------------------------------------------------------------
// WDeclarativeTextSvgScalePrivate
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeTextSvgScalePrivate : public WDeclarativeTextSvgPrivate
{
public:
    WDeclarativeTextSvgScalePrivate(WDeclarativeTextSvgScale * p);

    void init();

public: // Functions
    void update ();
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

#endif

#endif // SK_NO_DECLARATIVETEXTSVG
#endif // WDECLARATIVETEXTSVG_P_H
