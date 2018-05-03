#!/bin/sh

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.10.1"

MinGW_version="5.3.0"

VLC_version="2.2.8"

libtorrent_version="1.1.6"

#--------------------------------------------------------------------------------------------------

bin4="bin"
bin5="latest"

#--------------------------------------------------------------------------------------------------
# Linux

lib32="/usr/lib/i386-linux-gnu"
lib64="/usr/lib/x86_64-linux-gnu"

Qt5_version_linux="5.9.5"

VLC_version_linux="5.6.0"

libtorrent_version_linux="9.0.0"

Boost_version_linux="1.65.1"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 2 ] || [ $1 != "qt4" -a $1 != "qt5" -a $1 != "clean" ] || [ $2 != "win32" -a \
                                                                       $2 != "osx"   -a \
                                                                       $2 != "linux" ]; then

    echo "Usage: deploy <qt4 | qt5 | clean> <win32 | osx | linux>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $2 = "linux" ]; then

    if [ -d "${lib64}" ]; then

        lib="$lib64"
    else
        lib="$lib32"
    fi

    Qt5_version="$Qt5_version_linux"

    VLC_version="$VLC_version_linux"

    libtorrent_version="$libtorrent_version_linux"
fi

#--------------------------------------------------------------------------------------------------

if [ $1 = "qt4" ]; then

    Qt="$external/Qt/$Qt4_version"
else
    Qt="$external/Qt/$Qt5_version"
fi

MinGW="$external/MinGW/$MinGW_version/bin"

SSL="$external/OpenSSL"

VLC="$external/VLC/$VLC_version"

libtorrent="$external/libtorrent/$libtorrent_version"

if [ $2 = "linux" ]; then

    Boost="$external/Boost/$Boost_version_linux"
fi

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

echo "CLEANING"

