SK = $$_PRO_FILE_PWD_/../..

contains(QT_MAJOR_VERSION, 4) {
    SK_BIN = bin
} else {
    SK_BIN = latest
}

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

DEFINES += SK_TORRENT_LIBRARY

contains(QT_MAJOR_VERSION, 4) {
    DEFINES += QT_4
} else {
    DEFINES += QT_LATEST
}

QMAKE_CXXFLAGS += -std=c++11

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

win32:LIBS += -L$$SK/lib -ltorrent \
              -L$$SK/lib -lboost_system \
              -lmswsock -lws2_32 \

macx:LIBS += -L$$SK/lib -ltorrent \
             -L$$SK/lib -lboost_system \

unix:!macx:!android:LIBS += -ltorrent-rasterbar \
                            -lboost_system -lboost_random -lboost_chrono \

android:LIBS += -ltorrent_$$ANDROID_TARGET_ARCH

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
