#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

Sky="../../.."

SkyBase="$Sky/src/SkyBase"

SkyComponents="$Sky/src/SkyComponents"

SkyPresentation="$Sky/src/SkyPresentation"

#--------------------------------------------------------------------------------------------------
# environment

qt="qt6"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 1 -a $# != 2 ] \
   || \
   [ $1 != "win32" -a $1 != "win64" -a $1 != "macOS" -a $1 != "linux" -a $1 != "android" ] \
   || \
   [ $# = 2 -a "$2" != "all" -a "$2" != "clean" ]; then

    echo "Usage: generate <win32 | win64 | macOS | linux | android> [all | clean]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $1 = "win32" -o $1 = "win64" ]; then

    os="windows"
else
    os="default"
fi

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

if [ "$2" = "clean" ]; then

    echo "CLEANING"

    rm -f *.qml

    exit 0
fi

#--------------------------------------------------------------------------------------------------
# QML
#--------------------------------------------------------------------------------------------------

echo "COPYING QML"

cp ../qml/*.qml .

#--------------------------------------------------------------------------------------------------
# Deployer
#--------------------------------------------------------------------------------------------------

if [ $qt = "qt4" ]; then

    imports="QtQuick=1.1"

    defines="QT_4 QT_OLD"
else
    if [ $qt = "qt5" ]; then

        defines="QT_5 QT_OLD QT_NEW"
    else
        defines="QT_6 QT_NEW"
    fi

    if [ $1 = "linux" ]; then

        imports="QtQuick=2.7"
    else
        imports="QtQuick=2.14"
    fi

    if [ $qt = "qt5" ]; then

        imports="$imports QtMultimedia=5.4"
    else
        imports="$imports QtMultimedia=6.0"
    fi
fi

if [ $os = "windows" ]; then

    defines="$defines DESKTOP WINDOWS"

elif [ $1 = "macOS" ]; then

    defines="$defines DESKTOP MAC"

elif [ $1 = "linux" ]; then

    defines="$defines DESKTOP LINUX"
else
    defines="$defines MOBILE ANDROID"
fi

files="\
"$SkyBase"/Style.qml \
"$SkyBase"/WindowSky.qml \
"$SkyBase"/RectangleBorders.qml \
"$SkyBase"/ImageBarcode.qml \
"$SkyBase"/ImageTag.qml \
"$SkyBase"/TextBase.qml \
"$SkyBase"/BaseButton.qml \
"$SkyBase"/BaseLineEdit.qml \
"$SkyComponents"/StyleComponents.qml \
"$SkyComponents"/LineVertical.qml \
"$SkyComponents"/BorderVertical.qml \
"$SkyComponents"/BorderImageScaleBack.qml \
"$SkyComponents"/BorderImageShadow.qml \
"$SkyComponents"/LineEditBox.qml"

"$Sky"/deploy/deployer . "$imports" construct.qrc "$defines" $files