rm -rf deploy/*

touch deploy/.gitignore

if [ $1 = "clean" ]; then

    exit 0
fi

echo ""

#--------------------------------------------------------------------------------------------------
# Qt
#--------------------------------------------------------------------------------------------------

if [ $1 = "qt4" ]; then

    echo "COPYING Qt4"

    mkdir deploy/imageformats

    if [ $2 = "win32" ]; then

        cp "$MinGW"/libgcc_s_dw2-1.dll  deploy
        cp "$MinGW"/libstdc++-6.dll     deploy
        cp "$MinGW"/libwinpthread-1.dll deploy

        cp "$Qt"/bin/QtCore4.dll        deploy
        cp "$Qt"/bin/QtGui4.dll         deploy
        cp "$Qt"/bin/QtDeclarative4.dll deploy
        cp "$Qt"/bin/QtNetwork4.dll     deploy
        cp "$Qt"/bin/QtOpenGL4.dll      deploy
        cp "$Qt"/bin/QtScript4.dll      deploy
        cp "$Qt"/bin/QtSql4.dll         deploy
        cp "$Qt"/bin/QtSvg4.dll         deploy
        cp "$Qt"/bin/QtWebKit4.dll      deploy
        cp "$Qt"/bin/QtXml4.dll         deploy
        cp "$Qt"/bin/QtXmlPatterns4.dll deploy

        cp "$Qt"/plugins/imageformats/qsvg4.dll  deploy/imageformats
        cp "$Qt"/plugins/imageformats/qjpeg4.dll deploy/imageformats

    elif [ $2 = "linux" ]; then

        sudo cp "$lib"/libpng16.so.16 deploy

        cp "$Qt"/lib/libQtCore.so.4        deploy
        cp "$Qt"/lib/libQtGui.so.4         deploy
        cp "$Qt"/lib/libQtDeclarative.so.4 deploy
        cp "$Qt"/lib/libQtNetwork.so.4     deploy
        cp "$Qt"/lib/libQtOpenGL.so.4      deploy
        cp "$Qt"/lib/libQtScript.so.4      deploy
        cp "$Qt"/lib/libQtSql.so.4         deploy
        cp "$Qt"/lib/libQtSvg.so.4         deploy
        cp "$Qt"/lib/libQtWebKit.so.4      deploy
        cp "$Qt"/lib/libQtXml.so.4         deploy
        cp "$Qt"/lib/libQtXmlPatterns.so.4 deploy

        cp "$Qt"/plugins/imageformats/libqsvg.so  deploy/imageformats
        cp "$Qt"/plugins/imageformats/libqjpeg.so deploy/imageformats
    fi

    bin="$bin4"

else

    echo "COPYING Qt5"

    mkdir deploy/imageformats
    mkdir deploy/QtQuick.2

    if [ $2 = "win32" ]; then

        cp "$MinGW"/libgcc_s_dw2-1.dll  deploy
        cp "$MinGW"/libstdc++-6.dll     deploy
        cp "$MinGW"/libwinpthread-1.dll deploy

        cp "$Qt"/bin/Qt5Core.dll              deploy
        cp "$Qt"/bin/Qt5Gui.dll               deploy
        cp "$Qt"/bin/Qt5Network.dll           deploy
        cp "$Qt"/bin/Qt5OpenGL.dll            deploy
        cp "$Qt"/bin/Qt5Qml.dll               deploy
        cp "$Qt"/bin/Qt5Quick.dll             deploy
        cp "$Qt"/bin/Qt5Script.dll            deploy
        cp "$Qt"/bin/Qt5Svg.dll               deploy
        cp "$Qt"/bin/Qt5Widgets.dll           deploy
        cp "$Qt"/bin/Qt5Xml.dll               deploy
        cp "$Qt"/bin/Qt5XmlPatterns.dll       deploy
        cp "$Qt"/bin/Qt5WinExtras.dll         deploy

        cp "$Qt"/plugins/imageformats/qsvg.dll  deploy/imageformats
        cp "$Qt"/plugins/imageformats/qjpeg.dll deploy/imageformats

        mkdir deploy/platforms

        cp "$Qt"/plugins/platforms/qwindows.dll deploy/platforms

        cp "$Qt"/qml/QtQuick.2/qtquick2plugin.dll deploy/QtQuick.2
        cp "$Qt"/qml/QtQuick.2/qmldir             deploy/QtQuick.2

    elif [ $2 = "linux" ]; then

        sudo cp "$lib"/libpng16.so.16 deploy

        cp "$Qt"/lib/libQt5Core.so.5        deploy
        cp "$Qt"/lib/libQt5Gui.so.5         deploy
        cp "$Qt"/lib/libQt5Network.so.5     deploy
        cp "$Qt"/lib/libQt5OpenGL.so.5      deploy
        cp "$Qt"/lib/libQt5Qml.so.5         deploy
        cp "$Qt"/lib/libQt5Quick.so.5       deploy
        cp "$Qt"/lib/libQt5Script.so.5      deploy
        cp "$Qt"/lib/libQt5Svg.so.5         deploy
        cp "$Qt"/lib/libQt5Widgets.so.5     deploy
        cp "$Qt"/lib/libQt5Xml.so.5         deploy
        cp "$Qt"/lib/libQt5XmlPatterns.so.5 deploy

        cp "$Qt"/plugins/imageformats/libqsvg.so  deploy/imageformats
        cp "$Qt"/plugins/imageformats/libqjpeg.so deploy/imageformats

        cp "$Qt"/qml/QtQuick.2/libqtquick2plugin.so deploy/QtQuick.2
        cp "$Qt"/qml/QtQuick.2/qmldir               deploy/QtQuick.2
    fi

    bin="$bin5"
fi

#--------------------------------------------------------------------------------------------------
# SSL
#--------------------------------------------------------------------------------------------------

echo "COPYING SSL"

if [ $2 = "win32" ]; then

    cp "$SSL"/libeay32.dll deploy
    cp "$SSL"/ssleay32.dll deploy

elif [ $2 = "linux" ]; then

    sudo cp "$lib"/libssl.so.1.0.0    deploy
    sudo cp "$lib"/libcrypto.so.1.0.0 deploy
fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

echo "COPYING VLC"

if [ $2 = "win32" ]; then

    mkdir deploy/plugins

    cp -r "$VLC"/plugins/access       deploy/plugins
    cp -r "$VLC"/plugins/audio_filter deploy/plugins
    cp -r "$VLC"/plugins/audio_mixer  deploy/plugins
    cp -r "$VLC"/plugins/audio_output deploy/plugins
    cp -r "$VLC"/plugins/codec        deploy/plugins
    cp -r "$VLC"/plugins/control      deploy/plugins
    cp -r "$VLC"/plugins/demux        deploy/plugins
    cp -r "$VLC"/plugins/misc         deploy/plugins
    cp -r "$VLC"/plugins/packetizer   deploy/plugins
    cp -r "$VLC"/plugins/video_chroma deploy/plugins
    cp -r "$VLC"/plugins/video_filter deploy/plugins
    cp -r "$VLC"/plugins/video_output deploy/plugins

    cp "$VLC"/libvlc*.dll deploy

elif [ $2 = "linux" ]; then

    #mkdir -p deploy/vlc/plugins

    #sudo cp -r "$VLC"/plugins/access       deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/audio_filter deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/audio_mixer  deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/audio_output deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/codec        deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/control      deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/demux        deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/misc         deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/packetizer   deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/video_chroma deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/video_filter deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/video_output deploy/vlc/plugins

    #sudo cp "$VLC"/libvlc*.so* deploy

    sudo cp "$lib"/libaudio.so.2.4 deploy/libaudio.so.2
fi

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

if [ $2 = "linux" ]; then

    echo "COPYING libtorrent"

    cp "$libtorrent"/libtorrent*.so* deploy
fi

#--------------------------------------------------------------------------------------------------
# Boost
#--------------------------------------------------------------------------------------------------

if [ $2 = "linux" ]; then

    echo "COPYING Boost"

    cp "$Boost"/libboost*.so* deploy
fi

#--------------------------------------------------------------------------------------------------
# Sky
#--------------------------------------------------------------------------------------------------

echo "COPYING Sky"

if [ $2 = "win32" ]; then

    cp "$bin"/SkCore.dll    deploy
    cp "$bin"/SkGui.dll     deploy
    cp "$bin"/SkMedia.dll   deploy
    #cp "$bin"/SkWeb.dll     deploy
    cp "$bin"/SkTorrent.dll deploy
    cp "$bin"/SkBackend.dll deploy

elif [ $2 = "osx" ]; then

    cp "$bin"/libSkCore.dylib    deploy
    cp "$bin"/libSkGui.dylib     deploy
    cp "$bin"/libSkMedia.dylib   deploy
    #cp "$bin"/libSkWeb.dylib     deploy
    cp "$bin"/libSkTorrent.dylib deploy
    cp "$bin"/libSkBackend.dylib deploy

elif [ $2 = "linux" ]; then

    cp "$bin"/libSkCore.so    deploy
    cp "$bin"/libSkGui.so     deploy
    cp "$bin"/libSkMedia.so   deploy
    #cp "$bin"/libSkWeb.so     deploy
    cp "$bin"/libSkTorrent.so deploy
    cp "$bin"/libSkBackend.so deploy
fi

cp "$bin"/includeGenerator* deploy
cp "$bin"/deployer*         deploy
