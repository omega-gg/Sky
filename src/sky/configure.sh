#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

target="sky"

Sky="$PWD/../.."

external="$PWD/../../../3rdparty"

#--------------------------------------------------------------------------------------------------
# Android

JDK_version="11.0.2"

TOOLS_version="33.0.0"

SDK_version="34"

#--------------------------------------------------------------------------------------------------
# environment

compiler_win="mingw"

qt="qt6"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

copyAndroid()
{
    cp -r "$1"/armeabi-v7a $data/armeabi-v7a/libs
    cp -r "$1"/arm64-v8a   $data/arm64-v8a/libs
    cp -r "$1"/x86         $data/x86/libs
    cp -r "$1"/x86_64      $data/x86_64/libs
}

cleanAndroid()
{
    path="$data/$1/libs/$1"

    rm -f $path/*.so
    rm -f $path/*.a
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
else
    os="default"

    compiler="default"
fi

deploy="$Sky/deploy"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

echo "CLEANING"

# NOTE: We want to keep the 'storage' folder.
if [ -d "bin/storage" ]; then

    mv bin/storage .

    rm -rf bin/*
    touch  bin/.gitignore

    mv storage bin
else
    rm -rf bin/*
    touch  bin/.gitignore
fi

# NOTE: We have to remove the folder to delete .qmake.stash.
rm -rf build
mkdir  build
touch  build/.gitignore

if [ $1 = "android" ]; then

    data="dist/android/data"

    cleanAndroid armeabi-v7a
    cleanAndroid arm64-v8a
    cleanAndroid x86
    cleanAndroid x86_64
fi

if [ "$2" = "clean" ]; then

    exit 0
fi

#--------------------------------------------------------------------------------------------------
# MinGW
#--------------------------------------------------------------------------------------------------

echo "CONFIGURING $target"
echo "------------------------"

if [ $compiler = "mingw" ]; then

    echo "COPYING MinGW"

    cp "$deploy"/libgcc_s_*-1.dll    bin
    cp "$deploy"/libstdc++-6.dll     bin
    cp "$deploy"/libwinpthread-1.dll bin
fi

#--------------------------------------------------------------------------------------------------
# SSL
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ]; then

    echo "COPYING SSL"

    if [ $qt = "qt4" ]; then

        cp "$deploy"/libeay32.dll bin
        cp "$deploy"/ssleay32.dll bin
    else
        cp "$deploy"/libssl*.dll    bin
        cp "$deploy"/libcrypto*.dll bin
    fi

elif [ $1 = "android" ]; then

    echo "COPYING SSL"

    copyAndroid "$deploy"/ssl
fi

#--------------------------------------------------------------------------------------------------
# SDK
#--------------------------------------------------------------------------------------------------

if [ $1 = "android" ]; then

    echo "CONFIGURING SDK"

    cd "$external/SDK/$SDK_version/tools/bin"

    export JAVA_HOME="$external/JDK/$JDK_version"

    path="$PWD/../.."

    yes | ./sdkmanager --sdk_root="$path" --licenses

    ./sdkmanager --sdk_root="$path" "build-tools;$TOOLS_version" \
                                    "platforms;android-$SDK_version" \

    ./sdkmanager --sdk_root="$path" --update

    cd -
fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ]; then

    echo "COPYING VLC"

    rm -rf bin/plugins
    mkdir  bin/plugins

    cp -r "$deploy"/plugins bin

    cp "$deploy"/libvlc*.dll bin

elif [ $1 = "macOS" ]; then

    echo "COPYING VLC"

    rm -rf bin/plugins
    mkdir  bin/plugins

    cp -r "$deploy"/plugins bin

    cp "$deploy"/libvlc*.dylib bin

elif [ $1 = "linux" ]; then

    echo "COPYING VLC"

    rm -rf bin/vlc
    mkdir  bin/vlc

    cp -r "$deploy"/vlc bin

    cp "$deploy"/libvlc*.so* bin

    if [ -f "$deploy"/libidn.so* ]; then

        cp "$deploy"/libidn.so* bin
    fi

elif [ $1 = "android" ]; then

    echo "COPYING VLC"

    copyAndroid "$deploy"/vlc
fi

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

if [ $os = "windows" ]; then

    echo "COPYING libtorrent"

    cp "$deploy"/*torrent-rasterbar.dll bin

elif [ $1 = "macOS" ]; then

    echo "COPYING libtorrent"

    cp "$deploy"/libtorrent-rasterbar.dylib bin

elif [ $1 = "linux" ]; then

    echo "COPYING libtorrent"

    cp "$deploy"/libtorrent-rasterbar*.so* bin

elif [ $1 = "android" ]; then

    echo "COPYING libtorrent"

    copyAndroid "$deploy"/libtorrent
fi

#--------------------------------------------------------------------------------------------------
# Boost
#--------------------------------------------------------------------------------------------------

if [ $1 = "macOS" ]; then

    echo "COPYING Boost"

    cp "$deploy"/libboost*.dylib bin

elif [ $1 = "linux" ]; then

    echo "COPYING Boost"

    cp "$deploy"/libboost*.so* bin
fi

echo "------------------------"
