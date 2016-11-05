#!/bin/sh

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"

VLC_version="2.2.4"

libtorrent_version="1.1.0"

Boost_version="1.55.0"

#--------------------------------------------------------------------------------------------------

Qt4="$external/Qt/$Qt4_version"

VLC="$external/VLC/$VLC_version"

libtorrent="$external/libtorrent/$libtorrent_version"

Boost="$external/Boost/$Boost_version"

#--------------------------------------------------------------------------------------------------

libtorrent_name="libtorrent-rasterbar-$libtorrent_version"

libtorrent_archive="$libtorrent_name.tar.gz"

#--------------------------------------------------------------------------------------------------

Qt4_sources="https://github.com/qt/qt.git"

VLC_sources="https://github.com/videolan/vlc.git"

libtorrent_sources="https://github.com/arvidn/libtorrent/releases/download/libtorrent-1_1/$libtorrent_archive"

#--------------------------------------------------------------------------------------------------
# Linux

lib="/usr/lib"

lib64="/usr/lib/x86_64-linux-gnu"

#--------------------------------------------------------------------------------------------------

QtWebkit_version="4.10.2"

libvlc_version="5.5.0"

libvlccore_version="8.0.0"

libtorrent_version_linux="9.0.0"

Boost_version_linux="1.61.0"

#--------------------------------------------------------------------------------------------------

Boost_linux="$external/Boost/$Boost_version_linux"

#--------------------------------------------------------------------------------------------------

X11_dependecy="libx11-dev libxi-dev libxinerama-dev libxrandr-dev libxcursor-dev libfontconfig-dev"

Qt_dependecy="qt4-default libqtwebkit-dev openssl"

VLC_dependecy="libvlc-dev vlc"

libtorrent_dependecy="libtorrent-rasterbar-dev"

Boost_dependecy="libboost-all-dev"

tools_dependecy="git"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 2 ] || [ $1 != "all"       -a \
                    $1 != "install"   -a \
                    $1 != "uninstall" -a \
                    $1 != "deploy" ] || [ $2 != "linux32" -a $2 != "linux64" ]; then

    echo "Usage: 3rdparty <all | install | uninstall | deploy> <linux32 | linux64>"

    exit 1
fi

if [ $2 = "linux32" ]; then

    lib64="$lib"
fi

#--------------------------------------------------------------------------------------------------
# Install
#--------------------------------------------------------------------------------------------------

if [ $1 = "all" ] || [ $1 = "install" ]; then

    if [ $2 = "linux32" ] || [ $2 = "linux64" ]; then

        echo "INSTALLING X11"

        sudo apt-get install $X11_dependecy

        echo ""
        echo "INSTALLING Qt"

        sudo apt-get install $Qt_dependecy

        echo ""
        echo "INSTALLING libtorrent"

        sudo apt-get install $libtorrent_dependecy

        echo ""
        echo "INSTALLING Boost"

        sudo apt-get install $Boost_dependecy

        echo ""
        echo "INSTALLING TOOLS"

        sudo apt-get install $tools_dependecy
    fi
fi

if [ $1 = "uninstall" ]; then

    if [ $2 = "linux32" ] || [ $2 = "linux64" ]; then

        echo "UNINSTALLING X11"

        sudo apt-get remove $X11_dependecy

        echo ""
        echo "UNINSTALLING Qt"

        sudo apt-get remove $Qt_dependecy

        echo ""
        echo "UNINSTALLING libtorrent"

        sudo apt-get remove $libtorrent_dependecy

        echo ""
        echo "UNINSTALLING Boost"

        sudo apt-get remove $Boost_dependecy

        echo ""
        echo "UNINSTALLING TOOLS"

        sudo apt-get remove $tools_dependecy
    fi
fi

if [ $1 = "all" ]; then

    echo ""
fi

#--------------------------------------------------------------------------------------------------
# Deploy
#--------------------------------------------------------------------------------------------------

if [ $1 = "all" ] || [ $1 = "deploy" ]; then

    if [ $2 = "linux32" ] || [ $2 = "linux64" ]; then

        echo "DEPLOYING Qt"

        git clone "$Qt4_sources" "$Qt4"

        sudo cp "$lib64"/libQtCore.so.$Qt4_version        "$Qt4"/lib/libQtCore.so.4
        sudo cp "$lib64"/libQtDeclarative.so.$Qt4_version "$Qt4"/lib/libQtDeclarative.so.4
        sudo cp "$lib64"/libQtGui.so.$Qt4_version         "$Qt4"/lib/libQtGui.so.4
        sudo cp "$lib64"/libQtNetwork.so.$Qt4_version     "$Qt4"/lib/libQtNetwork.so.4
        sudo cp "$lib64"/libQtOpenGL.so.$Qt4_version      "$Qt4"/lib/libQtOpenGL.so.4
        sudo cp "$lib64"/libQtScript.so.$Qt4_version      "$Qt4"/lib/libQtScript.so.4
        sudo cp "$lib64"/libQtSql.so.$Qt4_version         "$Qt4"/lib/libQtSql.so.4
        sudo cp "$lib64"/libQtSvg.so.$Qt4_version         "$Qt4"/lib/libQtSvg.so.4
        sudo cp "$lib64"/libQtXml.so.$Qt4_version         "$Qt4"/lib/libQtXml.so.4
        sudo cp "$lib64"/libQtXmlPatterns.so.$Qt4_version "$Qt4"/lib/libQtXmlPatterns.so.4

        sudo cp "$lib64"/libQtWebKit.so.$QtWebkit_version "$Qt4"/lib/libQtWebKit.so.4

        mkdir -p "$Qt4"/plugins/imageformats

        sudo cp "$lib64"/qt4/plugins/imageformats/libqsvg.so  "$Qt4"/plugins/imageformats
        sudo cp "$lib64"/qt4/plugins/imageformats/libqjpeg.so "$Qt4"/plugins/imageformats

        echo ""
        echo "DEPLOYING VLC"

        mkdir -p "$VLC"

        sudo cp "$lib"/libvlc.so.$libvlc_version         "$VLC"/libvlc.so.5
        sudo cp "$lib"/libvlccore.so.$libvlccore_version "$VLC"/libvlccore.so.8

        sudo cp -r "$lib"/vlc/plugins "$VLC"

        echo ""
        echo "DEPLOYING libtorrent"

        mkdir -p "$libtorrent"

        cd "$libtorrent"

        wget "$libtorrent_sources"

        tar -xvzf "$libtorrent_archive"

        mv "$libtorrent_name"/* .
        rm "$libtorrent_name"

        ./configure

        make

        sudo make install

        cd -

        sudo cp "$lib"/libtorrent-rasterbar.so.$libtorrent_version_linux "$VLC"/libtorrent-rasterbar.so.9

        echo ""
        echo "DEPLOYING Boost"

        mkdir -p "$Boost_linux"

        sudo cp "$lib64"/libboost_system.so.$Boost_version_linux "$Boost_linux"
        sudo cp "$lib64"/libboost_random.so.$Boost_version_linux "$Boost_linux"
        sudo cp "$lib64"/libboost_chrono.so.$Boost_version_linux "$Boost_linux"
    fi
fi
