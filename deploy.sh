#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.15.2"

SSL_versionA="1.0.2u"
SSL_versionB="1.1.1k"

VLC_version="3.0.16"

libtorrent_version="2.0.4"

Boost_version="1.71.0"

#--------------------------------------------------------------------------------------------------
# Windows

MinGW_version="7.3.0"

#--------------------------------------------------------------------------------------------------
# environment

compiler_win="mingw"

qt="qt5"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 1 -a $# != 2 ] \
   || \
   [ $1 != "win32" -a $1 != "win64" -a $1 != "macOS" -a $1 != "linux" -a $1 != "android" ] \
   || \
   [ $# = 2 -a "$2" != "tools" -a "$2" != "clean" ]; then

    echo "Usage: deploy <win32 | win64 | macOS | linux | android> [tools | clean]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

external="$external/$1"

if [ $1 = "win32" -o $1 = "win64" ]; then

    os="windows"

    compiler="$compiler_win"

    if [ $compiler = "mingw" ]; then

        MinGW="$external/MinGW/$MinGW_version/bin"
    fi

elif [ $1 = "macOS" ]; then

    os="default"

    compiler="default"

elif [ $1 = "linux" ]; then

    os="default"

    compiler="default"
else
    os="default"

    compiler="default"
fi

#--------------------------------------------------------------------------------------------------

libs="$external/lib"

if [ $qt = "qt4" ]; then

    Qt="$external/Qt/$Qt4_version"
else
    Qt="$external/Qt/$Qt5_version"
fi

if [ $os = "windows" -a $qt = "qt4" ]; then

    SSL="$external/OpenSSL/$SSL_versionA"
else
    SSL="$external/OpenSSL/$SSL_versionB"
fi

VLC="$external/VLC/$VLC_version"

libtorrent="$external/libtorrent/$libtorrent_version"

Boost="$external/Boost/$Boost_version"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

echo "CLEANING"

rm -rf deploy/*

touch deploy/.gitignore

if [ "$2" = "clean" ]; then

    exit 0
fi

echo ""

#--------------------------------------------------------------------------------------------------
# Qt
#--------------------------------------------------------------------------------------------------

if [ $qt = "qt4" ]; then

    if [ $os = "windows" ]; then

        echo "COPYING Qt4"

        mkdir deploy/imageformats

        if [ $compiler = "mingw" ]; then

            cp "$MinGW"/libgcc_s_*-1.dll    deploy
            cp "$MinGW"/libstdc++-6.dll     deploy
            cp "$MinGW"/libwinpthread-1.dll deploy
        fi

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

    elif [ $1 = "linux" ]; then

        echo "COPYING Qt4"

        mkdir deploy/imageformats

        cp "$libs"/libpng16.so.16 deploy

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
else
    echo "COPYING Qt5"

    mkdir deploy/platforms
    mkdir deploy/imageformats
    mkdir deploy/QtQuick.2

    if [ $os = "windows" ]; then

        if [ $compiler = "mingw" ]; then

            cp "$MinGW"/libgcc_s_*-1.dll    deploy
            cp "$MinGW"/libstdc++-6.dll     deploy
            cp "$MinGW"/libwinpthread-1.dll deploy
        fi

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

        if [ -f "$Qt"/bin/Qt5QmlModels.dll ]; then

            cp "$Qt"/bin/Qt5QmlModels.dll       deploy
            cp "$Qt"/bin/Qt5QmlWorkerScript.dll deploy
        fi

        cp "$Qt"/plugins/platforms/qwindows.dll deploy/platforms

        cp "$Qt"/plugins/imageformats/qsvg.dll  deploy/imageformats
        cp "$Qt"/plugins/imageformats/qjpeg.dll deploy/imageformats

        cp "$Qt"/qml/QtQuick.2/qtquick2plugin.dll deploy/QtQuick.2
        cp "$Qt"/qml/QtQuick.2/qmldir             deploy/QtQuick.2

    elif [ $1 = "macOS" ]; then

        # FIXME Qt 5.14 macOS: We have to copy qt.conf to avoid a segfault.
        cp "$Qt"/bin/qt.conf deploy

        cp "$Qt"/lib/QtCore.framework/Versions/5/QtCore                 deploy/QtCore.dylib
        cp "$Qt"/lib/QtGui.framework/Versions/5/QtGui                   deploy/QtGui.dylib
        cp "$Qt"/lib/QtNetwork.framework/Versions/5/QtNetwork           deploy/QtNetwork.dylib
        cp "$Qt"/lib/QtOpenGL.framework/Versions/5/QtOpenGL             deploy/QtOpenGL.dylib
        cp "$Qt"/lib/QtQml.framework/Versions/5/QtQml                   deploy/QtQml.dylib
        cp "$Qt"/lib/QtQuick.framework/Versions/5/QtQuick               deploy/QtQuick.dylib
        cp "$Qt"/lib/QtSvg.framework/Versions/5/QtSvg                   deploy/QtSvg.dylib
        cp "$Qt"/lib/QtWidgets.framework/Versions/5/QtWidgets           deploy/QtWidgets.dylib
        cp "$Qt"/lib/QtXml.framework/Versions/5/QtXml                   deploy/QtXml.dylib
        cp "$Qt"/lib/QtXmlPatterns.framework/Versions/5/QtXmlPatterns   deploy/QtXmlPatterns.dylib
        cp "$Qt"/lib/QtDBus.framework/Versions/5/QtDBus                 deploy/QtDBus.dylib
        cp "$Qt"/lib/QtPrintSupport.framework/Versions/5/QtPrintSupport deploy/QtPrintSupport.dylib

        if [ -f "$Qt"/lib/QtQmlModels.framework/Versions/5/QtQmlModels ]; then

            cp "$Qt"/lib/QtQmlModels.framework/Versions/5/QtQmlModels deploy/QtQmlModels.dylib

            cp "$Qt"/lib/QtQmlWorkerScript.framework/Versions/5/QtQmlWorkerScript \
               deploy/QtQmlWorkerScript.dylib
        fi

        cp "$Qt"/plugins/platforms/libqcocoa.dylib deploy/platforms

        cp "$Qt"/plugins/imageformats/libqsvg.dylib  deploy/imageformats
        cp "$Qt"/plugins/imageformats/libqjpeg.dylib deploy/imageformats

        cp "$Qt"/qml/QtQuick.2/libqtquick2plugin.dylib deploy/QtQuick.2
        cp "$Qt"/qml/QtQuick.2/qmldir                  deploy/QtQuick.2

    elif [ $1 = "linux" ]; then

        mkdir deploy/xcbglintegrations

        cp "$libs"/libz.so.* deploy

        cp "$libs"/libicudata.so.* deploy
        cp "$libs"/libicui18n.so.* deploy
        cp "$libs"/libicuuc.so.*   deploy

        cp "$libs"/libdouble-conversion.so.* deploy
        cp "$libs"/libpng16.so.*             deploy
        cp "$libs"/libharfbuzz.so.*          deploy
        cp "$libs"/libxcb-xinerama.so.*      deploy

        # NOTE: Required for Ubuntu 20.04.
        if [ -f "$libs"/libpcre2-16.so.0 ]; then

            cp "$libs"/libpcre2-16.so.0 deploy
        fi

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

        if [ -f "$Qt"/lib/libQt5QmlModels.so.5 ]; then

            cp "$Qt"/lib/libQt5QmlModels.so.5       deploy
            cp "$Qt"/lib/libQt5QmlWorkerScript.so.5 deploy
        fi

        cp "$Qt"/plugins/platforms/libqxcb.so deploy/platforms

        cp "$Qt"/plugins/imageformats/libqsvg.so  deploy/imageformats
        cp "$Qt"/plugins/imageformats/libqjpeg.so deploy/imageformats

        cp "$Qt"/plugins/xcbglintegrations/libqxcb-egl-integration.so deploy/xcbglintegrations
        cp "$Qt"/plugins/xcbglintegrations/libqxcb-glx-integration.so deploy/xcbglintegrations

        cp "$Qt"/qml/QtQuick.2/libqtquick2plugin.so deploy/QtQuick.2
        cp "$Qt"/qml/QtQuick.2/qmldir               deploy/QtQuick.2

    elif [ $1 = "android" ]; then

        cp "$Qt"/lib/libQt5Core_*.so        deploy
        cp "$Qt"/lib/libQt5Gui_*.so         deploy
        cp "$Qt"/lib/libQt5Network_*.so     deploy
        cp "$Qt"/lib/libQt5OpenGL_*.so      deploy
        cp "$Qt"/lib/libQt5Qml_*.so         deploy
        cp "$Qt"/lib/libQt5Quick_*.so       deploy
        cp "$Qt"/lib/libQt5Svg_*.so         deploy
        cp "$Qt"/lib/libQt5Widgets_*.so     deploy
        cp "$Qt"/lib/libQt5Xml_*.so         deploy
        cp "$Qt"/lib/libQt5XmlPatterns_*.so deploy

        if [ -f "$Qt"/lib/libQt5QmlModels_armeabi-v7a.so ]; then

            cp "$Qt"/lib/libQt5QmlModels_*.so       deploy
            cp "$Qt"/lib/libQt5QmlWorkerScript_*.so deploy
        fi

        cp "$Qt"/plugins/platforms/lib*qtforandroid_*.so deploy/platforms

        cp "$Qt"/plugins/imageformats/lib*qsvg_*.so  deploy/imageformats
        cp "$Qt"/plugins/imageformats/lib*qjpeg_*.so deploy/imageformats

        cp "$Qt"/qml/QtQuick.2/lib*qtquick2plugin_*.so deploy/QtQuick.2
    fi
fi

#--------------------------------------------------------------------------------------------------
# SSL
#--------------------------------------------------------------------------------------------------

echo "COPYING SSL"

if [ $os = "windows" ]; then

    cp "$SSL"/*.dll deploy

elif [ $1 = "linux" ]; then

    cp "$SSL"/*.so* deploy
fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

echo "COPYING VLC"

if [ $os = "windows" ]; then

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

elif [ $1 = "macOS" ]; then

    mkdir -p deploy/plugins

    cp -r "$VLC"/plugins/*.dylib deploy/plugins

    cp "$VLC"/lib/libvlc.5.dylib     deploy/libvlc.dylib
    cp "$VLC"/lib/libvlccore.9.dylib deploy/libvlccore.dylib

# FIXME Linux: We can't seem to be able to enforce our VLC libraries on ArchLinux.
#elif [ $1 = "linux" ]; then

    #mkdir -p deploy/vlc/plugins

    #cp -r "$VLC"/plugins/access        deploy/vlc/plugins
    #cp -r "$VLC"/plugins/audio_filter  deploy/vlc/plugins
    #cp -r "$VLC"/plugins/audio_mixer   deploy/vlc/plugins
    #cp -r "$VLC"/plugins/audio_output  deploy/vlc/plugins
    #cp -r "$VLC"/plugins/codec         deploy/vlc/plugins
    #cp -r "$VLC"/plugins/control       deploy/vlc/plugins
    #cp -r "$VLC"/plugins/demux         deploy/vlc/plugins
    #cp -r "$VLC"/plugins/misc          deploy/vlc/plugins
    #cp -r "$VLC"/plugins/packetizer    deploy/vlc/plugins
    #cp -r "$VLC"/plugins/stream_filter deploy/vlc/plugins
    #cp -r "$VLC"/plugins/stream_out    deploy/vlc/plugins
    #cp -r "$VLC"/plugins/video_chroma  deploy/vlc/plugins
    #cp -r "$VLC"/plugins/video_filter  deploy/vlc/plugins
    #cp -r "$VLC"/plugins/video_output  deploy/vlc/plugins

    #cp "$VLC"/libvlc*.so* deploy
fi

#--------------------------------------------------------------------------------------------------
# libtorrent
#--------------------------------------------------------------------------------------------------

echo "COPYING libtorrent"

if [ $os = "windows" ]; then

    cp "$libtorrent"/*torrent-rasterbar.dll deploy

elif [ $1 = "macOS" ]; then

    cp "$libtorrent"/libtorrent-rasterbar.dylib deploy

elif [ $1 = "linux" ]; then

    cp "$libtorrent"/libtorrent-rasterbar*.so* deploy
fi

#--------------------------------------------------------------------------------------------------
# Boost
#--------------------------------------------------------------------------------------------------

if [ $1 = "macOS" ]; then

    echo "COPYING Boost"

    cp "$Boost"/libboost*.dylib deploy

elif [ $1 = "linux" ]; then

    echo "COPYING Boost"

    cp "$Boost"/libboost*.so* deploy
fi

#--------------------------------------------------------------------------------------------------
# Sky
#--------------------------------------------------------------------------------------------------

if [ "$2" != "tools" ]; then

    echo "COPYING Sky"

    if [ $os = "windows" ]; then

        cp lib/*SkCore.*    deploy
        cp lib/*SkGui.*     deploy
        cp lib/*SkMedia.*   deploy
        #cp lib/*SkWeb.*     deploy
        cp lib/*SkTorrent.* deploy
        cp lib/*SkBackend.* deploy

    elif [ $1 = "macOS" ]; then

        cp lib/libSkCore.dylib    deploy
        cp lib/libSkGui.dylib     deploy
        cp lib/libSkMedia.dylib   deploy
        #cp lib/libSkWeb.dylib     deploy
        cp lib/libSkTorrent.dylib deploy
        cp lib/libSkBackend.dylib deploy

    elif [ $1 = "linux" ]; then

        cp lib/libSkCore.so    deploy
        cp lib/libSkGui.so     deploy
        cp lib/libSkMedia.so   deploy
        #cp lib/libSkWeb.so     deploy
        cp lib/libSkTorrent.so deploy
        cp lib/libSkBackend.so deploy

    elif [ $1 = "android" ]; then

        cp lib/libSkCore_*.so    deploy
        cp lib/libSkGui_*.so     deploy
        cp lib/libSkMedia_*.so   deploy
        #cp lib/libSkWeb_*.so     deploy
        cp lib/libSkTorrent_*.so deploy
        cp lib/libSkBackend_*.so deploy
    fi
fi

#--------------------------------------------------------------------------------------------------
# tools
#--------------------------------------------------------------------------------------------------

echo "COPYING tools"

if [ $os = "windows" ]; then

    cp bin/includeGenerator.exe deploy
    cp bin/deployer.exe         deploy
else
    cp bin/includeGenerator deploy
    cp bin/deployer         deploy
fi
