#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.14.1"

MinGW_version="7.3.0"

VLC_version="3.0.6"

libtorrent_version="1.2.2"

Boost_version="1.71.0"

#--------------------------------------------------------------------------------------------------

bin4="bin"
bin5="latest"

#--------------------------------------------------------------------------------------------------
# Linux

include32="/usr/include/i386-linux-gnu"
include64="/usr/include/x86_64-linux-gnu"

Qt5_version_linux="5.9.5"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 2 ] || [ $1 != "qt4" -a $1 != "qt5" -a $1 != "clean" ] || [ $2 != "win32" -a \
                                                                       $2 != "win64" -a \
                                                                       $2 != "macOS" -a \
                                                                       $2 != "linux" ]; then

    echo "Usage: configure <qt4 | qt5 | clean> <win32 | win64 | macOS | linux>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

external="$external/$2"

if [ $2 = "win32" ]; then

    os="windows"

    MinGW="$external/MinGW/$MinGW_version/i686-w64-mingw32/lib"

elif [ $2 = "win64" ]; then

    os="windows"

    MinGW="$external/MinGW/$MinGW_version/x86_64-w64-mingw32/lib"

elif [ $2 = "macOS" ]; then

    os="default"

elif [ $2 = "linux" ]; then

    os="default"

    if [ $1 = "qt5" ]; then

        Qt5_version="$Qt5_version_linux"
    fi

    if [ -d "${include64}" ]; then

        include="$include64"
    else
        include="$include32"
    fi
fi

#--------------------------------------------------------------------------------------------------

Qt4="$external/Qt/$Qt4_version"
Qt5="$external/Qt/$Qt5_version"

zlib="$external/zlib"

VLC="$external/VLC/$VLC_version"

libtorrent="$external/libtorrent/$libtorrent_version"

Boost="$external/Boost/$Boost_version"

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

if [ $1 = "qt4" -a $2 = "linux" ]; then

    echo "COPYING Qt4"

    mkdir -p include/Qt4/QtCore/private
    mkdir -p include/Qt4/QtGui/private
    mkdir -p include/Qt4/QtDeclarative/private

    cp "$Qt4"/src/corelib/kernel/*_p.h include/Qt4/QtCore/private

    cp "$Qt4"/src/gui/kernel/*_p.h include/Qt4/QtGui/private

    cp "$Qt4"/src/declarative/qml/*_p.h           include/Qt4/QtDeclarative/private
    cp "$Qt4"/src/declarative/graphicsitems/*_p.h include/Qt4/QtDeclarative/private
    cp "$Qt4"/src/declarative/util/*_p.h          include/Qt4/QtDeclarative/private

elif [ $1 = "qt5" ]; then

    echo "COPYING Qt5"

    mkdir -p include/Qt5/QtCore/private
    mkdir -p include/Qt5/QtGui/private
    mkdir -p include/Qt5/QtQml/private
    mkdir -p include/Qt5/QtQuick/private

    if [ $os = "windows" ]; then

        cp -r "$Qt5"/include/QtCore  include/Qt5
        cp -r "$Qt5"/include/QtGui   include/Qt5
        cp -r "$Qt5"/include/QtQml   include/Qt5
        cp -r "$Qt5"/include/QtQuick include/Qt5

        cp -r "$Qt5"/include/QtGui/$Qt5_version/QtGui/qpa include/Qt5/QtGui

    elif [ $2 = "linux" ]; then

        cp -r "$include"/qt5/QtCore  include/Qt5
        cp -r "$include"/qt5/QtGui   include/Qt5
        cp -r "$include"/qt5/QtQml   include/Qt5
        cp -r "$include"/qt5/QtQuick include/Qt5

        cp -r "$include"/qt5/QtGui/$Qt5_version/QtGui/qpa include/Qt5/QtGui

    elif [ $2 = "macOS" ]; then

        cp -r "$Qt5"/lib/QtCore.framework/Headers/*  include/Qt5/QtCore
        cp -r "$Qt5"/lib/QtGui.framework/Headers/*   include/Qt5/QtGui
        cp -r "$Qt5"/lib/QtQml.framework/Headers/*   include/Qt5/QtQml
        cp -r "$Qt5"/lib/QtQuick.framework/Headers/* include/Qt5/QtQuick

        cp -r "$Qt5"/lib/QtGui.framework/Headers/$Qt5_version/QtGui/qpa include/Qt5/QtGui
    fi

    mv include/Qt5/QtCore/$Qt5_version/QtCore/private/*   include/Qt5/QtCore/private
    mv include/Qt5/QtGui/$Qt5_version/QtGui/private/*     include/Qt5/QtGui/private
    mv include/Qt5/QtQml/$Qt5_version/QtQml/private/*     include/Qt5/QtQml/private
    mv include/Qt5/QtQuick/$Qt5_version/QtQuick/private/* include/Qt5/QtQuick/private
fi

#--------------------------------------------------------------------------------------------------
# zlib
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ]; then

    cp "$MinGW"/libz.a lib
fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ]; then

    echo "COPYING VLC"

    cp -r "$VLC"/sdk/include/vlc include

    cp "$VLC"/sdk/lib/libvlc* lib

elif [ $2 = "macOS" ]; then

    echo "COPYING VLC"

    cp -r "$VLC"/include/vlc include

    cp "$VLC"/lib/libvlc.5.dylib     lib/libvlc.dylib
    cp "$VLC"/lib/libvlccore.9.dylib lib/libvlccore.dylib
fi

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ] || [ $2 = "macOS" ]; then

    echo "COPYING libtorrent"

    cp -r "$libtorrent"/libtorrent include

    cp "$libtorrent"/libtorrent.* lib
fi

#--------------------------------------------------------------------------------------------------
# Boost
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ] || [ $2 = "macOS" ]; then

    echo "COPYING Boost"

    cp -r "$Boost"/Boost include

    cp "$Boost"/libboost*.* lib
fi
