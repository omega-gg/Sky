#!/bin/sh
#set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../../../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.12.3"

QtInstaller_version="3.0.6"

MinGW_version="7.3.0"

VLC_version="3.0.6"

libtorrent_version="1.2.2"

Boost_version="1.71.0"

#--------------------------------------------------------------------------------------------------
# Android

NDK_version="21"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 1 -a $# != 2 ] \
   || \
   [ $1 != "win32" -a $1 != "win64" -a $1 != "macOS" -a $1 != "android32" -a $1 != "android64" ] \
   || \
   [ $# = 2 -a "$2" != "all" -a "$2" != "download" ]; then

    echo "Usage: generate <win32 | win64 | macOS | android32 | android64> [all | download]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# All
#--------------------------------------------------------------------------------------------------

if [ "$2" = "all" ]; then

    sh generate.sh win32
    sh generate.sh win64
    sh generate.sh macOS
    sh generate.sh android32
    sh generate.sh android64

    exit 0
fi

#--------------------------------------------------------------------------------------------------
# Download
#--------------------------------------------------------------------------------------------------

if [ "$2" = "download" ]; then

    sh download.sh $1

    echo ""
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $1 = "win32" -o $1 = "win64" ]; then

    os="windows"

elif [ $1 = "android32" -o $1 = "android64" ]; then

    os="android"
else
    os=""
fi

external="$external/$1"

MinGW="$external/MinGW/$MinGW_version"

Qt4="$external/Qt/$Qt4_version"
Qt5="$external/Qt/$Qt5_version"

QtInstaller="$external/Qt/installer/$QtInstaller_version"

SSL="$external/OpenSSL"

VLC="$external/VLC/$VLC_version"

libtorrent="$external/libtorrent/$libtorrent_version"

Boost="$external/boost/$Boost_version"

NDK="$external/NDK/$NDK_version"

tools="$external/tools"

#--------------------------------------------------------------------------------------------------
# Content
#--------------------------------------------------------------------------------------------------

path="3rdparty/$1"

rm -rf "$path"

mkdir -p "$path"

#--------------------------------------------------------------------------------------------------
# Qt4
#--------------------------------------------------------------------------------------------------

