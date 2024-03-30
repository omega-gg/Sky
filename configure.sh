#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.15.2"
Qt6_version="6.6.0"

VLC_version="3.0.20"

libtorrent_version="2.0.9"

Boost_version="1.78.0"

#--------------------------------------------------------------------------------------------------
# Windows

MinGW_version="11.2.0"

#--------------------------------------------------------------------------------------------------
# environment

compiler_win="mingw"

qt="qt5"

mobile="simulator"

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

copyIncludes()
{
    include="include/$2"

    mkdir -p $include/QtCore/private
    mkdir -p $include/QtGui/private
    mkdir -p $include/QtQml/private
    mkdir -p $include/QtQuick/private

    cp -r "$1"/include/QtCore  $include
    cp -r "$1"/include/QtGui   $include
    cp -r "$1"/include/QtQml   $include
    cp -r "$1"/include/QtQuick $include

    cp -r "$1"/include/QtGui/"$qx"*/QtGui/qpa $include/QtGui

    if [ $qt = "qt6" ]; then

        cp -r "$1"/include/QtGui/"$qx"*/QtGui/rhi $include/QtGui
    fi

    mv $include/QtCore/"$qx"*/QtCore/private/*   $include/QtCore/private
    mv $include/QtGui/"$qx"*/QtGui/private/*     $include/QtGui/private
    mv $include/QtQml/"$qx"*/QtQml/private/*     $include/QtQml/private
    mv $include/QtQuick/"$qx"*/QtQuick/private/* $include/QtQuick/private

    rm -rf $include/QtCore/"$qx"*
    rm -rf $include/QtGui/"$qx"*
    rm -rf $include/QtQml/"$qx"*
    rm -rf $include/QtQuick/"$qx"*
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 1 -a $# != 2 ] \
   || \
   [ $1 != "win32" -a $1 != "win64" -a $1 != "macOS" -a $1 != "iOS" -a $1 != "linux" -a \
     $1 != "android" ] \
   || \
   [ $# = 2 -a "$2" != "clean" ]; then

    echo "Usage: configure <win32 | win64 | macOS | iOS | linux | android> [clean]"

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
else
    os="default"

    compiler="default"
fi

#--------------------------------------------------------------------------------------------------

if [ $qt = "qt4" ]; then

    Qt="$external/Qt/$Qt4_version"

    QtX="Qt4"

elif [ $qt = "qt5" ]; then

    Qt="$external/Qt/$Qt5_version"

    QtX="Qt5"

    qx="5"
else
    if [ $1 = "iOS" ]; then

        Qt="$external/Qt/$Qt6_version/ios"
    else
        Qt="$external/Qt/$Qt6_version"
    fi

    QtX="Qt6"

    qx="6"
fi

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

include="include/$QtX"

if [ $qt = "qt4" ]; then

    if [ $1 = "linux" ]; then

        echo "COPYING Qt"

        mkdir -p $include/QtCore/private
        mkdir -p $include/QtGui/private
        mkdir -p $include/QtDeclarative/private

        cp "$Qt"/src/corelib/kernel/*_p.h $include/QtCore/private
        cp "$Qt"/src/corelib/thread/*_p.h $include/QtCore/private

        cp "$Qt"/src/gui/kernel/*_p.h       $include/QtGui/private
        cp "$Qt"/src/gui/image/*_p.h        $include/QtGui/private
        cp "$Qt"/src/gui/widgets/*_p.h      $include/QtGui/private
        cp "$Qt"/src/gui/graphicsview/*_p.h $include/QtGui/private
        cp "$Qt"/src/gui/effects/*_p.h      $include/QtGui/private

        cp "$Qt"/src/declarative/qml/*_p.h           $include/QtDeclarative/private
        cp "$Qt"/src/declarative/graphicsitems/*_p.h $include/QtDeclarative/private
        cp "$Qt"/src/declarative/util/*_p.h          $include/QtDeclarative/private
    fi
else
    echo "COPYING Qt"

    if [ $1 = "android" ]; then

        if [ $qt = "qt5" ]; then

            copyIncludes "$Qt" $QtX
        else
            copyIncludes "$Qt"/android_armv7     $QtX/armeabi-v7a
            copyIncludes "$Qt"/android_arm64_v8a $QtX/arm64-v8a
            copyIncludes "$Qt"/android_x86       $QtX/x86
            copyIncludes "$Qt"/android_x86_64    $QtX/x86_64
        fi
    else
        mkdir -p $include/QtCore/private
        mkdir -p $include/QtGui/private
        mkdir -p $include/QtQml/private
        mkdir -p $include/QtQuick/private

        if [ $os = "windows" ]; then

            cp -r "$Qt"/include/QtCore  $include
            cp -r "$Qt"/include/QtGui   $include
            cp -r "$Qt"/include/QtQml   $include
            cp -r "$Qt"/include/QtQuick $include

            cp -r "$Qt"/include/QtGui/"$qx"*/QtGui/qpa $include/QtGui

            if [ $qt = "qt6" ]; then

                cp -r "$Qt"/include/QtGui/"$qx"*/QtGui/rhi $include/QtGui
            fi

        elif [ $1 = "macOS" ]; then

            cp -r "$Qt"/lib/QtCore.framework/Headers/*  $include/QtCore
            cp -r "$Qt"/lib/QtGui.framework/Headers/*   $include/QtGui
            cp -r "$Qt"/lib/QtQml.framework/Headers/*   $include/QtQml
            cp -r "$Qt"/lib/QtQuick.framework/Headers/* $include/QtQuick

            cp -r "$Qt"/lib/QtGui.framework/Headers/"$qx"*/QtGui/qpa $include/QtGui

            if [ $qt = "qt6" ]; then

                cp -r "$Qt"/lib/QtGui.framework/Headers/"$qx"*/QtGui/rhi $include/QtGui
            fi

        elif [ $1 = "iOS" ]; then

            cp -r "$Qt"/include/QtCore  $include
            cp -r "$Qt"/include/QtGui   $include
            cp -r "$Qt"/include/QtQml   $include
            cp -r "$Qt"/include/QtQuick $include

            cp -r "$Qt"/include/QtGui/"$qx"*/QtGui/qpa $include/QtGui

            if [ $qt = "qt6" ]; then

                cp -r "$Qt"/include/QtGui/"$qx"*/QtGui/rhi $include/QtGui
            fi

        elif [ $1 = "linux" ]; then

            cp -r "$Qt"/include/QtCore  $include
            cp -r "$Qt"/include/QtGui   $include
            cp -r "$Qt"/include/QtQml   $include
            cp -r "$Qt"/include/QtQuick $include
            cp -r "$Qt"/include/QtDBus  $include

            cp -r "$Qt"/include/QtGui/"$qx"*/QtGui/qpa $include/QtGui

            if [ $qt = "qt6" ]; then

                cp -r "$Qt"/include/QtGui/"$qx"*/QtGui/rhi $include/QtGui
            fi
        fi

        mv $include/QtCore/"$qx"*/QtCore/private/*   $include/QtCore/private
        mv $include/QtGui/"$qx"*/QtGui/private/*     $include/QtGui/private
        mv $include/QtQml/"$qx"*/QtQml/private/*     $include/QtQml/private
        mv $include/QtQuick/"$qx"*/QtQuick/private/* $include/QtQuick/private

        rm -rf $include/QtCore/"$qx"*
        rm -rf $include/QtGui/"$qx"*
        rm -rf $include/QtQml/"$qx"*
        rm -rf $include/QtQuick/"$qx"*
    fi
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

elif [ $1 = "linux" ]; then

    echo "COPYING VLC"

    # NOTE: Sometimes we're using libvlc-dev from the system.
    if [ -d "$VLC"/include ]; then

        cp -r "$VLC"/include/vlc include
    fi

    cp "$VLC"/libvlc.so.5     lib/libvlc.so
    cp "$VLC"/libvlccore.so.9 lib/libvlccore.so

    cp "$VLC"/lib*.so* lib

    #----------------------------------------------------------------------------------------------
    # NOTE: Patching VLC libraries rpath for standalone packages.

    if [ -x "$(command -v patchelf)" ]; then

        find lib -maxdepth 1 -name libvlc*.so* -exec patchelf --set-rpath '$ORIGIN' {} \;
    else
        echo "patchelf is not installed"
    fi

elif [ $1 = "iOS" ]; then

    echo "COPYING VLC"

    cp -r "$VLC"/include/vlc include

    if [ $mobile = "simulator" ]; then

        cp -r "$VLC"/ios-arm64_i386_x86_64-simulator/MobileVLCKit.framework lib
    else
        cp -r "$VLC"/ios-arm64_armv7_armv7s/MobileVLCKit.framework lib
    fi

elif [ $1 = "android" ]; then

    echo "COPYING VLC"

    cp -r "$VLC"/include/vlc include

    copyAndroid "$VLC" lib
fi

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

if [ $1 != "iOS" ]; then

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

if [ $1 != "iOS" ]; then

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
