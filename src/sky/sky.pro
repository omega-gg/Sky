SK = $$_PRO_FILE_PWD_/../..

TARGET = sky

DESTDIR = $$_PRO_FILE_PWD_/bin

contains(QT_MAJOR_VERSION, 4) {
    QT += opengl declarative network script xml xmlpatterns svg

} else:contains(QT_MAJOR_VERSION, 5) {

    QT += opengl quick network xml xmlpatterns svg multimedia

    win32:QT += winextras

    unix:!macx:!ios:!android:QT += x11extras

    android:QT += androidextras
} else {
    QT += opengl quick network xml svg multimedia core5compat

    #----------------------------------------------------------------------------------------------
    # NOTE Qt6.3: We need the widgets for QApplication and QFileDialog(s).

    win32:QT += widgets

    macx:QT += widgets

    unix:!android:QT += widgets
}

greaterThan(QT_MAJOR_VERSION, 4) {
    unix:!macx:!ios:!android:QT += dbus
}

# C++17
contains(QT_MAJOR_VERSION, 4) {
    QMAKE_CXXFLAGS += -std=c++1z
} else {
    CONFIG += c++1z
}

DEFINES += QUAZIP_BUILD \
           SK_CORE_LIBRARY SK_GUI_LIBRARY SK_BARCODE_LIBRARY SK_BACKEND_LIBRARY SK_MEDIA_LIBRARY \
           SK_MULTIMEDIA_LIBRARY SK_TORRENT_LIBRARY \
           SK_CHARSET SK_BACKEND_LOCAL #SK_BACKEND_LOG

win32-msvc* {
    # libtorrent: This fixes the winsock2 and std::min errors.
    DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX

    # Boost: This prevents an issue with linking
    DEFINES += BOOST_ALL_NO_LIB
}

!win32-msvc*:!ios:!android:DEFINES += CAN_COMPILE_SSE2

#DEFINES += SK_SOFTWARE

contains(QT_MAJOR_VERSION, 4) {
    CONFIG(release, debug|release) {

        win32:DEFINES += SK_WIN_NATIVE
    }
} else {
    win32:DEFINES += SK_WIN_NATIVE
}

deploy|android {
    DEFINES += SK_DEPLOY

    RESOURCES = dist/qrc/sky.qrc
}

