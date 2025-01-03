#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.15.2"
Qt6_version="6.8.1"

SSL_versionA="1.0.2u"
SSL_versionB="1.1.1s"

VLC_version="3.0.21"

libtorrent_version="2.0.10"

Boost_version="1.86.0"

#--------------------------------------------------------------------------------------------------
# Windows

MinGW_version="13.1.0"

#--------------------------------------------------------------------------------------------------
# environment

compiler_win="mingw"

qt="qt5"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

copyAndroid()
{
    cp "$1/lib/lib$QtX"Core_*.so            deploy
    cp "$1/lib/lib$QtX"Gui_*.so             deploy
    cp "$1/lib/lib$QtX"Network_*.so         deploy
    cp "$1/lib/lib$QtX"OpenGL_*.so          deploy
    cp "$1/lib/lib$QtX"Qml_*.so             deploy
    cp "$1/lib/lib$QtX"Quick_*.so           deploy
    cp "$1/lib/lib$QtX"Svg_*.so             deploy
    cp "$1/lib/lib$QtX"Widgets_*.so         deploy
    cp "$1/lib/lib$QtX"Xml_*.so             deploy
    cp "$1/lib/lib$QtX"Multimedia_*.so      deploy
    cp "$1/lib/lib$QtX"MultimediaQuick_*.so deploy

    if [ $qt = "qt5" ]; then

        cp "$1/lib/lib$QtX"XmlPatterns_*.so deploy
    else
        cp "$1/lib/lib$QtX"Core5Compat_*.so deploy
        cp "$1/lib/lib$QtX"QmlMeta_*.so   deploy
    fi

    if [ -f "$1/lib/lib$QtX"QmlModels_"$2".so ]; then

        cp "$1/lib/lib$QtX"QmlModels_*.so       deploy
        cp "$1/lib/lib$QtX"QmlWorkerScript_*.so deploy
    fi

    cp "$1"/plugins/platforms/lib*qtforandroid_*.so deploy/platforms

    cp "$1"/plugins/imageformats/lib*qsvg_*.so  deploy/imageformats
    cp "$1"/plugins/imageformats/lib*qjpeg_*.so deploy/imageformats
    cp "$1"/plugins/imageformats/lib*qwebp_*.so deploy/imageformats

    if [ $qt = "qt5" ]; then

        cp "$1"/plugins/mediaservice/lib*qtmedia_*.so deploy/mediaservice
    else
        cp "$1"/plugins/tls/lib*qopensslbackend_*.so deploy/tls
    fi

    cp "$1"/qml/$QtQuick/lib*qtquick2plugin_*.so deploy/$QtQuick

    cp "$1"/qml/QtMultimedia/lib*multimedia*.so deploy/$QtQuick

    if [ $qt = "qt6" ]; then

        cp "$1"/qml/QtQml/WorkerScript/lib*workerscriptplugin_*.so deploy/QtQml/WorkerScript
    fi
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 1 -a $# != 2 ] \
   || \
   [ $1 != "win32" -a $1 != "win64" -a $1 != "macOS" -a $1 != "iOS" -a $1 != "linux" -a \
     $1 != "android" ] \
   || \
   [ $# = 2 -a "$2" != "tools" -a "$2" != "clean" ]; then

    echo "Usage: deploy <win32 | win64 | macOS | iOS | linux | android> [tools | clean]"

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
else
    if [ $1 = "iOS" -o $1 = "android" ]; then

        os="mobile"
    else
        os="default"
    fi

    compiler="default"
fi

#--------------------------------------------------------------------------------------------------

libs="$external/lib"

if [ $qt = "qt4" ]; then

    Qt="$external/Qt/$Qt4_version"

elif [ $qt = "qt5" ]; then

    Qt="$external/Qt/$Qt5_version"

    QtX="Qt5"

    qx="5"
else
    Qt="$external/Qt/$Qt6_version"

    if [ $1 = "iOS" ]; then

        Qt="$Qt/ios"
    fi

    QtX="Qt6"

    if [ $1 = "macOS" ]; then

        qx="A"
    else
        qx="6"
    fi
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

if [ $qt = "qt5" ]; then

    QtQuick="QtQuick.2"

elif [ $qt = "qt6" ]; then

    QtQuick="QtQuick"
fi

if [ $qt = "qt4" ]; then

    if [ $os = "windows" ]; then

        echo "COPYING Qt"

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
        cp "$Qt"/bin/QtWebKit4.dll      deploy

        cp "$Qt"/plugins/imageformats/qsvg4.dll  deploy/imageformats
        cp "$Qt"/plugins/imageformats/qjpeg4.dll deploy/imageformats

    elif [ $1 = "linux" ]; then

        echo "COPYING Qt"

        mkdir deploy/imageformats

        #cp "$libs"/libpng16.so.16 deploy

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
    echo "COPYING Qt"

    mkdir deploy/platforms
    mkdir deploy/imageformats
    mkdir deploy/$QtQuick
    mkdir deploy/QtMultimedia

    if [ $qt = "qt5" ]; then

        mkdir -p deploy/mediaservice
    else
        mkdir -p deploy/tls

        mkdir -p deploy/QtQml/WorkerScript
    fi

    if [ $os = "windows" ]; then

        if [ $compiler = "mingw" ]; then

            cp "$MinGW"/libgcc_s_*-1.dll    deploy
            cp "$MinGW"/libstdc++-6.dll     deploy
            cp "$MinGW"/libwinpthread-1.dll deploy
        fi

        if [ $qt = "qt5" ]; then

            cp "$Qt"/bin/libEGL.dll    deploy
            cp "$Qt"/bin/libGLESv2.dll deploy
        fi

        cp "$Qt/bin/$QtX"Core.dll            deploy
        cp "$Qt/bin/$QtX"Gui.dll             deploy
        cp "$Qt/bin/$QtX"Network.dll         deploy
        cp "$Qt/bin/$QtX"OpenGL.dll          deploy
        cp "$Qt/bin/$QtX"Qml.dll             deploy
        cp "$Qt/bin/$QtX"Quick.dll           deploy
        cp "$Qt/bin/$QtX"Svg.dll             deploy
        cp "$Qt/bin/$QtX"Widgets.dll         deploy
        cp "$Qt/bin/$QtX"Xml.dll             deploy
        cp "$Qt/bin/$QtX"Multimedia.dll      deploy
        cp "$Qt/bin/$QtX"MultimediaQuick.dll deploy

        if [ $qt = "qt5" ]; then

            cp "$Qt/bin/$QtX"XmlPatterns.dll deploy
            cp "$Qt/bin/$QtX"WinExtras.dll   deploy
        else
            cp "$Qt/bin/$QtX"Core5Compat.dll deploy
            cp "$Qt/bin/$QtX"QmlMeta.dll deploy
        fi

        if [ -f "$Qt/bin/$QtX"QmlModels.dll ]; then

            cp "$Qt/bin/$QtX"QmlModels.dll       deploy
            cp "$Qt/bin/$QtX"QmlWorkerScript.dll deploy
        fi

        cp "$Qt"/plugins/platforms/qwindows.dll deploy/platforms

        cp "$Qt"/plugins/imageformats/qsvg.dll  deploy/imageformats
        cp "$Qt"/plugins/imageformats/qjpeg.dll deploy/imageformats
        cp "$Qt"/plugins/imageformats/qwebp.dll deploy/imageformats

        if [ $qt = "qt5" ]; then

            cp "$Qt"/plugins/mediaservice/dsengine.dll deploy/mediaservice
        else
            cp "$Qt"/plugins/tls/qopensslbackend.dll deploy/tls
        fi

        cp "$Qt"/qml/$QtQuick/qtquick2plugin.dll deploy/$QtQuick
        cp "$Qt"/qml/$QtQuick/qmldir             deploy/$QtQuick

        cp "$Qt"/qml/QtMultimedia/*multimedia*.dll deploy/QtMultimedia
        cp "$Qt"/qml/QtMultimedia/qmldir           deploy/QtMultimedia

        if [ $qt = "qt6" ]; then

            cp "$Qt"/qml/QtQml/WorkerScript/workerscriptplugin.dll deploy/QtQml/WorkerScript
            cp "$Qt"/qml/QtQml/WorkerScript/qmldir                 deploy/QtQml/WorkerScript
        fi

    elif [ $1 = "macOS" ]; then

        # FIXME Qt 5.14 macOS: We have to copy qt.conf to avoid a segfault.
        cp "$Qt"/bin/qt.conf deploy

        cp "$Qt"/lib/QtCore.framework/Versions/$qx/QtCore                       deploy/QtCore.dylib
        cp "$Qt"/lib/QtGui.framework/Versions/$qx/QtGui                         deploy/QtGui.dylib
        cp "$Qt"/lib/QtNetwork.framework/Versions/$qx/QtNetwork                 deploy/QtNetwork.dylib
        cp "$Qt"/lib/QtOpenGL.framework/Versions/$qx/QtOpenGL                   deploy/QtOpenGL.dylib
        cp "$Qt"/lib/QtQml.framework/Versions/$qx/QtQml                         deploy/QtQml.dylib
        cp "$Qt"/lib/QtQuick.framework/Versions/$qx/QtQuick                     deploy/QtQuick.dylib
        cp "$Qt"/lib/QtSvg.framework/Versions/$qx/QtSvg                         deploy/QtSvg.dylib
        cp "$Qt"/lib/QtWidgets.framework/Versions/$qx/QtWidgets                 deploy/QtWidgets.dylib
        cp "$Qt"/lib/QtXml.framework/Versions/$qx/QtXml                         deploy/QtXml.dylib
        cp "$Qt"/lib/QtMultimedia.framework/Versions/$qx/QtMultimedia           deploy/QtMultimedia.dylib
        cp "$Qt"/lib/QtMultimediaQuick.framework/Versions/$qx/QtMultimediaQuick deploy/QtMultimediaQuick.dylib
        cp "$Qt"/lib/QtDBus.framework/Versions/$qx/QtDBus                       deploy/QtDBus.dylib
        cp "$Qt"/lib/QtPrintSupport.framework/Versions/$qx/QtPrintSupport       deploy/QtPrintSupport.dylib

        if [ $qt = "qt5" ]; then

            cp "$Qt"/lib/QtXmlPatterns.framework/Versions/$qx/QtXmlPatterns \
                deploy/QtXmlPatterns.dylib
        else
            cp "$Qt"/lib/QtCore5Compat.framework/Versions/$qx/QtCore5Compat \
                deploy/QtCore5Compat.dylib

            cp "$Qt"/lib/QtQmlMeta.framework/Versions/$qx/QtQmlMeta deploy/QtQmlMeta.dylib
        fi

        if [ -f "$Qt"/lib/QtQmlModels.framework/Versions/$qx/QtQmlModels ]; then

            cp "$Qt"/lib/QtQmlModels.framework/Versions/$qx/QtQmlModels deploy/QtQmlModels.dylib

            cp "$Qt"/lib/QtQmlWorkerScript.framework/Versions/$qx/QtQmlWorkerScript \
               deploy/QtQmlWorkerScript.dylib
        fi

        cp "$Qt"/plugins/platforms/libqcocoa.dylib deploy/platforms

        cp "$Qt"/plugins/imageformats/libqsvg.dylib  deploy/imageformats
        cp "$Qt"/plugins/imageformats/libqjpeg.dylib deploy/imageformats
        cp "$Qt"/plugins/imageformats/libqwebp.dylib deploy/imageformats

        if [ $qt = "qt5" ]; then

            cp "$Qt"/plugins/mediaservice/libqavfcamera.dylib deploy/mediaservice
        else
            cp "$Qt"/plugins/tls/libqopensslbackend.dylib deploy/tls
        fi

        cp "$Qt"/qml/$QtQuick/libqtquick2plugin.dylib deploy/$QtQuick
        cp "$Qt"/qml/$QtQuick/qmldir                  deploy/$QtQuick

        cp "$Qt"/qml/QtMultimedia/lib*multimedia*.dylib deploy/QtMultimedia
        cp "$Qt"/qml/QtMultimedia/qmldir                deploy/QtMultimedia

        if [ $qt = "qt6" ]; then

            cp "$Qt"/qml/QtQml/WorkerScript/libworkerscriptplugin.dylib deploy/QtQml/WorkerScript
            cp "$Qt"/qml/QtQml/WorkerScript/qmldir                      deploy/QtQml/WorkerScript
        fi

    elif [ $1 = "iOS" ]; then

        cp "$Qt/lib/lib$QtX"Core.a            deploy
        cp "$Qt/lib/lib$QtX"Gui.a             deploy
        cp "$Qt/lib/lib$QtX"Network.a         deploy
        cp "$Qt/lib/lib$QtX"OpenGL.a          deploy
        cp "$Qt/lib/lib$QtX"Qml.a             deploy
        cp "$Qt/lib/lib$QtX"Quick.a           deploy
        cp "$Qt/lib/lib$QtX"Svg.a             deploy
        cp "$Qt/lib/lib$QtX"Widgets.a         deploy
        cp "$Qt/lib/lib$QtX"Xml.a             deploy
        cp "$Qt/lib/lib$QtX"Multimedia.a      deploy
        cp "$Qt/lib/lib$QtX"MultimediaQuick.a deploy

        if [ $qt = "qt5" ]; then

            cp "$Qt/lib/lib$QtX"XmlPatterns.a deploy
        else
            cp "$Qt/lib/lib$QtX"Core5Compat.a deploy
            cp "$Qt/lib/lib$QtX"QmlMeta.a deploy
        fi

        if [ -f "$Qt/lib/lib$QtX"QmlModels.a ]; then

            cp "$Qt/lib/lib$QtX"QmlModels.a       deploy
            cp "$Qt/lib/lib$QtX"QmlWorkerScript.a deploy
        fi

        cp "$Qt"/plugins/platforms/libqios.a deploy/platforms

        cp "$Qt"/plugins/imageformats/libqsvg.a  deploy/imageformats
        cp "$Qt"/plugins/imageformats/libqjpeg.a deploy/imageformats
        cp "$Qt"/plugins/imageformats/libqwebp.a deploy/imageformats

        if [ $qt = "qt5" ]; then

            cp "$Qt"/plugins/mediaservice/libqavfcamera.a deploy/mediaservice
        else
            cp "$Qt"/plugins/tls/libqsecuretransportbackend.a deploy/tls
        fi

        cp "$Qt"/qml/$QtQuick/libqtquick2plugin.a deploy/$QtQuick
        cp "$Qt"/qml/$QtQuick/qmldir              deploy/$QtQuick

        cp "$Qt"/qml/QtMultimedia/lib*multimedia*.a deploy/QtMultimedia
        cp "$Qt"/qml/QtMultimedia/qmldir            deploy/QtMultimedia

        if [ $qt = "qt6" ]; then

            cp "$Qt"/qml/QtQml/WorkerScript/libworkerscriptplugin.a deploy/QtQml/WorkerScript
            cp "$Qt"/qml/QtQml/WorkerScript/qmldir                  deploy/QtQml/WorkerScript
        fi

    elif [ $1 = "linux" ]; then

        mkdir deploy/xcbglintegrations

        #cp "$libs"/libz.so.* deploy

        #cp "$libs"/libdouble-conversion.so.* deploy
        #cp "$libs"/libpng16.so.*             deploy
        #cp "$libs"/libharfbuzz.so.*          deploy
        #cp "$libs"/libxcb-xinerama.so.*      deploy

        # NOTE: Required for Ubuntu 20.04.
        #if [ -f "$libs"/libpcre2-16.so.0 ]; then

            #cp "$libs"/libpcre2-16.so.0 deploy
        #fi

        cp "$Qt"/lib/libicudata.so.* deploy
        cp "$Qt"/lib/libicui18n.so.* deploy
        cp "$Qt"/lib/libicuuc.so.*   deploy

        # NOTE: We only want one occurence of each library.
        if [ -f deploy/libicudata.so.*.* ]; then

            rm deploy/libicudata.so.*.*
            rm deploy/libicui18n.so.*.*
            rm deploy/libicuuc.so.*.*
        fi

        cp "$Qt/lib/lib$QtX"Core.so.$qx              deploy
        cp "$Qt/lib/lib$QtX"Gui.so.$qx               deploy
        cp "$Qt/lib/lib$QtX"Network.so.$qx           deploy
        cp "$Qt/lib/lib$QtX"OpenGL.so.$qx            deploy
        cp "$Qt/lib/lib$QtX"Qml.so.$qx               deploy
        cp "$Qt/lib/lib$QtX"Quick.so.$qx             deploy
        cp "$Qt/lib/lib$QtX"Svg.so.$qx               deploy
        cp "$Qt/lib/lib$QtX"Widgets.so.$qx           deploy
        cp "$Qt/lib/lib$QtX"Xml.so.$qx               deploy
        cp "$Qt/lib/lib$QtX"Multimedia.so.$qx        deploy
        cp "$Qt/lib/lib$QtX"MultimediaQuick.so.$qx   deploy
        cp "$Qt/lib/lib$QtX"XcbQpa.so.$qx            deploy
        cp "$Qt/lib/lib$QtX"DBus.so.$qx              deploy

        if [ $qt = "qt5" ]; then

            cp "$Qt/lib/lib$QtX"XmlPatterns.so.$qx deploy
        else
            cp "$Qt/lib/lib$QtX"Core5Compat.so.$qx deploy
            cp "$Qt/lib/lib$QtX"QmlMeta.so.$qx     deploy
        fi

        if [ -f "$Qt/lib/lib$QtX"QmlModels.so.$qx ]; then

            cp "$Qt/lib/lib$QtX"QmlModels.so.$qx       deploy
            cp "$Qt/lib/lib$QtX"QmlWorkerScript.so.$qx deploy
        fi

        cp "$Qt"/plugins/platforms/libqxcb.so deploy/platforms

        cp "$Qt"/plugins/imageformats/libqsvg.so  deploy/imageformats
        cp "$Qt"/plugins/imageformats/libqjpeg.so deploy/imageformats

        if [ -f "$Qt"/plugins/imageformats/libqwebp.so ]; then

            cp "$Qt"/plugins/imageformats/libqwebp.so deploy/imageformats
        fi

        if [ $qt = "qt5" ]; then

            cp "$Qt"/plugins/mediaservice/libgstcamerabin.so deploy/mediaservice
        else
            cp "$Qt"/plugins/tls/libqopensslbackend.so deploy/tls
        fi

        cp "$Qt"/plugins/xcbglintegrations/libqxcb-egl-integration.so deploy/xcbglintegrations
        cp "$Qt"/plugins/xcbglintegrations/libqxcb-glx-integration.so deploy/xcbglintegrations

        cp "$Qt"/qml/$QtQuick/libqtquick2plugin.so deploy/$QtQuick
        cp "$Qt"/qml/$QtQuick/qmldir               deploy/$QtQuick

        cp "$Qt"/qml/QtMultimedia/lib*multimedia*.so deploy/QtMultimedia
        cp "$Qt"/qml/QtMultimedia/qmldir             deploy/QtMultimedia

        if [ $qt = "qt6" ]; then

            cp "$Qt"/qml/QtQml/WorkerScript/libworkerscriptplugin.so deploy/QtQml/WorkerScript
            cp "$Qt"/qml/QtQml/WorkerScript/qmldir                   deploy/QtQml/WorkerScript
        fi

    elif [ $1 = "android" ]; then

        if [ $qt = "qt5" ]; then

            copyAndroid "$Qt" armeabi-v7a
        else
            copyAndroid "$Qt"/android_armv7     armeabi-v7a
            copyAndroid "$Qt"/android_arm64_v8a arm64-v8a
            copyAndroid "$Qt"/android_x86       x86
            copyAndroid "$Qt"/android_x86_64    x86_64
        fi
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

    cp -r "$VLC"/plugins/access             deploy/plugins
    cp -r "$VLC"/plugins/audio_filter       deploy/plugins
    cp -r "$VLC"/plugins/audio_mixer        deploy/plugins
    cp -r "$VLC"/plugins/audio_output       deploy/plugins
    cp -r "$VLC"/plugins/codec              deploy/plugins
    cp -r "$VLC"/plugins/control            deploy/plugins
    cp -r "$VLC"/plugins/demux              deploy/plugins
    cp -r "$VLC"/plugins/misc               deploy/plugins
    cp -r "$VLC"/plugins/packetizer         deploy/plugins
    cp -r "$VLC"/plugins/services_discovery deploy/plugins
    cp -r "$VLC"/plugins/stream_filter      deploy/plugins
    cp -r "$VLC"/plugins/stream_out         deploy/plugins
    cp -r "$VLC"/plugins/video_chroma       deploy/plugins
    cp -r "$VLC"/plugins/video_filter       deploy/plugins
    cp -r "$VLC"/plugins/video_output       deploy/plugins

    cp "$VLC"/libvlc*.dll deploy

elif [ $1 = "macOS" ]; then

    mkdir -p deploy/plugins

    cp -r "$VLC"/plugins/*.dylib deploy/plugins

    cp "$VLC"/lib/libvlc.5.dylib     deploy/libvlc.dylib
    cp "$VLC"/lib/libvlccore.9.dylib deploy/libvlccore.dylib

elif [ $1 = "linux" ]; then

    mkdir -p deploy/vlc/plugins

    cp -r "$VLC"/vlc/plugins/access             deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/audio_filter       deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/audio_mixer        deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/audio_output       deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/codec              deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/control            deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/demux              deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/misc               deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/packetizer         deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/services_discovery deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/stream_filter      deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/stream_out         deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/video_chroma       deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/video_filter       deploy/vlc/plugins
    cp -r "$VLC"/vlc/plugins/video_output       deploy/vlc/plugins

    cp -r "$VLC"/vlc/lib*.so* deploy/vlc

    cp "$VLC"/libvlc*.so* deploy

    #----------------------------------------------------------------------------------------------
    # NOTE: Patching VLC libraries rpath for standalone packages.

    if [ -x "$(command -v patchelf)" ]; then

        find deploy -maxdepth 1 -name libvlc*.so* -exec patchelf --set-rpath '$ORIGIN' {} \;

        find deploy/vlc -maxdepth 1 -name lib*.so* -exec patchelf --set-rpath '$ORIGIN/../' {} \;

        find deploy/vlc/plugins -name lib*.so* -exec patchelf --set-rpath \
                                '$ORIGIN/../../:$ORIGIN/../../../' {} \;
    else
        echo "patchelf is not installed"
    fi
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

    # NOTE: We make sure the deployed library will be resolved by the binary.
    cp "$libtorrent"/libtorrent-rasterbar.so deploy/libtorrent-rasterbar.so.$libtorrent_version
fi

#--------------------------------------------------------------------------------------------------
# Boost
#--------------------------------------------------------------------------------------------------

if [ $1 = "macOS" ]; then

    echo "COPYING Boost"

    cp "$Boost"/libboost*.dylib deploy

elif [ $1 = "linux" ]; then

    echo "COPYING Boost"

    # NOTE: We make sure the deployed library will be resolved by the binary.
    cp "$Boost"/libboost_system.so deploy/libboost_system.so.$Boost_version
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

    elif [ $1 = "iOS" ]; then

        cp lib/libSkCore.a    deploy
        cp lib/libSkGui.a     deploy
        cp lib/libSkMedia.a   deploy
        #cp lib/libSkWeb.a     deploy
        cp lib/libSkBackend.a deploy

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
    cp bin/projectGenerator.exe deploy
    cp bin/imageConverter.exe   deploy

    if [ -f bin/androiddeployqt.exe ]; then

        cp bin/androiddeployqt.exe deploy
    fi

elif [ $os = "mobile" ]; then

    cp -r "$external"/Sky/* deploy
else
    cp bin/includeGenerator deploy
    cp bin/deployer         deploy
    cp bin/projectGenerator deploy
    cp bin/imageConverter   deploy

    if [ -f bin/androiddeployqt ]; then

        cp bin/androiddeployqt deploy
    fi
fi

if [ $1 = "macOS" ]; then

    #----------------------------------------------------------------------------------------------
    # includeGenerator

    install_name_tool -change @rpath/QtCore.framework/Versions/$qx/QtCore \
                              @loader_path/QtCore.dylib deploy/includeGenerator

    #----------------------------------------------------------------------------------------------
    # deployer

    install_name_tool -change @rpath/QtCore.framework/Versions/$qx/QtCore \
                              @loader_path/QtCore.dylib deploy/deployer

    #----------------------------------------------------------------------------------------------
    # projectGenerator

    install_name_tool -change @rpath/QtCore.framework/Versions/$qx/QtCore \
                              @loader_path/QtCore.dylib deploy/projectGenerator

    #----------------------------------------------------------------------------------------------
    # imageConverter

    install_name_tool -change @rpath/QtCore.framework/Versions/$qx/QtCore \
                              @loader_path/QtCore.dylib deploy/imageConverter

    install_name_tool -change @rpath/QtGui.framework/Versions/$qx/QtGui \
                              @loader_path/QtGui.dylib deploy/imageConverter
fi

#--------------------------------------------------------------------------------------------------
# shaders
#--------------------------------------------------------------------------------------------------

if [ $qt = "qt6" ]; then

    echo "COPYING shaders"

    mkdir -p deploy/shaders

    cp dist/shaders/qsb/*.qsb deploy/shaders
fi
