#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

thirdparty="../3rdparty"

#--------------------------------------------------------------------------------------------------
# environment

compiler_win="mingw"

qt="qt6"

vlc="vlc3"

mobile="simulator"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

replace()
{
    expression='s/'"$1"'=\"'"$2"'"/'"$1"'=\"'"$3"'"/g'

    apply $expression environment.sh

    apply $expression 3rdparty.sh
    apply $expression configure.sh
    apply $expression build.sh
    apply $expression deploy.sh

    apply $expression dist/shaders/generate.sh
}

apply()
{
    if [ $host = "macOS" ]; then

        sed -i "" $1 $2
    else
        sed -i $1 $2
    fi
}

#--------------------------------------------------------------------------------------------------

getOs()
{
    case `uname` in
    Darwin*) echo "macOS";;
    *)       echo "other";;
    esac
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 1 -a $# != 2 ] \
   || \
   [ $1 != "mingw"     -a $1 != "msvc" -a \
     $1 != "qt4"       -a $1 != "qt5"  -a $1 != "qt6" -a \
     $1 != "vlc3"      -a $1 != "vlc4" -a \
     $1 != "simulator" -a $1 != "device" ] \
   || \
   [ $# = 2 -a "$2" != "all" ]; then

    echo "Usage: environment <mingw | msvc"
    echo "                    qt4 | qt5 | qt6 |"
    echo "                    vlc3 | vlc4 |"
    echo "                    simulator | device> [all]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

host=$(getOs)

#--------------------------------------------------------------------------------------------------
# 3rdparty
#--------------------------------------------------------------------------------------------------

if [ "$2" = "all" ]; then

    echo "ENVIRONMENT 3rdparty"
    echo "--------------------"

    cd "$thirdparty"

    sh environment.sh $1

    cd -

    echo "--------------------"
    echo ""
fi

#--------------------------------------------------------------------------------------------------
# Replacements
#--------------------------------------------------------------------------------------------------

if [ $1 = "msvc" -o $1 = "mingw" ]; then

    replace compiler_win $compiler_win $1

elif [ $1 = "qt4" -o $1 = "qt5" -o $1 = "qt6" ]; then

    replace qt $qt $1

elif [ $1 = "vlc3" -o $1 = "vlc4" ]; then

    replace vlc $vlc $1
else
    replace mobile $mobile $1
fi
