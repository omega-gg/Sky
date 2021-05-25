#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.15.2"

VLC_version="3.0.12"

libtorrent_version="1.2.13"

Boost_version="1.71.0"

#--------------------------------------------------------------------------------------------------
# Windows

MinGW_version="7.3.0"

#--------------------------------------------------------------------------------------------------
# environment

compiler_win="mingw"

qt="qt5"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

copyAndroid()
{
    cp -r "$1"/armeabi-v7a "$2"
    cp -r "$1"/arm64-v8a   "$2"
    cp -r "$1"/x86         "$2"
    cp -r "$1"/x86_64      "$2"
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 1 -a $# != 2 ] \
   || \
   [ $1 != "win32" -a $1 != "win64" -a $1 != "macOS" -a $1 != "linux" -a $1 != "android" ] \
   || \
   [ $# = 2 -a "$2" != "clean" ]; then

    echo "Usage: configure <win32 | win64 | macOS | linux | android> [clean]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

external="$external/$1"

if [ $1 = "win32" -o $1 = "win64" ]; then

    os="windows"

    compiler="$compiler_win"

    if [ $1 = "win32" ]; then

        MinGW="$external/MinGW/$MinGW_version/i686-w64-mingw32/lib"
    else
        MinGW="$external/MinGW/$MinGW_version/x86_64-w64-mingw32/lib"
    fi

elif [ $1 = "macOS" ]; then

    os="default"

    compiler="default"

elif [ $1 = "linux" ]; then

    os="default"

    compiler="default"
else
    os="default"

    compiler="default"
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

echo "CLEANING"

rm -rf lib
mkdir  lib
touch  lib/.gitignore

rm -rf bin/*
touch  bin/.gitignore

# NOTE: We have to remove the folder to delete .qmake.stash.
rm -rf build
mkdir  build
touch  build/.gitignore

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

if [ "$2" = "clean" ]; then

    exit 0
fi

#--------------------------------------------------------------------------------------------------
# Qt
#--------------------------------------------------------------------------------------------------

if [ $qt = "qt4" -a $1 = "linux" ]; then

    echo "COPYING Qt4"

    mkdir -p include/Qt4/QtCore/private
    mkdir -p include/Qt4/QtGui/private
    mkdir -p include/Qt4/QtDeclarative/private

    cp "$Qt4"/src/corelib/kernel/*_p.h include/Qt4/QtCore/private

    cp "$Qt4"/src/gui/kernel/*_p.h include/Qt4/QtGui/private

    cp "$Qt4"/src/declarative/qml/*_p.h           include/Qt4/QtDeclarative/private
    cp "$Qt4"/src/declarative/graphicsitems/*_p.h include/Qt4/QtDeclarative/private
    cp "$Qt4"/src/declarative/util/*_p.h          include/Qt4/QtDeclarative/private

elif [ $qt = "qt5" ]; then

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

        cp -r "$Qt5"/include/QtGui/5*/QtGui/qpa include/Qt5/QtGui

    elif [ $1 = "linux" ]; then

        cp -r "$Qt5"/include/QtCore  include/Qt5
        cp -r "$Qt5"/include/QtGui   include/Qt5
        cp -r "$Qt5"/include/QtQml   include/Qt5
        cp -r "$Qt5"/include/QtQuick include/Qt5

        cp -r "$Qt5"/include/QtGui/5*/QtGui/qpa include/Qt5/QtGui

    elif [ $1 = "macOS" ]; then

        cp -r "$Qt5"/lib/QtCore.framework/Headers/*  include/Qt5/QtCore
        cp -r "$Qt5"/lib/QtGui.framework/Headers/*   include/Qt5/QtGui
        cp -r "$Qt5"/lib/QtQml.framework/Headers/*   include/Qt5/QtQml
        cp -r "$Qt5"/lib/QtQuick.framework/Headers/* include/Qt5/QtQuick

        cp -r "$Qt5"/lib/QtGui.framework/Headers/5*/QtGui/qpa include/Qt5/QtGui

    elif [ $1 = "android" ]; then

        cp -r "$Qt5"/include/QtCore  include/Qt5
        cp -r "$Qt5"/include/QtGui   include/Qt5
        cp -r "$Qt5"/include/QtQml   include/Qt5
        cp -r "$Qt5"/include/QtQuick include/Qt5

        cp -r "$Qt5"/include/QtGui/5*/QtGui/qpa include/Qt5/QtGui
    fi

    mv include/Qt5/QtCore/5*/QtCore/private/*   include/Qt5/QtCore/private
    mv include/Qt5/QtGui/5*/QtGui/private/*     include/Qt5/QtGui/private
    mv include/Qt5/QtQml/5*/QtQml/private/*     include/Qt5/QtQml/private
    mv include/Qt5/QtQuick/5*/QtQuick/private/* include/Qt5/QtQuick/private
fi

#--------------------------------------------------------------------------------------------------
# zlib
#--------------------------------------------------------------------------------------------------

if [ $compiler = "mingw" ]; then

    cp "$MinGW"/libz.a lib
fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ]; then

    echo "COPYING VLC"

    cp -r "$VLC"/sdk/include/vlc include

    cp "$VLC"/sdk/lib/libvlc* lib

elif [ $1 = "macOS" ]; then

    echo "COPYING VLC"

    cp -r "$VLC"/include/vlc include

    cp "$VLC"/lib/libvlc.5.dylib     lib/libvlc.dylib
    cp "$VLC"/lib/libvlccore.9.dylib lib/libvlccore.dylib

elif [ $1 = "android" ]; then

    echo "COPYING VLC"

    cp -r "$VLC"/include/vlc include

    copyAndroid "$VLC" lib
fi

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ] || [ $1 = "macOS" ] || [ $1 = "android" ]; then

    echo "COPYING libtorrent"

    cp -r "$libtorrent"/libtorrent include

    if [ $1 = "android" ]; then

        copyAndroid "$libtorrent" lib
    else
        cp "$libtorrent"/*torrent*.* lib
    fi
fi

#--------------------------------------------------------------------------------------------------
# Boost
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ] || [ $1 = "macOS" ] || [ $1 = "android" ]; then

    echo "COPYING Boost"

    path="include/boost"

    mkdir -p $path

    cp -r "$Boost"/Boost/* $path

    if [ $1 = "android" ]; then

        copyAndroid "$Boost" lib
    else
        cp "$Boost"/*boost*.* lib
    fi
fi
