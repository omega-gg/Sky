//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#ifndef WCONTROLLERDECLARATIVE_H
#define WCONTROLLERDECLARATIVE_H

// Sk includes
#include <WController>

#ifndef SK_NO_CONTROLLERDECLARATIVE

// Forward declarations
#ifdef QT_4
class QDeclarativeEngine;
#else
class QQmlEngine;
#endif
class WControllerDeclarativePrivate;

// Defines
#define wControllerDeclarative WControllerDeclarative::instance()

class SK_CORE_EXPORT WControllerDeclarative : public WController
{
    Q_OBJECT

    Q_ENUMS(Direction)
    Q_ENUMS(Corner)
    Q_ENUMS(IconStyle)

public: // Enums
    enum Direction { Left, Right, Up, Down };

    enum Corner { TopLeft, TopRight, BottomLeft, BottomRight,
                  TopLeftCorner, TopRightCorner, BottomLeftCorner, BottomRightCorner };

    enum IconStyle
    {
        IconNormal,
        IconRaised,
        IconSunken
    };

private:
    WControllerDeclarative();

protected: // Initialize
    /* virtual */ void init();

public: // Interface
    void clearComponentCache();

    void setContextProperty(const QString & name, const QVariant & value);
    void setContextProperty(const QString & name, QObject        * value);

public: // Properties
#ifdef QT_4
    QDeclarativeEngine * engine() const;
#else
    QQmlEngine * engine() const;
#endif

private:
    W_DECLARE_PRIVATE   (WControllerDeclarative)
    W_DECLARE_CONTROLLER(WControllerDeclarative)
};

#endif // SK_NO_CONTROLLERDECLARATIVE
#endif // WCONTROLLERDECLARATIVE_H
