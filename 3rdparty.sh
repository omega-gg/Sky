#!/bin/sh

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.10.1"

VLC_version="2.2.8"

libtorrent_version="1.1.6"

Boost_version="1.55.0"

#--------------------------------------------------------------------------------------------------
# Windows

archive_win32="http://omega.gg/get/Sky/3rdparty/win32"

#--------------------------------------------------------------------------------------------------
# Linux

usr="/usr/lib"

lib32="/usr/lib/i386-linux-gnu"
lib64="/usr/lib/x86_64-linux-gnu"

Qt5_version_linux="5.9.1"

#--------------------------------------------------------------------------------------------------
# Ubuntu

QtWebkit_version_ubuntu="4.10.2"

libvlc_version_ubuntu="5.5.0"

libvlccore_version_ubuntu="8.0.0"

libtorrent_version_ubuntu="9.0.0"

Boost_version_ubuntu="1.62.0"

#--------------------------------------------------------------------------------------------------

X11_ubuntu="libx11-dev libxi-dev libxinerama-dev libxrandr-dev libxcursor-dev libfontconfig-dev"

Qt4_ubuntu="qt4-default libqtwebkit-dev openssl"
Qt5_ubuntu="qt5-default openssl"

VLC_ubuntu="libvlc-dev vlc"

libtorrent_ubuntu="libtorrent-rasterbar-dev"

Boost_ubuntu="libboost-all-dev"

tools_ubuntu="git"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 3 ] \
   || \
   [ $1 != "all" -a $1 != "install" -a $1 != "uninstall" -a $1 != "deploy" -a $1 != "clean" ] \
   || \
   [ $2 != "qt4" -a $2 != "qt5" ] \
   || \
   [ $3 != "win32" -a $3 != "osx" -a $3 != "ubuntu" ]; then

    echo "Usage: 3rdparty <all | install | uninstall | deploy | clean> <qt4 | qt5> " \
         "<win32 | osx | ubuntu>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $3 = "ubuntu" ]; then

    linux=true

    #----------------------------------------------------------------------------------------------

    if [ $2 = "qt4" ]; then

        QtWebkit_version="$QtWebkit_version_ubuntu"
    else
        Qt5_version="$Qt5_version_linux"
    fi

    libvlc_version="$libvlc_version_ubuntu"

    libvlccore_version="$libvlccore_version_ubuntu"

    libtorrent_version="$libtorrent_version_ubuntu"

    Boost_version="$Boost_version_ubuntu"

    #----------------------------------------------------------------------------------------------

    if [ $2 = "qt4" ]; then

        Qt_linux="$Qt4_ubuntu"
    else
        Qt_linux="$Qt5_ubuntu"
    fi

    X11_linux="$X11_ubuntu"

    VLC_linux="$VLC_ubuntu"

    libtorrent_linux="$libtorrent_ubuntu"

    Boost_linux="$Boost_ubuntu"

    tools_linux="$tools_ubuntu"
else
    linux=false
fi

if [ $2 = "qt4" ]; then

    Qt="$external/Qt/$Qt4_version"

    Qt_name="qt-everywhere-opensource-src-$Qt4_version"

    Qt_archive="$Qt_name.tar.gz"

    Qt_sources="http://download.qt.io/official_releases/qt/4.8/$Qt4_version/$Qt_archive"
else
    Qt="$external/Qt/$Qt5_version"

    Qt_name="qt-everywhere-opensource-src-$Qt5_version"

    Qt_archive="$Qt_name.tar.xz"

    Qt_sources="http://download.qt.io/official_releases/qt/5.9/$Qt5_version/single/$Qt_archive"
fi

if [ $linux = true ]; then

    if [ -d "${lib64}" ]; then

        lib="$lib64"
    else
        lib="$lib32"
    fi

    Boost="$external/Boost/$Boost_version"
fi

#--------------------------------------------------------------------------------------------------

VLC="$external/VLC/$VLC_version"

libtorrent="$external/libtorrent/$libtorrent_version"

Boost="$external/Boost/$Boost_version"

#--------------------------------------------------------------------------------------------------
# Install
#--------------------------------------------------------------------------------------------------

