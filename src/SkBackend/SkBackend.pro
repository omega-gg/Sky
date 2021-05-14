SK = $$_PRO_FILE_PWD_/../..

SK_BIN = bin

CONFIG(debug, debug|release) {
    TARGET = SkBackendD
} else {
    TARGET = SkBackend
}

DESTDIR = $$SK/lib

TEMPLATE = lib

contains(QT_MAJOR_VERSION, 4) {
    QT += declarative network script xml
} else {
    QT += quick network xml
}

win32:CONFIG += dll

DEFINES += SK_BACKEND_LIBRARY SK_BACKEND_LOG

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

include(src/io/io.pri)
include(src/media/media.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkCore/private \
               $$SK/include/SkGui \
               $$SK/include/SkGui/private \
               $$SK/include/SkTorrent \
               $$SK/include/SkTorrent/private \
               $$SK/include/SkBackend \
               $$SK/include/SkBackend/private \

android {
    CONFIG(debug, debug|release) {

        LIBS += -L$$SK/lib -lSkCoreD_$$ANDROID_TARGET_ARCH \
                -L$$SK/lib -lSkGuiD_$$ANDROID_TARGET_ARCH \
                -L$$SK/lib -lSkTorrentD_$$ANDROID_TARGET_ARCH
    } else {
        LIBS += -L$$SK/lib -lSkCore_$$ANDROID_TARGET_ARCH \
                -L$$SK/lib -lSkGui_$$ANDROID_TARGET_ARCH \
                -L$$SK/lib -lSkTorrent_$$ANDROID_TARGET_ARCH
    }
} else {
    CONFIG(debug, debug|release) {

        LIBS += -L$$SK/lib -lSkCoreD \
                -L$$SK/lib -lSkGuiD \
                -L$$SK/lib -lSkTorrentD
    } else {
        LIBS += -L$$SK/lib -lSkCore \
                -L$$SK/lib -lSkGui \
                -L$$SK/lib -lSkTorrent
    }
}

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
