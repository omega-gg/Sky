#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

Sky="../.."

external="$PWD/../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.14.1"

MinGW_version="7.3.0"

#--------------------------------------------------------------------------------------------------

make_arguments="-j 4"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

getOs()
{
    case "$OSTYPE" in
    msys*)   os="win";;
    darwin*) os="macOS";;
    linux*)  os="linux";;
    *)       os="other";;
    esac

    type=`uname -m`

    if [ $type = "x86_64" ]; then

        if [ $os = "win" ]; then

            echo win64
        else
            echo $os
        fi

    elif [ $os = "win" ]; then

        echo win32
    else
        echo $os
    fi
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 2 -a $# != 3 ] \
   || \
   [ $1 != "qt4" -a $1 != "qt5" -a $1 != "clean" ] \
   || \
   [ $2 != "win32" -a $2 != "win64" -a $2 != "macOS" -a $2 != "linux" -a $2 != "android32" -a \
                                                                         $2 != "android64" ]  \
   || \
   [ $# = 3 -a "$3" != "deploy" -a "$3" != "tools" ]; then

    echo \
    "Usage: build <qt4 | qt5 | clean> <win32 | win64 | macOS | linux | android32 | android64> \
[deploy | tools]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

host=$(getOs)

external="$external/$2"

if [ $2 = "win32" -o $2 = "win64" ]; then

    os="windows"

    MinGW="$external/MinGW/$MinGW_version/bin"

if [ $2 = "android32" -o $2 = "android64" ]; then

    if [ $host != "linux" ]; then

        echo "You have to cross-compile $2 from Linux (preferably Ubuntu)."

        exit 1
    fi

    os="android"
else
    os="default"
fi

if [ $1 = "qt4" ]; then

    Qt="$external/Qt/$Qt4_version"
else
    Qt="$external/Qt/$Qt5_version"
fi

if [ $os = "windows" -o $2 = "macOS" ]; then

    qmake="$Qt/bin/qmake"
else
    qmake="qmake"
fi

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

if [ $1 = "clean" ]; then

    echo "CLEANING"

    rm -rf build/qt4/*
    rm -rf build/qt5/*

    touch build/qt4/.gitignore
    touch build/qt5/.gitignore

    exit 0
fi

#--------------------------------------------------------------------------------------------------
# Build Sky
#--------------------------------------------------------------------------------------------------

echo "BUILDING Sky"
echo "------------"

if [ $1 = "qt4" ]; then

    export QT_SELECT=qt4

    build="build/qt4"
else
    export QT_SELECT=qt5

    build="build/qt5"
fi

if [ $os = "windows" ]; then

    spec=win32-g++

    PATH="$Qt/bin:$MinGW:$PATH"

elif [ $2 = "macOS" ]; then

    spec=macx-clang

    export PATH=$Qt/bin:$PATH

elif [ $2 = "linux" ]; then

    if [ -d "/usr/lib/x86_64-linux-gnu" ]; then

        spec=linux-g++-64
    else
        spec=linux-g++-32
    fi

elif [ $os = "android" ]; then

    spec=android-clang

    if [ $2 = "android32" ]; then

        abi=armeabi-v7a
    else
        abi=arm64-v8a
    fi
fi

$qmake --version
echo ""

cd $build

if [ "$3" = "tools" ]; then

    if [ $os = "android" ]; then

        $qmake -r -spec $spec "CONFIG += release" "TOOLS = true" "ANDROID_ABIS = $abi" $Sky
    else
        $qmake -r -spec $spec "CONFIG += release" "TOOLS = true" $Sky
    fi
elif [ $os = "android" ]; then

    $qmake -r -spec $spec "CONFIG += release" "ANDROID_ABIS = $abi" $Sky
else
    $qmake -r -spec $spec "CONFIG += release" $Sky
fi

echo ""

if [ $os = "windows" ]; then

    mingw32-make $make_arguments
else
    make $make_arguments
fi

echo "------------"

#--------------------------------------------------------------------------------------------------
# Deploying Sky
#--------------------------------------------------------------------------------------------------

if [ "$3" = "deploy" ]; then

    echo ""
    echo "DEPLOYING Sky"
    echo "-------------"

    cd $Sky

    sh deploy.sh $1 $2

    echo "-------------"

elif [ "$3" = "tools" ]; then

    echo ""
    echo "DEPLOYING tools"
    echo "---------------"

    cd $Sky

    sh deploy.sh $1 $2 tools

    echo "---------------"
fi
