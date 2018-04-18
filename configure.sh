#!/bin/sh

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.10.1"

VLC_version="2.2.8"

libtorrent_version="1.1.6"

Boost_version="1.55.0"

#--------------------------------------------------------------------------------------------------

bin4="bin"
bin5="latest"

#--------------------------------------------------------------------------------------------------
# Linux

include32="/usr/include/i386-linux-gnu"
include64="/usr/include/x86_64-linux-gnu"

Qt5_version_linux="5.9.1"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 2 ] || [ $1 != "qt4" -a $1 != "qt5" -a $1 != "clean" ] || [ $2 != "win32" -a \
                                                                       $2 != "osx"   -a \
                                                                       $2 != "linux" ]; then

    echo "Usage: configure <qt4 | qt5 | clean> <win32 | osx | linux>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

Qt4="$external/Qt/$Qt4_version"
Qt5="$external/Qt/$Qt5_version"

VLC="$external/VLC/$VLC_version"

libtorrent="$external/libtorrent/$libtorrent_version"

Boost="$external/Boost/$Boost_version"

#--------------------------------------------------------------------------------------------------

if [ $2 = "linux" ]; then

    if [ $2 = "qt5" ]; then

        Qt5_version="$Qt5_version_linux"
    fi

    if [ -d "${include64}" ]; then

        include="$include64"
    else
        include="$include32"
    fi
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

    rm -rf include/Qt4
    rm -rf include/Qt5

    #----------------------------------------------------------------------------------------------
    # VLC

    rm -rf include/vlc

    #----------------------------------------------------------------------------------------------
    # libtorrent

    rm -rf include/libtorrent

    #----------------------------------------------------------------------------------------------
    # Boost

    rm -rf include/Boost

    exit 0
fi

#--------------------------------------------------------------------------------------------------
# Qt
#--------------------------------------------------------------------------------------------------

echo "COPYING Qt"

if [ $1 = "qt4" -a $2 = "linux" ]; then

    mkdir -p include/Qt4/QtCore/private
    mkdir -p include/Qt4/QtGui/private
    mkdir -p include/Qt4/QtDeclarative/private

    cp "$Qt4"/src/corelib/kernel/*_p.h include/Qt4/QtCore/private

    cp "$Qt4"/src/gui/kernel/*_p.h include/Qt4/QtGui/private

    cp "$Qt4"/src/declarative/qml/*_p.h           include/Qt4/QtDeclarative/private
    cp "$Qt4"/src/declarative/graphicsitems/*_p.h include/Qt4/QtDeclarative/private
    cp "$Qt4"/src/declarative/util/*_p.h          include/Qt4/QtDeclarative/private

elif [ $1 = "qt5" ]; then

    mkdir -p include/Qt5/QtCore/private
    mkdir -p include/Qt5/QtGui/private
    mkdir -p include/Qt5/QtQml/private
    mkdir -p include/Qt5/QtQuick/private

    if [ $2 = "win32" ]; then

        cp "$Qt5"/include/QtCore/*  include/Qt5/QtCore
        cp "$Qt5"/include/QtGui/*   include/Qt5/QtGui
        cp "$Qt5"/include/QtQml/*   include/Qt5/QtQml
        cp "$Qt5"/include/QtQuick/* include/Qt5/QtQuick

        cp "$Qt5"/include/QtCore/$Qt5_version/QtCore/private/*   include/Qt5/QtCore/private
        cp "$Qt5"/include/QtGui/$Qt5_version/QtGui/private/*     include/Qt5/QtGui/private
        cp "$Qt5"/include/QtQml/$Qt5_version/QtQml/private/*     include/Qt5/QtQml/private
        cp "$Qt5"/include/QtQuick/$Qt5_version/QtQuick/private/* include/Qt5/QtQuick/private

        cp -r "$Qt5"/include/QtGui/$Qt5_version/QtGui/qpa include/Qt5/QtGui

    elif [ $2 = "linux" ]; then

        cp "$include"/qt5/QtCore/*  include/Qt5/QtCore
        cp "$include"/qt5/QtGui/*   include/Qt5/QtGui
        cp "$include"/qt5/QtQml/*   include/Qt5/QtQml
        cp "$include"/qt5/QtQuick/* include/Qt5/QtQuick

        cp "$include"/qt5/QtCore/$Qt5_version/QtCore/private/*   include/Qt5/QtCore/private
        cp "$include"/qt5/QtGui/$Qt5_version/QtGui/private/*     include/Qt5/QtGui/private
        cp "$include"/qt5/QtQml/$Qt5_version/QtQml/private/*     include/Qt5/QtQml/private
        cp "$include"/qt5/QtQuick/$Qt5_version/QtQuick/private/* include/Qt5/QtQuick/private

        cp -r "$include"/qt5/QtGui/$Qt5_version/QtGui/qpa include/Qt5/QtGui

    elif [ $2 = "osx" ]; then

        Qt5=/usr/local/opt/qt\@5.5
    fi
fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

if [ $2 = "win32" ]; then

    echo "COPYING VLC"

    cp -r "$VLC"/sdk/include/vlc include

    cp "$VLC"/sdk/lib/libvlc* lib

elif [ $2 = "osx" ]; then

    echo "COPYING VLC"

    cp -r /Applications/VLC.app/Contents/MacOS/include include

    cp -r /Applications/VLC.app/Contents/MacOS/lib lib
fi

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

if [ $2 = "win32" ]; then

    echo "COPYING libtorrent"

    cp -r "$libtorrent"/libtorrent include

    cp "$libtorrent"/libtorrent.* lib

elif [ $2 = "osx" ]; then

    echo "COPYING libtorrent"

    cp -r /usr/local/include/libtorrent include

    cp /usr/local/lib/libtorrent-* lib
fi

#--------------------------------------------------------------------------------------------------
# Boost
#--------------------------------------------------------------------------------------------------

if [ $2 = "win32" ]; then

    echo "COPYING Boost"

    cp -r "$Boost"/Boost include

    cp "$Boost"/libboost*.* lib

elif [ $2 = "osx" ]; then

    echo "COPYING Boost"

    cp -r /usr/local/opt/boost\@1.55/include include

    cp -r /usr/local/opt/boost\@1.55/lib lib
fi
