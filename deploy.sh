#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.12.3"

MinGW_version="7.3.0"

VLC_version="3.0.6"

libtorrent_version="1.2.2"

#--------------------------------------------------------------------------------------------------

bin4="bin"
bin5="latest"

#--------------------------------------------------------------------------------------------------
# Linux

base32="/lib/i386-linux-gnu"
base64="/lib/x86_64-linux-gnu"

lib32="/usr/lib/i386-linux-gnu"
lib64="/usr/lib/x86_64-linux-gnu"

Qt5_version_linux="5.9.5"

VLC_version_linux="5.6.0"

libtorrent_version_linux="9.0.0"

Boost_version_linux="1.65.1"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 2 -a $# != 3 ] \
   || \
   [ $1 != "qt4" -a $1 != "qt5" -a $1 != "clean" ] \
   || \
   [ $2 != "win32" -a $2 != "win64" -a $2 != "macOS" -a $2 != "linux" ] \
   || \
   [ $# = 3 -a "$3" != "tools" ]; then

    echo "Usage: deploy <qt4 | qt5 | clean> <win32 | win64 | macOS | linux> [tools]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $2 = "win32" -o $2 = "win64" ]; then

    windows=true

    external="$external/$2"

    MinGW="$external/MinGW/$MinGW_version/bin"

elif [ $2 = "macOS" ]; then

    windows=false

    external="$external/$2"

elif [ $2 = "linux" ]; then

    windows=false

    if [ -d "${lib64}" ]; then

        base="$base64"

        lib="$lib64"
    else
        base="$base32"

        lib="$lib32"
    fi

    Qt5_version="$Qt5_version_linux"

    VLC_version="$VLC_version_linux"

    libtorrent_version="$libtorrent_version_linux"
else
    windows=false
fi

#--------------------------------------------------------------------------------------------------

if [ $1 = "qt4" ]; then

    Qt="$external/Qt/$Qt4_version"
else
    Qt="$external/Qt/$Qt5_version"
fi

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

    if [ $windows = true ]; then

        cp "$MinGW"/libgcc_s_*-1.dll    deploy
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

    mkdir deploy/platforms
    mkdir deploy/imageformats
    mkdir deploy/QtQuick.2

    if [ $windows = true ]; then

        cp "$MinGW"/libgcc_s_*-1.dll    deploy
        cp "$MinGW"/libstdc++-6.dll     deploy
        cp "$MinGW"/libwinpthread-1.dll deploy

        cp "$Qt"/bin/libEGL.dll    deploy
        cp "$Qt"/bin/libGLESv2.dll deploy

        cp "$Qt"/bin/Qt5Core.dll        deploy
        cp "$Qt"/bin/Qt5Gui.dll         deploy
        cp "$Qt"/bin/Qt5Network.dll     deploy
        cp "$Qt"/bin/Qt5OpenGL.dll      deploy
        cp "$Qt"/bin/Qt5Qml.dll         deploy
        cp "$Qt"/bin/Qt5Quick.dll       deploy
        cp "$Qt"/bin/Qt5Svg.dll         deploy
        cp "$Qt"/bin/Qt5Widgets.dll     deploy
        cp "$Qt"/bin/Qt5Xml.dll         deploy
        cp "$Qt"/bin/Qt5XmlPatterns.dll deploy
        cp "$Qt"/bin/Qt5WinExtras.dll   deploy

        cp "$Qt"/plugins/platforms/qwindows.dll deploy/platforms

        cp "$Qt"/plugins/imageformats/qsvg.dll  deploy/imageformats
        cp "$Qt"/plugins/imageformats/qjpeg.dll deploy/imageformats

        cp "$Qt"/qml/QtQuick.2/qtquick2plugin.dll deploy/QtQuick.2
        cp "$Qt"/qml/QtQuick.2/qmldir             deploy/QtQuick.2

    elif [ $2 = "macOS" ]; then

        cp "$Qt"/lib/QtCore.framework/Versions/5/QtCore               deploy
        cp "$Qt"/lib/QtGui.framework/Versions/5/QtGui                 deploy
        cp "$Qt"/lib/QtNetwork.framework/Versions/5/QtNetwork         deploy
        cp "$Qt"/lib/QtQml.framework/Versions/5/QtQml                 deploy
        cp "$Qt"/lib/QtQuick.framework/Versions/5/QtQuick             deploy
        cp "$Qt"/lib/QtSvg.framework/Versions/5/QtSvg                 deploy
        cp "$Qt"/lib/QtWidgets.framework/Versions/5/QtWidgets         deploy
        cp "$Qt"/lib/QtXml.framework/Versions/5/QtXml                 deploy
        cp "$Qt"/lib/QtXmlPatterns.framework/Versions/5/QtXmlPatterns deploy
        cp "$Qt"/lib/QtCore.framework/Versions/5/QtCore               deploy

        cp "$Qt"/plugins/platforms/libqcocoa.dylib deploy/platforms

        cp "$Qt"/plugins/imageformats/libqsvg.dylib  deploy/imageformats
        cp "$Qt"/plugins/imageformats/libqjpeg.dylib deploy/imageformats

        cp "$Qt"/qml/QtQuick.2/libqtquick2plugin.dylib deploy/QtQuick.2
        cp "$Qt"/qml/QtQuick.2/qmldir                  deploy/QtQuick.2

    elif [ $2 = "linux" ]; then

        mkdir deploy/xcbglintegrations

        sudo cp "$base"/libz.so.1 deploy

        sudo cp "$lib"/libicudata.so.60 deploy
        sudo cp "$lib"/libicui18n.so.60 deploy
        sudo cp "$lib"/libicuuc.so.60   deploy

        sudo cp "$lib"/libpng16.so.16       deploy
        sudo cp "$lib"/libharfbuzz.so.0     deploy
        sudo cp "$lib"/libxcb-xinerama.so.0 deploy

        cp "$Qt"/lib/libQt5Core.so.5        deploy
        cp "$Qt"/lib/libQt5Gui.so.5         deploy
        cp "$Qt"/lib/libQt5Network.so.5     deploy
        cp "$Qt"/lib/libQt5OpenGL.so.5      deploy
        cp "$Qt"/lib/libQt5Qml.so.5         deploy
        cp "$Qt"/lib/libQt5Quick.so.5       deploy
        cp "$Qt"/lib/libQt5Svg.so.5         deploy
        cp "$Qt"/lib/libQt5Widgets.so.5     deploy
        cp "$Qt"/lib/libQt5Xml.so.5         deploy
        cp "$Qt"/lib/libQt5XmlPatterns.so.5 deploy
        cp "$Qt"/lib/libQt5XcbQpa.so.5      deploy
        cp "$Qt"/lib/libQt5DBus.so.5        deploy

        cp "$Qt"/plugins/platforms/libqxcb.so deploy/platforms

        cp "$Qt"/plugins/imageformats/libqsvg.so  deploy/imageformats
        cp "$Qt"/plugins/imageformats/libqjpeg.so deploy/imageformats

        cp "$Qt"/plugins/xcbglintegrations/libqxcb-egl-integration.so deploy/xcbglintegrations
        cp "$Qt"/plugins/xcbglintegrations/libqxcb-glx-integration.so deploy/xcbglintegrations

        cp "$Qt"/qml/QtQuick.2/libqtquick2plugin.so deploy/QtQuick.2
        cp "$Qt"/qml/QtQuick.2/qmldir               deploy/QtQuick.2
    fi

    bin="$bin5"
fi

#--------------------------------------------------------------------------------------------------
# SSL
#--------------------------------------------------------------------------------------------------

echo "COPYING SSL"

if [ $windows = true ]; then

    cp "$SSL"/libeay32.dll deploy
    cp "$SSL"/ssleay32.dll deploy

elif [ $2 = "linux" ]; then

    sudo cp "$lib"/libssl.so.1.1    deploy
    sudo cp "$lib"/libcrypto.so.1.1 deploy
fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

echo "COPYING VLC"

if [ $windows = true ]; then

    mkdir deploy/plugins

    cp -r "$VLC"/plugins/access        deploy/plugins
    cp -r "$VLC"/plugins/audio_filter  deploy/plugins
    cp -r "$VLC"/plugins/audio_mixer   deploy/plugins
    cp -r "$VLC"/plugins/audio_output  deploy/plugins
    cp -r "$VLC"/plugins/codec         deploy/plugins
    cp -r "$VLC"/plugins/control       deploy/plugins
    cp -r "$VLC"/plugins/demux         deploy/plugins
    cp -r "$VLC"/plugins/misc          deploy/plugins
    cp -r "$VLC"/plugins/packetizer    deploy/plugins
    cp -r "$VLC"/plugins/stream_filter deploy/plugins
    cp -r "$VLC"/plugins/stream_out    deploy/plugins
    cp -r "$VLC"/plugins/video_chroma  deploy/plugins
    cp -r "$VLC"/plugins/video_filter  deploy/plugins
    cp -r "$VLC"/plugins/video_output  deploy/plugins

    cp "$VLC"/libvlc*.dll deploy

elif [ $2 = "macOS" ]; then

    mkdir -p deploy/vlc/plugins

    cp -r "$VLC"/plugins/*.dylib deploy/vlc/plugins

    cp "$VLC"/libvlc*.dylib* deploy

#elif [ $2 = "linux" ]; then

    #mkdir -p deploy/vlc/plugins

    #sudo cp -r "$VLC"/plugins/access        deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/audio_filter  deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/audio_mixer   deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/audio_output  deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/codec         deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/control       deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/demux         deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/misc          deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/packetizer    deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/stream_filter deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/stream_out    deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/video_chroma  deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/video_filter  deploy/vlc/plugins
    #sudo cp -r "$VLC"/plugins/video_output  deploy/vlc/plugins

    #sudo cp "$VLC"/libvlc*.so* deploy
fi

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

echo "COPYING libtorrent"

if [ $windows = true ]; then

    cp "$libtorrent"/libtorrent.dll deploy

elif [ $2 = "linux" ]; then

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

if [ "$3" != "tools" ]; then

    echo "COPYING Sky"

    if [ $windows = true ]; then

        cp "$bin"/SkCore.dll    deploy
        cp "$bin"/SkGui.dll     deploy
        cp "$bin"/SkMedia.dll   deploy
        #cp "$bin"/SkWeb.dll     deploy
        cp "$bin"/SkTorrent.dll deploy
        cp "$bin"/SkBackend.dll deploy

    elif [ $2 = "macOS" ]; then

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
fi

echo "COPYING tools"

cp "$bin"/includeGenerator* deploy
cp "$bin"/deployer*         deploy