!win32-msvc*:!ios:!android:QMAKE_CXXFLAGS += -msse

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include($$SK/src/Sk.pri)
include(src/global/global.pri)
include(src/controllers/controllers.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkGui \
               $$SK/include/SkBarcode \
               $$SK/include/SkBackend \
               $$SK/include/SkMedia \
               $$SK/include/SkMultimedia \
               $$SK/include/SkTorrent \
               $$SK/include \
               src/controllers

unix:contains(QT_MAJOR_VERSION, 4) {
    INCLUDEPATH += $$SK/include/$$QTX \
                   $$SK/include/$$QTX/QtCore \
                   $$SK/include/$$QTX/QtGui \
                   $$SK/include/$$QTX/QtDeclarative
}

greaterThan(QT_MAJOR_VERSION, 4) {
    INCLUDEPATH += $$SK/include/$$QTX \
                   $$SK/include/$$QTX/QtCore \
                   $$SK/include/$$QTX/QtGui \
                   $$SK/include/$$QTX/QtQml \
                   $$SK/include/$$QTX/QtQuick
}

unix:!macx:!ios:!android:greaterThan(QT_MAJOR_VERSION, 4) {
    INCLUDEPATH += $$SK/include/$$QTX/QtDBus
}

CONFIG(debug, debug|release) {
    LIBS += -L$$SK/lib -lSkCoreD \
            -L$$SK/lib -lSkGuiD \
            -L$$SK/lib -lSkBarcodeD \
            -L$$SK/lib -lSkBackendD \
            -L$$SK/lib -lSkMediaD \
            -L$$SK/lib -lSkMultimediaD \
            -L$$SK/lib -lSkTorrentD
} else {
    LIBS += -L$$SK/lib -lSkCore \
            -L$$SK/lib -lSkGui \
            -L$$SK/lib -lSkBarcode \
            -L$$SK/lib -lSkBackend \
            -L$$SK/lib -lSkMedia \
            -L$$SK/lib -lSkMultimedia \
            -L$$SK/lib -lSkTorrent
}

#win32:contains(QT_MAJOR_VERSION, 5) {
#    LIBS += -lopengl32
#}

win32:LIBS += -L$$SK/lib -llibvlc \
              -lmswsock -lws2_32

win32:LIBS += -L$$SK/lib -ltorrent \
              -L$$SK/lib -lboost_system

# Boost dependencies
win32-msvc*:LIBS += Advapi32.lib Iphlpapi.lib

# Windows dependency for ShellExecuteA and SystemParametersInfo
win32-msvc*:LIBS += shell32.lib User32.lib

unix:!ios:!android:LIBS += -L$$SK/lib -lvlc \
                           -L$$SK/lib -ltorrent-rasterbar \
                           -L$$SK/lib -lboost_system

android:LIBS += -L$$SK/lib/$$ABI -lvlc \
                -L$$SK/lib/$$ABI -ltorrent-rasterbar \
                -L$$SK/lib/$$ABI -ltry_signal

unix:!macx:!ios:!android:contains(QT_MAJOR_VERSION, 4) {
    LIBS += -lX11
}

macx {
CONFIG(debug, debug|release) {
    QMAKE_POST_LINK  = install_name_tool -change libSkCoreD.dylib \
                       @executable_path/libSkCoreD.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkGuiD.dylib \
                       @executable_path/libSkGuiD.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkBarcodeD.dylib \
                       @executable_path/libSkBarcodeD.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkBackendD.dylib \
                       @executable_path/libSkTorrentD.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkMediaD.dylib \
                       @executable_path/libSkMediaD.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkMultimediaD.dylib \
                       @executable_path/libSkMultimediaD.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkTorrentD.dylib \
                       @executable_path/libSkTorrentD.dylib $${DESTDIR}/$${TARGET};
} else {
    QMAKE_POST_LINK  = install_name_tool -change libSkCore.dylib \
                       @executable_path/libSkCore.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkGui.dylib \
                       @executable_path/libSkGui.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkBarcode.dylib \
                       @executable_path/libSkBarcode.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkBackend.dylib \
                       @executable_path/libSkTorrent.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkMedia.dylib \
                       @executable_path/libSkMedia.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkMultimedia.dylib \
                       @executable_path/libSkMultimedia.dylib $${DESTDIR}/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libSkTorrent.dylib \
                       @executable_path/libSkTorrent.dylib $${DESTDIR}/$${TARGET};
}
}

macx {
    PATH=$${DESTDIR}/$${TARGET}.app/Contents/MacOS

    QMAKE_POST_LINK = install_name_tool -change @rpath/libvlccore.dylib \
                      @loader_path/libvlccore.dylib $${DESTDIR}/libvlc.dylib;

    QMAKE_POST_LINK += install_name_tool -change @rpath/libvlc.dylib \
                       @loader_path/libvlc.dylib $$PATH/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libtorrent-rasterbar.dylib.2.0.11 \
                       @loader_path/libtorrent-rasterbar.dylib $$PATH/$${TARGET};

    QMAKE_POST_LINK += install_name_tool -change libboost_system.dylib \
                       @loader_path/libboost_system.dylib $$PATH/$${TARGET};

    QMAKE_POST_LINK += $${QMAKE_COPY} -r $${DESTDIR}/plugins $$PATH;

    QMAKE_POST_LINK += $${QMAKE_COPY} $${DESTDIR}/libvlc.dylib     $$PATH;
    QMAKE_POST_LINK += $${QMAKE_COPY} $${DESTDIR}/libvlccore.dylib $$PATH;
}

macx:ICON = dist/icon.icns

RC_FILE = dist/sky.rc

OTHER_FILES += environment.sh \
               content/generate.sh \
               qml/Main.qml \
               dist/sky.rc \