if [ $1 = "all" ] || [ $1 = "install" ]; then

    if [ $3 = "osx" ]; then

        sudo chown -R $(whoami) /usr/local

        echo ""
        echo "INSTALLING Qt"

        brew install -y Qt@5.5

        echo ""
        echo "INSTALLING SSL"

        brew install -y openssl

        echo ""
        echo "INSTALLING VLC"

        brew cask install vlc

        echo ""
        echo "INSTALLING Boost"

        brew install -y boost@1.55

        echo ""
        echo "INSTALLING libtorrent"

        brew install -y libtorrent-rasterbar

    elif [ $linux = true ]; then

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

    if [ $3 = "osx" ]; then

        echo ""
        echo "UNINSTALLING Qt"

        brew remove -y Qt@5.5

        echo ""
        echo "UNINSTALLING VLC"

        brew cask remove vlc

        echo ""
        echo "UNINSTALLING Boost"

        brew remove -y boost@1.55

        echo ""
        echo "UNINSTALLING libtorrent"

        brew remove -y libtorrent-rasterbar

        # We have to remove OpenSSL after libtorrent
        echo ""
        echo "UNINSTALLING SSL"

        brew remove -y openssl

    elif [ $linux = true ]; then

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

    sudo rm -rf "$Qt"
    sudo rm -rf "$VLC"
    sudo rm -rf "$libtorrent"
    sudo rm -rf "$Boost_linux"

    exit 0
fi

#--------------------------------------------------------------------------------------------------
# Deploy
#--------------------------------------------------------------------------------------------------

if [ $1 = "all" ] || [ $1 = "deploy" ]; then

    if [ $3 = "win32" ]; then

        echo "3rdparty archive -> $archive_win32"

    elif [ $linux = true ]; then

        echo "DEPLOYING Qt"

        if [ ! -d "${Qt}" ]; then

            mkdir -p "$Qt"

            cd "$Qt"

            wget "$Qt_sources"

            tar -xf "$Qt_archive"

            mv "$Qt_name"/* .

            rm -rf "$Qt_name"
            rm "$Qt_archive"

            cd -
        fi

        if [ $2 = "qt4" ]; then

            sudo cp "$lib"/libQtCore.so.$Qt_version        "$Qt"/lib/libQtCore.so.4
            sudo cp "$lib"/libQtGui.so.$Qt_version         "$Qt"/lib/libQtGui.so.4
            sudo cp "$lib"/libQtDeclarative.so.$Qt_version "$Qt"/lib/libQtDeclarative.so.4
            sudo cp "$lib"/libQtNetwork.so.$Qt_version     "$Qt"/lib/libQtNetwork.so.4
            sudo cp "$lib"/libQtOpenGL.so.$Qt_version      "$Qt"/lib/libQtOpenGL.so.4
            sudo cp "$lib"/libQtScript.so.$Qt_version      "$Qt"/lib/libQtScript.so.4
            sudo cp "$lib"/libQtSql.so.$Qt_version         "$Qt"/lib/libQtSql.so.4
            sudo cp "$lib"/libQtSvg.so.$Qt_version         "$Qt"/lib/libQtSvg.so.4
            sudo cp "$lib"/libQtXml.so.$Qt_version         "$Qt"/lib/libQtXml.so.4
            sudo cp "$lib"/libQtXmlPatterns.so.$Qt_version "$Qt"/lib/libQtXmlPatterns.so.4

            sudo cp "$lib"/libQtWebKit.so.$QtWebkit_version "$Qt"/lib/libQtWebKit.so.4

            mkdir -p "$Qt"/plugins/imageformats

            sudo cp "$lib"/qt4/plugins/imageformats/libqsvg.so  "$Qt"/plugins/imageformats
            sudo cp "$lib"/qt4/plugins/imageformats/libqjpeg.so "$Qt"/plugins/imageformats
        fi

        echo ""
        echo "DEPLOYING VLC"

        mkdir -p "$VLC"

        sudo cp "$lib"/libvlc.so.$libvlc_version         "$VLC"/libvlc.so.5
        sudo cp "$lib"/libvlccore.so.$libvlccore_version "$VLC"/libvlccore.so.8

        sudo cp -r "$lib"/vlc/plugins "$VLC"

        echo ""
        echo "DEPLOYING libtorrent"

        mkdir -p "$libtorrent"

        sudo cp "$lib"/libtorrent-rasterbar.so.$libtorrent_version \
                "$libtorrent"/libtorrent-rasterbar.so.9

        echo ""
        echo "DEPLOYING Boost"

        mkdir -p "$Boost"

        sudo cp "$lib"/libboost_system.so.$Boost_version "$Boost"
        sudo cp "$lib"/libboost_random.so.$Boost_version "$Boost"
        sudo cp "$lib"/libboost_chrono.so.$Boost_version "$Boost"
    fi
fi
