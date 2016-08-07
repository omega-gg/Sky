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

#ifndef WDECLARATIVETEXTSVG_P_H
#define WDECLARATIVETEXTSVG_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Sk includes
#include <private/WDeclarativeItem_p>

#ifndef SK_NO_DECLARATIVETEXTSVG

// Forward declarations
class QSvgRenderer;

class SK_GUI_EXPORT WDeclarativeTextSvgPrivate : public WDeclarativeItemPrivate
{
protected:
    WDeclarativeTextSvgPrivate(WDeclarativeTextSvg * p);

    void init();

public: // Functions
    void load();

    QString getText(const QString & x,
                    const QString & y,
                    const QString & family,
                    const QString & weight,
                    const QString & size,
                    const QString & color, const QString & extra = QString()) const;

    QString getOutline(const QString & color, int size) const;

    QString getWeight() const;

public: // Slots
    void onRepaintNeeded();

public: // Variables
    QSvgRenderer * renderer;

    int width;
    int height;

    QString text;

    QFont font;

    QColor color;

    WDeclarativeTextSvg::TextStyle style;

    QColor styleColor;
    int    styleSize;

    WDeclarativeTextSvg::TextOutline outline;

protected:
    W_DECLARE_PUBLIC(WDeclarativeTextSvg)
};

#endif // SK_NO_DECLARATIVETEXTSVG
#endif // WDECLARATIVETEXTSVG_P_H
