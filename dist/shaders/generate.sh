#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="$PWD/../../../3rdparty"

Qt6_version="6.2.1"

#--------------------------------------------------------------------------------------------------
# environment

qt="qt5"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

generate()
{
    $qsb --glsl "100 es,120,150" --hlsl 50 --msl 12 --batchable -o qsb/$1.qsb $1
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 1 ] \
   || \
   [ $1 != "win32" -a $1 != "win64" -a $1 != "macOS" -a $1 != "linux" -a $1 != "android" ]; then

    echo "Usage: generate <win32 | win64 | macOS | linux | android>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

external="$external/$1"

if [ $qt = "qt4" -o $qt = "qt5" ]; then

    echo "The qsb tools needs Qt 6 or higher"

    exit 1
fi

if [ $qt = "qt6" ]; then

    Qt="$external/Qt/$Qt6_version"
fi

qsb="$Qt/bin/qsb"

#--------------------------------------------------------------------------------------------------
# Generate
#--------------------------------------------------------------------------------------------------

echo "GENERATING shaders"

generate video.vert
generate video.frag