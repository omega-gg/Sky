//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WCONTROLLERDECLARATIVE_H
#define WCONTROLLERDECLARATIVE_H

// Sk includes
#include <WController>

#ifndef SK_NO_CONTROLLERDECLARATIVE

// Forward declarations
class QDeclarativeEngine;
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

    enum Corner { TopLeft, TopRight, BottomLeft, BottomRight };

    enum IconStyle
    {
        IconNormal,
        IconRaised,
        IconSunken
    };

private:
    WControllerDeclarative();

protected: // Initialize
    void init();

public: // Interface
    void clearComponentCache();

    void setContextProperty(const QString & name, const QVariant & value);
    void setContextProperty(const QString & name, QObject        * value);

public: // Properties
    QDeclarativeEngine * engine() const;

private:
    W_DECLARE_PRIVATE   (WControllerDeclarative)
    W_DECLARE_CONTROLLER(WControllerDeclarative)
};

#endif // SK_NO_CONTROLLERDECLARATIVE
#endif // WCONTROLLERDECLARATIVE_H
