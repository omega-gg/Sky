#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

target="sky"

Sky="$PWD/../.."

backend="$PWD/../../../backend"

#--------------------------------------------------------------------------------------------------
# environment

compiler_win="mingw"

qt="qt6"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 1 -a $# != 2 ] \
   || \
   [ $1 != "win32" -a $1 != "win64" -a $1 != "macOS" -a $1 != "iOS" -a $1 != "linux" -a \
     $1 != "android" ] \
   || \
   [ $# = 2 -a "$2" != "clean" ]; then

    echo "Usage: deploy <win32 | win64 | macOS | iOS | linux | android> [clean]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $1 = "win32" -o $1 = "win64" ]; then

    os="windows"

    compiler="$compiler_win"
else
    if [ $1 = "iOS" -o $1 = "android" ]; then

        os="mobile"
    else
        os="default"
    fi

    compiler="default"
fi

if [ $qt = "qt5" ]; then

    QtX="Qt5"

    qx="5"

elif [ $qt = "qt6" ]; then

    QtX="Qt6"

    if [ $1 = "macOS" ]; then

        qx="A"
    else
        qx="6"
    fi
fi

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
# Bundle
#--------------------------------------------------------------------------------------------------

if [ $1 = "macOS" ]; then

    cp -r bin/$target.app deploy

    deploy="deploy/$target.app/Contents/MacOS"
else
    deploy="deploy"
fi

#--------------------------------------------------------------------------------------------------
# sky
#--------------------------------------------------------------------------------------------------

echo "COPYING $target"

if [ $os = "windows" ]; then

    cp bin/$target.exe deploy

elif [ $1 = "macOS" ]; then

    cd $deploy

    #----------------------------------------------------------------------------------------------
    # target

    install_name_tool -change @rpath/QtCore.framework/Versions/$qx/QtCore \
                              @loader_path/QtCore.dylib $target

    install_name_tool -change @rpath/QtGui.framework/Versions/$qx/QtGui \
                              @loader_path/QtGui.dylib $target

    install_name_tool -change @rpath/QtNetwork.framework/Versions/$qx/QtNetwork \
                              @loader_path/QtNetwork.dylib $target

    install_name_tool -change @rpath/QtOpenGL.framework/Versions/$qx/QtOpenGL \
                              @loader_path/QtOpenGL.dylib $target

    install_name_tool -change @rpath/QtQml.framework/Versions/$qx/QtQml \
                              @loader_path/QtQml.dylib $target

    if [ -f QtQmlModels.dylib ]; then

        install_name_tool -change @rpath/QtQmlModels.framework/Versions/$qx/QtQmlModels \
                                  @loader_path/QtQmlModels.dylib $target
    fi

    install_name_tool -change @rpath/QtQuick.framework/Versions/$qx/QtQuick \
                              @loader_path/QtQuick.dylib $target

    install_name_tool -change @rpath/QtSvg.framework/Versions/$qx/QtSvg \
                              @loader_path/QtSvg.dylib $target

    install_name_tool -change @rpath/QtWidgets.framework/Versions/$qx/QtWidgets \
                              @loader_path/QtWidgets.dylib $target

    install_name_tool -change @rpath/QtXml.framework/Versions/$qx/QtXml \
                              @loader_path/QtXml.dylib $target

    install_name_tool -change @rpath/QtMultimedia.framework/Versions/$qx/QtMultimedia \
                              @loader_path/QtMultimedia.dylib $target

    if [ $qt = "qt5" ]; then

        install_name_tool -change @rpath/QtXmlPatterns.framework/Versions/$qx/QtXmlPatterns \
                                  @loader_path/QtXmlPatterns.dylib $target
    else
        install_name_tool -change @rpath/QtCore5Compat.framework/Versions/$qx/QtCore5Compat \
                                  @loader_path/QtCore5Compat.dylib $target

        install_name_tool -change @rpath/QtQmlWorkerScript.framework/Versions/$qx/QtQmlWorkerScript \
                                  @loader_path/QtQmlWorkerScript.dylib $target

        install_name_tool -change @rpath/QtQmlMeta.framework/Versions/$qx/QtQmlMeta \
                                  @loader_path/QtQmlMeta.dylib $target
    fi

    otool -L $target

    #----------------------------------------------------------------------------------------------
    # QtGui

    if [ $qt = "qt6" ]; then

        install_name_tool -change @rpath/QtDBus.framework/Versions/$qx/QtDBus \
                                  @loader_path/QtDBus.dylib QtGui.dylib
    fi

    otool -L QtGui.dylib

    #----------------------------------------------------------------------------------------------
    # platforms

    if [ $qt = "qt5" ]; then

        install_name_tool -change @rpath/QtDBus.framework/Versions/$qx/QtDBus \
                                  @loader_path/../QtDBus.dylib platforms/libqcocoa.dylib

        install_name_tool -change @rpath/QtPrintSupport.framework/Versions/$qx/QtPrintSupport \
                                  @loader_path/../QtPrintSupport.dylib platforms/libqcocoa.dylib
    fi

    otool -L platforms/libqcocoa.dylib

    #----------------------------------------------------------------------------------------------
    # QtQml

    #if [ $qt = "qt6" ]; then
    #
    #    install_name_tool -change \
    #                      @rpath/QtQmlWorkerScript.framework/Versions/$qx/QtQmlWorkerScript \
    #                      @loader_path/../../QtQmlWorkerScript.dylib \
    #                      QtQml/WorkerScript/libworkerscriptplugin.dylib
    #
    #    otool -L QtQml/WorkerScript/libworkerscriptplugin.dylib
    #fi

    #----------------------------------------------------------------------------------------------
    # QtQuick

    if [ $qt = "qt5" ]; then

        if [ -f QtQmlModels.dylib ]; then

            install_name_tool -change \
                              @rpath/QtQmlWorkerScript.framework/Versions/$qx/QtQmlWorkerScript \
                              @loader_path/../QtQmlWorkerScript.dylib \
                              $QtQuick/libqtquick2plugin.dylib
        fi
    fi

    otool -L $QtQuick/libqtquick2plugin.dylib

    #----------------------------------------------------------------------------------------------
    # QtMultimedia

    if [ $qt = "qt5" ]; then

        libmultimedia="libdeclarative_multimedia"
    else
        libmultimedia="libquickmultimediaplugin"
    fi

    install_name_tool -change \
                      @rpath/QtMultimedia.framework/Versions/$qx/QtMultimedia \
                      @loader_path/../QtMultimedia.dylib \
                      QtMultimedia/$libmultimedia.dylib

    install_name_tool -change \
                      @rpath/QtMultimediaQuick.framework/Versions/$qx/QtMultimediaQuick \
                      @loader_path/../QtMultimediaQuick.dylib \
                      QtMultimedia/$libmultimedia.dylib

    otool -L QtMultimedia/$libmultimedia.dylib

    #----------------------------------------------------------------------------------------------
    # VLC

    install_name_tool -change @rpath/libvlccore.dylib \
                              @loader_path/libvlccore.dylib libvlc.dylib

    otool -L libvlc.dylib

    #----------------------------------------------------------------------------------------------
    # libtorrent

    install_name_tool -change libboost_system.dylib \
                              @loader_path/libboost_system.dylib libtorrent-rasterbar.dylib

    otool -L libtorrent-rasterbar.dylib

    #----------------------------------------------------------------------------------------------

    cd -

elif [ $1 = "iOS" ]; then

    cp -r bin/$target.app $deploy

elif [ $1 = "linux" ]; then

    cp bin/$target $deploy

    # NOTE: This script is useful for compatibilty. It enforces the application path for libraries.
    cp dist/script/start.sh $deploy

    chmod +x $deploy/start.sh

elif [ $1 = "android" ]; then

    cp bin/lib$target* $deploy
fi

#--------------------------------------------------------------------------------------------------
# backend
#--------------------------------------------------------------------------------------------------

if [ $os != "mobile" ]; then

    echo "COPYING backend"

    mkdir -p $deploy/backend/cover

    cp "$backend"/cover/* $deploy/backend/cover

    cp "$backend"/*.vbml $deploy/backend
fi

#--------------------------------------------------------------------------------------------------
# Sky
#--------------------------------------------------------------------------------------------------

echo "DEPLOYING to Sky"
echo "----------------"

cp -r deploy/* "$Sky"/deploy

echo "----------------"
