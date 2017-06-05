#!/bin/sh

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"

VLC_version="2.2.6"

libtorrent_version="1.1.0"

Boost_version="1.55.0"

#--------------------------------------------------------------------------------------------------

Qt4="$external/Qt/$Qt4_version"

VLC="$external/VLC/$VLC_version"

libtorrent="$external/libtorrent/$libtorrent_version"

Boost="$external/Boost/$Boost_version"

#--------------------------------------------------------------------------------------------------

Qt4_name="qt-everywhere-opensource-src-4.8.7"

Qt4_archive="$Qt4_name.tar.gz"

Qt4_sources="http://download.qt.io/official_releases/qt/4.8/4.8.7/$Qt4_archive"

#--------------------------------------------------------------------------------------------------
# Windows

archive_win32="http://omega.gg/get/Sky/3rdparty/win32"

#--------------------------------------------------------------------------------------------------
# Linux

usr="/usr/lib"

lib32="/usr/lib/i386-linux-gnu"
lib64="/usr/lib/x86_64-linux-gnu"

#--------------------------------------------------------------------------------------------------
# Ubuntu

QtWebkit_version_ubuntu="4.10.2"

libvlc_version_ubuntu="5.5.0"

libvlccore_version_ubuntu="8.0.0"

libtorrent_version_ubuntu="9.0.0"

Boost_version_ubuntu="1.61.0"

#--------------------------------------------------------------------------------------------------

X11_ubuntu="libx11-dev libxi-dev libxinerama-dev libxrandr-dev libxcursor-dev libfontconfig-dev"

Qt_ubuntu="qt4-default libqtwebkit-dev openssl"

VLC_ubuntu="libvlc-dev vlc"

libtorrent_ubuntu="libtorrent-rasterbar-dev"

Boost_ubuntu="libboost-all-dev"

tools_ubuntu="git"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 2 ] || [ $1 != "all"       -a \
                    $1 != "install"   -a \
                    $1 != "uninstall" -a \
                    $1 != "deploy"    -a \
                    $1 != "clean" ] || [ $2 != "win32" -a $2 != "ubuntu" ]; then

    echo "Usage: 3rdparty <all | install | uninstall | deploy | clean> <win32 | ubuntu>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $2 = "ubuntu" ]; then

    linux=true

    #----------------------------------------------------------------------------------------------

    QtWebkit_version_linux="$QtWebkit_version_ubuntu"

    libvlc_version_linux="$libvlc_version_ubuntu"

    libvlccore_version_linux="$libvlccore_version_ubuntu"

    libtorrent_version_linux="$libtorrent_version_ubuntu"

    Boost_version_linux="$Boost_version_ubuntu"

    #----------------------------------------------------------------------------------------------

    X11_linux="$X11_ubuntu"

    Qt_linux="$Qt_ubuntu"

    VLC_linux="$VLC_ubuntu"

    libtorrent_linux="$libtorrent_ubuntu"

    Boost_linux="$Boost_ubuntu"

    tools_linux="$tools_ubuntu"
else
    linux=false
fi

if [ $linux = true ]; then

    if [ $(uname -m) = "x86_64" ]; then

        lib="$lib64"
    else
        lib="$lib32"
    fi

    Boost="$external/Boost/$Boost_version_linux"
fi

#--------------------------------------------------------------------------------------------------
# Install
#--------------------------------------------------------------------------------------------------

if [ $1 = "all" ] || [ $1 = "install" ]; then

    if [ $linux = true ]; then

        echo "INSTALLING X11"

        sudo apt-get install -y $X11_linux

        echo ""
        echo "INSTALLING Qt"

        sudo apt-get install -y $Qt_linux

        echo ""
        echo "INSTALLING VLC"

        sudo apt-get install -y $VLC_linux

        echo ""
        echo "INSTALLING libtorrent"

        sudo apt-get install -y $libtorrent_linux

        echo ""
        echo "INSTALLING Boost"

        sudo apt-get install -y $Boost_linux

        echo ""
        echo "INSTALLING TOOLS"

        sudo apt-get install -y $tools_linux
    fi
fi

