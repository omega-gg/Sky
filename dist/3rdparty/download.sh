#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../../../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt5_version="5.12.3"

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
   [ $# = 2 -a "$2" != "all" ]; then

    echo "Usage: download <win32 | win64 | macOS | android32 | android64> [all]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# All
#--------------------------------------------------------------------------------------------------

if [ "$2" = "all" ]; then

    sh download.sh win32
    sh download.sh win64
    sh download.sh macOS
    sh download.sh android32
    sh download.sh android64

    exit 0
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

    os="windows"

elif [ $1 = "android32" -o $1 = "android64" ]; then

    os="android"
else
    os=""
fi

if [ "$OSTYPE" = "darwin"* ]; then

    # NOTE: We use ggrep on macOS because it supports Perl regexp (brew install grep).
    grep="ggrep"
else
    grep="grep"
fi

external="$external/$1"

Qt5="$external/Qt/$Qt5_version"

SSL="$external/OpenSSL"

VLC="$external/VLC/$VLC_version"

libtorrent="$external/libtorrent/$libtorrent_version"

Boost="$external/Boost/$Boost_version"

NDK="$external/NDK/$NDK_version"

#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ]; then

    if [ $1 = "win32" ]; then

        SSL_url="https://indy.fulgan.com/SSL/Archive/openssl-1.0.2p-i386-win32.zip"
    else
        SSL_url="https://indy.fulgan.com/SSL/Archive/openssl-1.0.2p-x64_86-win64.zip"
    fi

    VLC_url="http://download.videolan.org/pub/videolan/vlc/$VLC_version/$1/vlc-$VLC_version-$1.7z"

elif [ $1 = "macOS" ]; then

    VLC_url="http://download.videolan.org/pub/videolan/vlc/$VLC_version/macosx/vlc-$VLC_version.dmg"
fi

libtorrent_url="https://dev.azure.com/bunjee/libtorrent/_apis/build/builds/465/artifacts"

if [ $os = "android" ]; then

    NDK_url="https://dl.google.com/android/repository/android-ndk-r$NDK_version-linux-x86_64.zip"
fi

#--------------------------------------------------------------------------------------------------
# Qt5
#--------------------------------------------------------------------------------------------------

echo "DOWNLOADING Qt5"

test -d "$Qt5" && rm -rf "$Qt5"/*

if [ $os = "windows" ]; then

    sh install-qt.sh --directory "$Qt5" --version $Qt5_version \
                     --toolchain $1_mingw73 qtbase qtdeclarative qtxmlpatterns qtsvg qtwinextras

    if [ $1 = "win32" ]; then

        mv "$Qt5"/$Qt5_version/mingw73_32/* "$Qt5"
    else
        mv "$Qt5"/$Qt5_version/mingw73_64/* "$Qt5"
    fi

elif [ $1 = "macOS" ]; then

    sh install-qt.sh --directory "$Qt5" --version $Qt5_version \
                     --toolchain clang_64 qtbase qtdeclarative qtxmlpatterns qtsvg

    mv "$Qt5"/$Qt5_version/clang_64/* "$Qt5"

elif [ $1 = "android32" ]; then

    sh install-qt.sh --directory "$Qt5" --version $Qt5_version --host linux_x64 --target android \
                     --toolchain android_armv7 qtbase qtdeclarative qtxmlpatterns qtsvg

    mv "$Qt5"/$Qt5_version/android_armv7/* "$Qt5"

elif [ $1 = "android64" ]; then

    sh install-qt.sh --directory "$Qt5" --version $Qt5_version --host linux_x64 --target android \
                     --toolchain android_arm64_v8a qtbase qtdeclarative qtxmlpatterns qtsvg

    mv "$Qt5"/$Qt5_version/android_arm64_v8a/* "$Qt5"
fi

rm -rf "$Qt5"/$Qt5_version

#--------------------------------------------------------------------------------------------------
# SSL
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ]; then

    echo ""
    echo "DOWNLOADING SSL"
    echo $SSL_url

    curl -L -o ssl.zip $SSL_url

    test -d "$SSL" && rm -rf "$SSL"/*

    7z x ssl.zip -o"$SSL"

    rm ssl.zip
fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

echo ""
echo "DOWNLOADING VLC"
echo $VLC_url

if [ $os = "windows" ]; then

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

    #----------------------------------------------------------------------------------------------
    # NOTE macOS: We get a header error when extracting the archive with 7z.

    set +e

    7z x VLC.dmg -o"$VLC"

    set -e

    #----------------------------------------------------------------------------------------------

    rm VLC.dmg

    path="$VLC/VLC media player"

    mv "$path"/VLC.app/Contents/MacOS/* "$VLC"

    rm -rf "$path"
fi

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

echo ""
echo "DOWNLOADING libtorrent"

curl -L -o artifacts.json $libtorrent_url

test -d "$libtorrent" && rm -rf "$libtorrent"/*
test -d "$Boost"      && rm -rf "$Boost"/*

artifacts=$(cat artifacts.json)

rm artifacts.json

libtorrent_url=$(artifact libtorrent-$1)

echo $libtorrent_url

#curl -L -o libtorrent.zip $(artifact libtorrent-$1)

#7z x libtorrent.zip -y -o"$external"

#rm libtorrent.zip

#--------------------------------------------------------------------------------------------------
# NDK
#--------------------------------------------------------------------------------------------------

if [ $os = "android" ]; then

    echo ""
    echo "DOWNLOADING NDK"

    curl -L -o NDK.zip $NDK_url

    test -d "$NDK" && rm -rf "$NDK"/*

    7z x NDK.zip -o"$NDK"

    rm NDK.zip

    path="$NDK/android-ndk-r$NDK_version"

    mv "$path"/* "$NDK"

    rm -rf "$path"
fi
