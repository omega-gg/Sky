#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.12.3"

QtInstaller_version="3.0.6"

MinGW_version="7.3.0"

VLC_version="3.0.6"

libtorrent_version="1.2.2"

Boost_version="1.71.0"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 1 ] || [ $1 != "win32" -a $1 != "win64" -a $1 != "macOS" ]; then

    echo "Usage: 3rdparty <win32 | win64 | macOS>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $1 = "win32" -o $1 = "win64" ]; then

    windows=true
else
    windows=false
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

cp "$Qt5"/bin/qmake.exe       "$qt"/bin
cp "$Qt5"/bin/moc.exe         "$qt"/bin
cp "$Qt5"/bin/rcc.exe         "$qt"/bin
cp "$Qt5"/bin/qmlcachegen.exe "$qt"/bin

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

if [ $windows = true ]; then

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

    cp "$Qt5"/bin/lib*.dylib "$qt"/bin

    cp "$Qt5"/bin/Qt*.dylib "$qt"/bin

    cp "$Qt5"/plugins/imageformats/q*.dylib "$qt"/plugins/imageformats
    cp "$Qt5"/plugins/platforms/q*.dylib    "$qt"/plugins/platforms

    #----------------------------------------------------------------------------------------------

    rm "$qt"/bin/*debug.*

    rm "$qt"/plugins/imageformats/*debug.*
    rm "$qt"/plugins/platforms/*debug.*

    find "$qt"/lib -type f -name *debug.* -delete
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

if [ $windows = true ]; then

    echo "COPYING MinGW"

    mingw="$path/MinGW/$MinGW_version"

    mkdir -p "$mingw"

    cp -r "$MinGW"/* "$mingw"
fi

#--------------------------------------------------------------------------------------------------
# SSL
#--------------------------------------------------------------------------------------------------

if [ $windows = true ]; then

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
cp -r "$VLC"/sdk     "$vlc"

cp "$VLC"/libvlc*.* "$vlc"

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

zip -rq $archive "$path"

#--------------------------------------------------------------------------------------------------

echo ""
echo "CLEANING"

rm -rf 3rdparty
