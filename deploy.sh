#!/bin/sh

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

external="../3rdparty"

#--------------------------------------------------------------------------------------------------

Qt4_version="4.8.7"
Qt5_version="5.5.1"

MinGW_version="4.9.2"

VLC_version="2.2.4"

#--------------------------------------------------------------------------------------------------

Qt4="$external/Qt/$Qt4_version"
Qt5="$external/Qt/$Qt5_version"

MinGW="$external/MinGW/$MinGW_version/bin"

SSL="$external/OpenSSL"

VLC="$external/VLC/$VLC_version"

#--------------------------------------------------------------------------------------------------

bin4="bin"
bin5="latest"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 2 ] || [ $1 != "qt4" -a $1 != "qt5" -a $1 != "clean" ] || [ $2 != "win32" -a \
                                                                       $2 != "linux" ]; then

    echo "Usage: deploy <qt4 | qt5 | clean> <win32 | linux>"

    exit 1
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

        cp "$Qt4"/bin/QtCore4.dll        deploy
        cp "$Qt4"/bin/QtDeclarative4.dll deploy
        cp "$Qt4"/bin/QtGui4.dll         deploy
        cp "$Qt4"/bin/QtNetwork4.dll     deploy
        cp "$Qt4"/bin/QtOpenGL4.dll      deploy
        cp "$Qt4"/bin/QtScript4.dll      deploy
        cp "$Qt4"/bin/QtSql4.dll         deploy
        cp "$Qt4"/bin/QtSvg4.dll         deploy
        cp "$Qt4"/bin/QtWebKit4.dll      deploy
        cp "$Qt4"/bin/QtXml4.dll         deploy
        cp "$Qt4"/bin/QtXmlPatterns4.dll deploy

        cp "$Qt4"/plugins/imageformats/qsvg4.dll  deploy/imageformats
        cp "$Qt4"/plugins/imageformats/qjpeg4.dll deploy/imageformats
    fi

    bin="$bin4"

else

    echo "COPYING Qt5"

    mkdir deploy/imageformats
    mkdir deploy/platforms

    if [ $2 = "win32" ]; then

        cp "$MinGW"/libgcc_s_dw2-1.dll  deploy
        cp "$MinGW"/libstdc++-6.dll     deploy
        cp "$MinGW"/libwinpthread-1.dll deploy

        cp "$Qt5"/bin/icudt54.dll deploy
        cp "$Qt5"/bin/icuin54.dll deploy
        cp "$Qt5"/bin/icuuc54.dll deploy

        cp "$Qt5"/bin/Qt5Core.dll              deploy
        cp "$Qt5"/bin/Qt5Declarative.dll       deploy
        cp "$Qt5"/bin/Qt5Gui.dll               deploy
        cp "$Qt5"/bin/Qt5Multimedia.dll        deploy
        cp "$Qt5"/bin/Qt5MultimediaWidgets.dll deploy
        cp "$Qt5"/bin/Qt5Network.dll           deploy
        cp "$Qt5"/bin/Qt5OpenGL.dll            deploy
        cp "$Qt5"/bin/Qt5Positioning.dll       deploy
        cp "$Qt5"/bin/Qt5PrintSupport.dll      deploy
        cp "$Qt5"/bin/Qt5Qml.dll               deploy
        cp "$Qt5"/bin/Qt5Quick.dll             deploy
        cp "$Qt5"/bin/Qt5Script.dll            deploy
        cp "$Qt5"/bin/Qt5Sensors.dll           deploy
        cp "$Qt5"/bin/Qt5Sql.dll               deploy
        cp "$Qt5"/bin/Qt5Svg.dll               deploy
        cp "$Qt5"/bin/Qt5WebChannel.dll        deploy
        cp "$Qt5"/bin/Qt5WebKit.dll            deploy
        cp "$Qt5"/bin/Qt5WebKitWidgets.dll     deploy
        cp "$Qt5"/bin/Qt5Widgets.dll           deploy
        cp "$Qt5"/bin/Qt5Xml.dll               deploy
        cp "$Qt5"/bin/Qt5XmlPatterns.dll       deploy

        cp "$Qt5"/plugins/imageformats/qsvg.dll  deploy/imageformats
        cp "$Qt5"/plugins/imageformats/qjpeg.dll deploy/imageformats

        cp "$Qt5"/plugins/platforms/qwindows.dll deploy/platforms
    fi

    bin="$bin5"
fi

if [ $2 = "win32" ]; then

    cp "$SSL"/libeay32.dll deploy
    cp "$SSL"/ssleay32.dll deploy
fi

#--------------------------------------------------------------------------------------------------
# VLC
#--------------------------------------------------------------------------------------------------

echo "COPYING VLC"

mkdir deploy/plugins

cp -r "$VLC"/plugins/access       deploy/plugins
cp -r "$VLC"/plugins/audio_filter deploy/plugins
cp -r "$VLC"/plugins/audio_mixer  deploy/plugins
cp -r "$VLC"/plugins/audio_output deploy/plugins
cp -r "$VLC"/plugins/codec        deploy/plugins
cp -r "$VLC"/plugins/control      deploy/plugins
cp -r "$VLC"/plugins/demux        deploy/plugins
cp -r "$VLC"/plugins/misc         deploy/plugins
cp -r "$VLC"/plugins/video_chroma deploy/plugins
cp -r "$VLC"/plugins/video_filter deploy/plugins
cp -r "$VLC"/plugins/video_output deploy/plugins

if [ $2 = "win32" ]; then

    cp "$VLC"/libvlc*.dll deploy
fi

#--------------------------------------------------------------------------------------------------
# Sky
#--------------------------------------------------------------------------------------------------

echo "COPYING Sky"

if [ $2 = "win32" ]; then

    cp "$bin"/SkCore.dll    deploy
    cp "$bin"/SkGui.dll     deploy
    cp "$bin"/SkMedia.dll   deploy
    cp "$bin"/SkWeb.dll     deploy
    cp "$bin"/SkTorrent.dll deploy
    cp "$bin"/SkBackend.dll deploy
fi

cp "$bin"/includeGenerator* deploy
cp "$bin"/deployer*         deploy