if [ $1 = "win32" ]; then

    echo "COPYING Qt4"

    qt="$path/Qt/$Qt4_version"

    mkdir -p "$qt"/bin
    mkdir -p "$qt"/plugins/imageformats

    cp "$Qt4"/bin/qmake.exe "$qt"/bin
    cp "$Qt4"/bin/moc.exe   "$qt"/bin
    cp "$Qt4"/bin/rcc.exe   "$qt"/bin

    cp "$Qt4"/bin/Qt*.dll "$qt"/bin

    cp "$Qt4"/bin/qt.conf "$qt"/bin

    cp "$Qt4"/plugins/imageformats/q*.dll "$qt"/plugins/imageformats

    #----------------------------------------------------------------------------------------------

    cp -r "$Qt4"/lib "$qt"

    cp -r "$Qt4"/include "$qt"
    cp -r "$Qt4"/src     "$qt"

    cp -r "$Qt4"/mkspecs "$qt"

    #----------------------------------------------------------------------------------------------

    rm "$qt"/bin/*d4.*

    rm "$qt"/plugins/imageformats/*d4.*

    rm "$qt"/lib/*d4.*
fi

#--------------------------------------------------------------------------------------------------
# Qt5
#--------------------------------------------------------------------------------------------------

echo "COPYING Qt5"

qt="$path/Qt/$Qt5_version"

mkdir -p "$qt"/bin
mkdir -p "$qt"/plugins/imageformats
mkdir -p "$qt"/plugins/platforms
mkdir -p "$qt"/qml

#cp "$Qt5"/bin/qmake.exe       "$qt"/bin
#cp "$Qt5"/bin/moc.exe         "$qt"/bin
#cp "$Qt5"/bin/rcc.exe         "$qt"/bin
#cp "$Qt5"/bin/qmlcachegen.exe "$qt"/bin

#cp "$Qt5"/bin/lib*.dll "$qt"/bin

#cp "$Qt5"/bin/Qt*.dll "$qt"/bin

cp "$Qt5"/bin/qt.conf "$qt"/bin

#cp "$Qt5"/plugins/imageformats/q*.dll "$qt"/plugins/imageformats
#cp "$Qt5"/plugins/platforms/q*.dll    "$qt"/plugins/platforms

#--------------------------------------------------------------------------------------------------

cp -r "$Qt5"/lib "$qt"

cp -r "$Qt5"/include "$qt"

cp -r "$Qt5"/qml/QtQuick.2 "$qt"/qml

cp -r "$Qt5"/mkspecs "$qt"

#--------------------------------------------------------------------------------------------------

#rm "$qt"/bin/*d.*

#rm "$qt"/plugins/imageformats/*d.*
#rm "$qt"/plugins/platforms/*d.*

#rm "$qt"/lib/*d.*

if [ $os = "windows" ]; then

    cp "$Qt5"/bin/qmake.exe       "$qt"/bin
    cp "$Qt5"/bin/moc.exe         "$qt"/bin
    cp "$Qt5"/bin/rcc.exe         "$qt"/bin
    cp "$Qt5"/bin/qmlcachegen.exe "$qt"/bin

    cp "$Qt5"/bin/lib*.dll "$qt"/bin

    cp "$Qt5"/bin/Qt*.dll "$qt"/bin

    cp "$Qt5"/plugins/imageformats/q*.dll "$qt"/plugins/imageformats
    cp "$Qt5"/plugins/platforms/q*.dll    "$qt"/plugins/platforms

    #----------------------------------------------------------------------------------------------

    rm "$qt"/bin/*d.*

    rm "$qt"/plugins/imageformats/*d.*
    rm "$qt"/plugins/platforms/*d.*

    rm "$qt"/lib/*d.*

elif [ $1 = "macOS" ]; then

    cp "$Qt5"/bin/qmake       "$qt"/bin
    cp "$Qt5"/bin/moc         "$qt"/bin
    cp "$Qt5"/bin/rcc         "$qt"/bin
    cp "$Qt5"/bin/qmlcachegen "$qt"/bin

    cp "$Qt5"/plugins/imageformats/libq*.dylib "$qt"/plugins/imageformats
    cp "$Qt5"/plugins/platforms/libq*.dylib    "$qt"/plugins/platforms

    #----------------------------------------------------------------------------------------------

    rm "$qt"/plugins/imageformats/*debug*
    rm "$qt"/plugins/platforms/*debug*

    find "$qt"/lib -name "*_debug*" -delete

elif [ $os = "android" ]; then

    cp "$Qt5"/bin/qmake       "$qt"/bin
    cp "$Qt5"/bin/moc         "$qt"/bin
    cp "$Qt5"/bin/rcc         "$qt"/bin
    cp "$Qt5"/bin/qmlcachegen "$qt"/bin

    cp "$Qt5"/plugins/imageformats/libq*.so "$qt"/plugins/imageformats

    cp -r "$Qt5"/plugins/platforms/android "$qt"/plugins/platforms
fi

#--------------------------------------------------------------------------------------------------
# QtInstaller
#--------------------------------------------------------------------------------------------------

if [ $1 = "win32" ]; then

    qt="$path/Qt/installer/$QtInstaller_version"

    mkdir -p "$qt"/bin

    cp "$QtInstaller"/bin/archivegen.exe    "$qt"/bin
    cp "$QtInstaller"/bin/binarycreator.exe "$qt"/bin
    cp "$QtInstaller"/bin/devtool.exe       "$qt"/bin
    cp "$QtInstaller"/bin/installerbase.exe "$qt"/bin
    cp "$QtInstaller"/bin/repogen.exe       "$qt"/bin
fi

#--------------------------------------------------------------------------------------------------
# MinGW
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ]; then

    echo "COPYING MinGW"

    mingw="$path/MinGW/$MinGW_version"

    mkdir -p "$mingw"

    cp -r "$MinGW"/* "$mingw"

    #----------------------------------------------------------------------------------------------
    # NOTE Windows: This is helpful to build Qt with OpenGL ES.
    # Update: It seems to be a better idea to install ANGLE on the build machine.

    #cp -r "$Qt5"/include/QtANGLE/GLES2 "$mingw"/include

    #cp "$Qt5"/bin/libEGL.dll    "$mingw"/bin
    #cp "$Qt5"/bin/libGLESv2.dll "$mingw"/bin

    #cp "$Qt5"/lib/liblibEGL.a    "$mingw"/lib
    #cp "$Qt5"/lib/liblibGLESv2.a "$mingw"/lib

    #----------------------------------------------------------------------------------------------
fi

#--------------------------------------------------------------------------------------------------
# SSL
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ]; then

    echo "COPYING SSL"

    ssl="$path/OpenSSL"

    mkdir -p "$ssl"

    cp "$SSL"/*.dll "$ssl"
fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

echo "COPYING VLC"

vlc="$path/VLC/$VLC_version"

mkdir -p "$vlc"

cp -r "$VLC"/plugins "$vlc"

if [ $os = "windows" ]; then

    cp -r "$VLC"/sdk "$vlc"

    cp "$VLC"/libvlc*.* "$vlc"

elif [ $1 = "macOS" ]; then

    cp -r "$VLC"/include "$vlc"
    cp -r "$VLC"/lib     "$vlc"
    cp -r "$VLC"/share   "$vlc"
fi

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

echo "COPYING libtorrent"

torrent="$path/libtorrent/$libtorrent_version"

mkdir -p "$torrent"

cp -r "$libtorrent"/* "$torrent"

#--------------------------------------------------------------------------------------------------
# Boost
#--------------------------------------------------------------------------------------------------

echo "COPYING Boost"

boost="$path/Boost/$Boost_version"

mkdir -p "$boost"

cp -r "$Boost"/* "$boost"

#--------------------------------------------------------------------------------------------------
# NDK
#--------------------------------------------------------------------------------------------------

if [ $os = "android" ]; then

    echo "COPYING NDK"

    ndk="$path/NDK/$NDK_version"

    mkdir -p "$ndk"

    cp -r "$NDK"/* "$ndk"
fi

#--------------------------------------------------------------------------------------------------
# tools
#--------------------------------------------------------------------------------------------------

if [ $1 = "win32" ]; then

    cp -r "$tools" "$path"
fi

#--------------------------------------------------------------------------------------------------
# Archive
#--------------------------------------------------------------------------------------------------

echo ""
echo "COMPRESSING ZIP"

archive="3rdparty-$1.zip"

rm -f $archive

7z a $archive "$path"

#--------------------------------------------------------------------------------------------------

echo ""
echo "CLEANING"

rm -rf 3rdparty
