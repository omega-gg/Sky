#!/bin/sh

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

Qt5="$external/Qt/5.5.1"

Qt5_version="5.5.1"

VLC="$external/VLC/2.2.3"

#--------------------------------------------------------------------------------------------------

bin4="bin"
bin5="latest"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 2 ] || [ $1 != "qt4" -a $1 != "qt5" -a $1 != "clean" ] || [ $2 != "win32" ]; then

    echo "Usage: configure <qt4 | qt5 | clean> <win32>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

if [ $1 = "clean" ]; then

    echo "CLEANING"

    rm -rf lib
    mkdir  lib
    touch  lib/.gitignore

    rm -rf "$bin4"
    mkdir  "$bin4"
    touch  "$bin4"/.gitignore

    rm -rf "$bin5"
    mkdir  "$bin5"
    touch  "$bin5"/.gitignore

    #----------------------------------------------------------------------------------------------
    # Qt

    rm -rf include/Qt

    #----------------------------------------------------------------------------------------------
    # VLC

    rm -rf include/vlc

    exit 0
fi

#--------------------------------------------------------------------------------------------------
# Qt
#--------------------------------------------------------------------------------------------------

echo "COPYING Qt"

if [ $1 = "qt5" ]; then

    mkdir include/Qt

    mkdir include/Qt/QtCore
    mkdir include/Qt/QtCore/private

    cp "$Qt5"/include/QtCore/* include/Qt/QtCore

    cp "$Qt5"/include/QtCore/"$Qt5_version"/QtCore/private/* include/Qt/QtCore/private

    mkdir include/Qt/QtDeclarative
    mkdir include/Qt/QtDeclarative/private

    cp "$Qt5"/include/QtDeclarative/* include/Qt/QtDeclarative

    cp "$Qt5"/include/QtDeclarative/"$Qt5_version"/QtDeclarative/private/* include/Qt/QtDeclarative/private
fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

echo "COPYING VLC"

cp -r "$VLC"/sdk/include/vlc include

if [ $2 = "win32" ]; then

    cp "$VLC"/sdk/lib/libvlc*.* lib
fi
