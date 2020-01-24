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

if [ $# != 1 -a $# != 2 ] \
   || \
   [ $1 != "win32" -a $1 != "win64" -a $1 != "macOS" ] || [ $# = 2 -a "$2" != "download" ]; then

    echo "Usage: 3rdparty <win32 | win64 | macOS> [download]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

function artifact
{
    echo $artifacts | $grep -Po '"id":.*?[^\\]}}'         | \
                      $grep $1                            | \
                      $grep -Po '"downloadUrl":.*?[^\\]"' | \
                      $grep -o '"[^"]*"$'                 | tr -d '"'
}

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $1 = "win32" -o $1 = "win64" ]; then

    windows=true

    grep=grep

elif [ $1 = "macOS" ]; then

    windows=false

    # NOTE: We use ggrep on macOS because it supports Perl regexp (brew install grep).
    grep=ggrep
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

if [ $windows = true ]; then

    if [ $1 = "win32" ]; then

        SSL_url="https://indy.fulgan.com/SSL/Archive/openssl-1.0.2p-i386-win32.zip"
    else
        SSL_url="https://indy.fulgan.com/SSL/Archive/openssl-1.0.2p-x64_86-win64.zip"
    fi

    VLC_url="http://download.videolan.org/pub/videolan/vlc/$VLC_version/$1/vlc-$VLC_version-$1.7z"

elif [ $1 = "macOS" ]; then

    VLC_url="http://download.videolan.org/pub/videolan/vlc/$VLC_version/macosx/vlc-$VLC_version-$1.dmg"
fi

libtorrent_url="https://dev.azure.com/bunjee/libtorrent/_apis/build/builds/465/artifacts"

#--------------------------------------------------------------------------------------------------
# Download
#--------------------------------------------------------------------------------------------------

if [ "$2" = "download" ]; then

    #----------------------------------------------------------------------------------------------
    # Qt5
    #----------------------------------------------------------------------------------------------

    echo "DOWNLOADING Qt5"

    test -d "$Qt5" && rm -rf "$Qt5"/*

    if [ $windows = true ]; then

        sh install-qt.sh --directory "$Qt5" --version $Qt5_version --host windows_x86 \
                         --toolchain $1_mingw73 \
                         qtbase qtdeclarative qtxmlpatterns qtsvg qtwinextras

        if [ $1 = "win32" ]; then

            mv "$Qt5"/$Qt5_version/mingw73_32/* "$Qt5"
        else
            mv "$Qt5"/$Qt5_version/mingw73_64/* "$Qt5"
        fi

    elif [ $1 = "macOS" ]; then

        sh install-qt.sh --directory "$Qt5" --version $Qt5_version --host mac_x64 \
                         --toolchain clang_64 \
                         qtbase qtdeclarative qtxmlpatterns qtsvg

        mv "$Qt5"/$Qt5_version/clang_64/* "$Qt5"
    fi

    rm -rf "$Qt5"/$Qt5_version

    #----------------------------------------------------------------------------------------------
    # SSL
    #----------------------------------------------------------------------------------------------

    if [ $windows = true ]; then

        echo ""
        echo "DOWNLOADING SSL"
        echo $SSL_url

        curl -L -o ssl.zip $SSL_url

        test -d "$SSL" && rm -rf "$SSL"/*

        7z x ssl.zip -o"$SSL"

        rm ssl.zip
    fi

    exit 0

    #----------------------------------------------------------------------------------------------
    # VLC
    #----------------------------------------------------------------------------------------------

    echo ""
    echo "DOWNLOADING VLC"
    echo $VLC_url

    if [ $windows = true ]; then

        curl -L -o VLC.7z $VLC_url

        test -d "$VLC" && rm -rf "$VLC"/*

        7z x VLC.7z -o"$VLC"

        rm VLC.7z

        path="$VLC/vlc-$VLC_version"

        mv "$path"/* "$VLC"

        rm -rf "$path"

    elif [ $1 = "macOS" ]; then

        curl -L -o VLC.dmg $VLC_url

        test -d "$VLC" && rm -rf "$VLC"/*
    fi

    #----------------------------------------------------------------------------------------------
    # libtorrent
    #----------------------------------------------------------------------------------------------

    echo ""
    echo "DOWNLOADING libtorrent"

    curl -L -o artifacts.json $libtorrent_url

    artifacts=$(cat artifacts.json)

    rm artifacts.json

    libtorrent_url=$(artifact libtorrent-$1)

    echo $libtorrent_url

    #curl -L -o libtorrent.zip $(artifact libtorrent-$1)

    #7z x libtorrent.zip -y -o"$$external"

    #rm libtorrent.zip
fi

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

    cp "$Qt5"/plugins/imageformats/libq*.dylib "$qt"/plugins/imageformats
    cp "$Qt5"/plugins/platforms/libq*.dylib    "$qt"/plugins/platforms

    #----------------------------------------------------------------------------------------------

    rm "$qt"/plugins/imageformats/*debug*
    rm "$qt"/plugins/platforms/*debug*

    find "$qt"/lib -name "*_debug*" -delete
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

    #----------------------------------------------------------------------------------------------
    # NOTE Windows: This is helpful to build Qt with OpenGL ES.

    cp -r "$Qt5"/include/QtANGLE/GLES2 "$mingw"/include

    cp "$Qt5"/bin/libEGL.dll    "$mingw"/bin
    cp "$Qt5"/bin/libGLESv2.dll "$mingw"/bin

    cp "$Qt5"/lib/liblibEGL.a    "$mingw"/lib
    cp "$Qt5"/lib/liblibGLESv2.a "$mingw"/lib

    #----------------------------------------------------------------------------------------------
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

if [ $windows = true ]; then

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
