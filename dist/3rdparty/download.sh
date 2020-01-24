#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../../../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt5_version="5.12.3"

VLC_version="3.0.6"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 1 ] || [ $1 != "win32" -a $1 != "win64" -a $1 != "macOS" ]; then

    echo "Usage: download <win32 | win64 | macOS>"

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

Qt5="$external/Qt/$Qt5_version"

SSL="$external/OpenSSL"

VLC="$external/VLC/$VLC_version"

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
# Qt5
#--------------------------------------------------------------------------------------------------

echo "DOWNLOADING Qt5"

test -d "$Qt5" && rm -rf "$Qt5"/*

if [ $windows = true ]; then

    sh install-qt.sh --directory "$Qt5" --version $Qt5_version --host windows_x86 \
                     --toolchain $1_mingw73 qtbase qtdeclarative qtxmlpatterns qtsvg qtwinextras

    if [ $1 = "win32" ]; then

        mv "$Qt5"/$Qt5_version/mingw73_32/* "$Qt5"
    else
        mv "$Qt5"/$Qt5_version/mingw73_64/* "$Qt5"
    fi

elif [ $1 = "macOS" ]; then

    sh install-qt.sh --directory "$Qt5" --version $Qt5_version --host mac_x64 \
                     --toolchain clang_64 qtbase qtdeclarative qtxmlpatterns qtsvg

    mv "$Qt5"/$Qt5_version/clang_64/* "$Qt5"
fi

rm -rf "$Qt5"/$Qt5_version

#--------------------------------------------------------------------------------------------------
# SSL
#--------------------------------------------------------------------------------------------------

if [ $windows = true ]; then

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

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

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
