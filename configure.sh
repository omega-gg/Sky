#!/bin/sh

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.5.1"

VLC_version="2.2.6"

libtorrent_version="1.1.0"

Boost_version="1.55.0"

#--------------------------------------------------------------------------------------------------

Qt4="$external/Qt/$Qt4_version"
Qt5="$external/Qt/$Qt5_version"

VLC="$external/VLC/$VLC_version"

libtorrent="$external/libtorrent/$libtorrent_version"

Boost="$external/Boost/$Boost_version"

#--------------------------------------------------------------------------------------------------

bin4="bin"
bin5="latest"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 2 ] || [ $1 != "qt4" -a $1 != "qt5" -a $1 != "clean" ] || [ $2 != "win32" -a \
                                                                       $2 != "linux" -a \
                                                                       $2 != "osx" ]; then

    echo "Usage: configure <qt4 | qt5 | clean> <win32 | linux | osx>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

if [ $1 = "clean" ]; then

    echo "CLEANING"

    rm -rf lib
    mkdir  lib
    touch  lib/.gitignore

    rm -rf "$bin4"
    mkdir  "$bin4"
    touch  "$bin4"/.gitignore

    rm -rf "$bin5"
    mkdir  "$bin5"
    touch  "$bin5"/.gitignore

    #----------------------------------------------------------------------------------------------
    # Qt

    rm -rf include/Qt

    #----------------------------------------------------------------------------------------------
    # VLC

    rm -rf include/vlc

    #----------------------------------------------------------------------------------------------
    # libtorrent

    rm -rf include/libtorrent

    #----------------------------------------------------------------------------------------------
    # Boost

    rm -rf include/Boost

    exit 0
fi

#--------------------------------------------------------------------------------------------------
# Qt
#--------------------------------------------------------------------------------------------------

echo "COPYING Qt"

if [ $1 = "qt4" -a $2 = "linux" ]; then

    mkdir -p include/Qt/QtCore/private

    cp "$Qt4"/src/corelib/kernel/*_p.h include/Qt/QtCore/private

    mkdir -p include/Qt/QtGui/private

    cp "$Qt4"/src/gui/kernel/*_p.h include/Qt/QtGui/private

    mkdir -p include/Qt/QtDeclarative/private

    cp "$Qt4"/src/declarative/qml/*_p.h           include/Qt/QtDeclarative/private
    cp "$Qt4"/src/declarative/graphicsitems/*_p.h include/Qt/QtDeclarative/private
    cp "$Qt4"/src/declarative/util/*_p.h          include/Qt/QtDeclarative/private

elif [ $1 = "qt5" -a $2 = "linux" -a $2 = "win32" ]; then

    mkdir -p include/Qt/QtCore/private

    cp "$Qt5"/include/QtCore/* include/Qt/QtCore

    cp "$Qt5"/include/QtCore/$Qt5_version/QtCore/private/* include/Qt/QtCore/private

    mkdir -p include/Qt/QtDeclarative/private

    cp "$Qt5"/include/QtDeclarative/* include/Qt/QtDeclarative

    cp "$Qt5"/include/QtDeclarative/$Qt5_version/QtDeclarative/private/* \
        include/Qt/QtDeclarative/private

elif [ $1 = "qt5" -a $2 = "osx" ]; then

    Qt5=/usr/local/opt/qt\@5.5

    mkdir -p include/Qt/QtCore/private

    ditto "$Qt5"/include/QtCore include/Qt/QtCore

    ditto "$Qt5"/include/QtCore/$Qt5_version/QtCore/private include/Qt/QtCore/private

    mkdir -p include/Qt/QtDeclarative/private

    ditto "$Qt5"/include/QtDeclarative include/Qt/QtDeclarative

    ditto "$Qt5"/include/QtDeclarative/$Qt5_version/QtDeclarative/private \
        include/Qt/QtDeclarative/private

elif [ $1 = "qt4" -a $2 = "osx" ]; then

    echo "Qt4 is not currently supported on OSX"

    exit 1

fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

if [ $2 = "win32" ]; then

    echo "COPYING VLC"

    cp -r "$VLC"/sdk/include/vlc include

    cp "$VLC"/sdk/lib/libvlc* lib
fi

if [ $2 = "osx" ]; then

    echo "COPYING VLC"

    ditto /Applications/VLC.app/Contents/MacOS/include include

    ditto /Applications/VLC.app/Contents/MacOS/lib lib
fi

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

if [ $2 = "win32" ]; then

    echo "COPYING libtorrent"

    cp -r "$libtorrent"/libtorrent include

    cp "$libtorrent"/libtorrent.* lib
fi


if [ $2 = "osx" ]; then

    echo "COPYING libtorrent"

    ditto /usr/local/include/libtorrent include/libtorrent

    ditto /usr/local/lib/libtorrent-* lib
fi

#--------------------------------------------------------------------------------------------------
# Boost
#--------------------------------------------------------------------------------------------------

if [ $2 = "win32" ]; then

    echo "COPYING Boost"

    cp -r "$Boost"/Boost include

    cp "$Boost"/libboost*.* lib
fi

if [ $2 = "osx" ]; then

    echo "COPYING Boost"

    ditto /usr/local/opt/boost\@1.55/include include

    ditto /usr/local/opt/boost\@1.55/lib lib
fi
