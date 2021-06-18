SK = $$_PRO_FILE_PWD_/../..

SK_BIN = bin

CONFIG(debug, debug|release) {
    TARGET = SkTorrentD
} else {
    TARGET = SkTorrent
}

DESTDIR = $$SK/lib

TEMPLATE = lib

contains(QT_MAJOR_VERSION, 4) {
    QT += declarative network
} else {
    QT += quick network
}

win32:CONFIG += dll

# NOTE macOS: This seems to be required when building against libtorrent 2.0+.
macx:CONFIG += c++14

DEFINES += SK_TORRENT_LIBRARY

win32-msvc* {
    # libtorrent: This fixes the winsock2 errors
    DEFINES += WIN32_LEAN_AND_MEAN

    # Boost: This prevents an issue with linking
    DEFINES += BOOST_ALL_NO_LIB
}

contains(QT_MAJOR_VERSION, 4) {
    DEFINES += QT_4
} else {
    DEFINES += QT_LATEST
}

android {
    DEFINES += SK_MOBILE
} else {
    DEFINES += SK_DESKTOP
}

!win32-msvc*:QMAKE_CXXFLAGS += -std=c++14

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include(src/controllers/controllers.pri)
include(src/io/io.pri)
include(src/media/media.pri)
include(src/torrent/torrent.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkCore/private \
               $$SK/include/SkGui \
               $$SK/include/SkGui/private \
               $$SK/include/SkTorrent \
               $$SK/include/SkTorrent/private \
               $$SK/include \

android {
    CONFIG(debug, debug|release) {

        LIBS += -L$$SK/lib -lSkCoreD_$$ANDROID_TARGET_ARCH \
                -L$$SK/lib -lSkGuiD_$$ANDROID_TARGET_ARCH
    } else {
        LIBS += -L$$SK/lib -lSkCore_$$ANDROID_TARGET_ARCH \
                -L$$SK/lib -lSkGui_$$ANDROID_TARGET_ARCH
    }
} else {
    CONFIG(debug, debug|release) {

        LIBS += -L$$SK/lib -lSkCoreD \
                -L$$SK/lib -lSkGuiD
    } else {
        LIBS += -L$$SK/lib -lSkCore \
                -L$$SK/lib -lSkGui
    }
}

win32:LIBS += -lmswsock -lws2_32

win32:LIBS += -L$$SK/lib -ltorrent \
              -L$$SK/lib -lboost_system

# Boost dependencies
win32-msvc*:LIBS += Advapi32.lib Iphlpapi.lib

macx:LIBS += -L$$SK/lib -ltorrent-rasterbar \
             -L$$SK/lib -lboost_system

unix:!macx:!android:LIBS += -ltorrent-rasterbar \
                            -lboost_system -lboost_random -lboost_chrono

android:LIBS += -L$$SK/lib/$$ANDROID_TARGET_ARCH -ltorrent-rasterbar \
                -L$$SK/lib/$$ANDROID_TARGET_ARCH -ltry_signal

macx {
CONFIG(debug, debug|release) {
    QMAKE_POST_LINK  = install_name_tool -change libSkCoreD.dylib \
                       @loader_path/libSkCoreD.dylib $${DESTDIR}/lib$${TARGET}.dylib;

    QMAKE_POST_LINK += install_name_tool -change libSkGuiD.dylib \
                       @loader_path/libSkGuiD.dylib $${DESTDIR}/lib$${TARGET}.dylib;
} else {
    QMAKE_POST_LINK  = install_name_tool -change libSkCore.dylib \
                       @loader_path/libSkCore.dylib $${DESTDIR}/lib$${TARGET}.dylib;

    QMAKE_POST_LINK += install_name_tool -change libSkGui.dylib \
                       @loader_path/libSkGui.dylib $${DESTDIR}/lib$${TARGET}.dylib;
}
}