if [ $1 = "uninstall" ]; then

    if [ $linux = true ]; then

        echo "UNINSTALLING X11"

        sudo apt-get remove -y $X11_linux

        echo ""
        echo "UNINSTALLING Qt"

        sudo apt-get remove -y $Qt_linux

        echo ""
        echo "UNINSTALLING VLC"

        sudo apt-get remove -y $VLC_linux

        echo ""
        echo "UNINSTALLING libtorrent"

        sudo apt-get remove -y $libtorrent_linux

        echo ""
        echo "UNINSTALLING Boost"

        sudo apt-get remove -y $Boost_linux

        echo ""
        echo "UNINSTALLING TOOLS"

        sudo apt-get remove -y $tools_linux
    fi
fi

if [ $1 = "all" -a $linux = true ]; then

    echo ""
fi

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

if [ $1 = "clean" -a $linux = true ]; then

    echo "CLEANING"

    sudo rm -rf "$Qt4"
    sudo rm -rf "$VLC"
    sudo rm -rf "$libtorrent"
    sudo rm -rf "$Boost_linux"

    exit 0
fi

#--------------------------------------------------------------------------------------------------
# Deploy
#--------------------------------------------------------------------------------------------------

if [ $1 = "all" ] || [ $1 = "deploy" ]; then

    if [ $2 = "win32" ]; then

        echo "3rdparty archive -> $archive_win32"

    elif [ $linux = true ]; then

        echo "DEPLOYING Qt"

        if [ ! -d "$Qt4" ]; then

            mkdir -p "$Qt4"

            cd "$Qt4"

            wget "$Qt4_sources"

            tar -xzf "$Qt4_archive"

            mv "$Qt4_name"/* .

            rm -rf "$Qt4_name"
            rm "$Qt4_archive"

            cd -
        fi

        sudo cp "$lib"/libQtCore.so.$Qt4_version        "$Qt4"/lib/libQtCore.so.4
        sudo cp "$lib"/libQtGui.so.$Qt4_version         "$Qt4"/lib/libQtGui.so.4
        sudo cp "$lib"/libQtDeclarative.so.$Qt4_version "$Qt4"/lib/libQtDeclarative.so.4
        sudo cp "$lib"/libQtNetwork.so.$Qt4_version     "$Qt4"/lib/libQtNetwork.so.4
        sudo cp "$lib"/libQtOpenGL.so.$Qt4_version      "$Qt4"/lib/libQtOpenGL.so.4
        sudo cp "$lib"/libQtScript.so.$Qt4_version      "$Qt4"/lib/libQtScript.so.4
        sudo cp "$lib"/libQtSql.so.$Qt4_version         "$Qt4"/lib/libQtSql.so.4
        sudo cp "$lib"/libQtSvg.so.$Qt4_version         "$Qt4"/lib/libQtSvg.so.4
        sudo cp "$lib"/libQtXml.so.$Qt4_version         "$Qt4"/lib/libQtXml.so.4
        sudo cp "$lib"/libQtXmlPatterns.so.$Qt4_version "$Qt4"/lib/libQtXmlPatterns.so.4

        sudo cp "$lib"/libQtWebKit.so.$QtWebkit_version_linux "$Qt4"/lib/libQtWebKit.so.4

        mkdir -p "$Qt4"/plugins/imageformats

        sudo cp "$lib"/qt4/plugins/imageformats/libqsvg.so  "$Qt4"/plugins/imageformats
        sudo cp "$lib"/qt4/plugins/imageformats/libqjpeg.so "$Qt4"/plugins/imageformats

        echo ""
        echo "DEPLOYING VLC"

        mkdir -p "$VLC"

        sudo cp "$usr"/libvlc.so.$libvlc_version_linux         "$VLC"/libvlc.so.5
        sudo cp "$usr"/libvlccore.so.$libvlccore_version_linux "$VLC"/libvlccore.so.8

        sudo cp -r "$usr"/vlc/plugins "$VLC"

        echo ""
        echo "DEPLOYING libtorrent"

        mkdir -p "$libtorrent"

        sudo cp "$usr"/libtorrent-rasterbar.so.$libtorrent_version_linux \
                "$libtorrent"/libtorrent-rasterbar.so.9

        echo ""
        echo "DEPLOYING Boost"

        mkdir -p "$Boost"

        sudo cp "$lib"/libboost_system.so.$Boost_version_linux "$Boost"
        sudo cp "$lib"/libboost_random.so.$Boost_version_linux "$Boost"
        sudo cp "$lib"/libboost_chrono.so.$Boost_version_linux "$Boost"
    fi
fi
