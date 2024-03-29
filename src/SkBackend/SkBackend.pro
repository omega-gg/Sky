SK = $$_PRO_FILE_PWD_/../..

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

# C++17
contains(QT_MAJOR_VERSION, 4) {
    QMAKE_CXXFLAGS += -std=c++1z
} else {
    CONFIG += c++1z
}

DEFINES += SK_BACKEND_LIBRARY #SK_BACKEND_LOG

ios:DEFINES += SK_NO_TORRENT

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include(../Sk.pri)
include(src/io/io.pri)
include(src/media/media.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkCore/private \
               $$SK/include/SkGui \
               $$SK/include/SkGui/private \
               $$SK/include/SkBackend \
               $$SK/include/SkBackend/private \
               $$SK/include

android {
    CONFIG(debug, debug|release) {

        LIBS += -L$$SK/lib -lSkCoreD_$$ABI \
                -L$$SK/lib -lSkGuiD_$$ABI
    } else {
        LIBS += -L$$SK/lib -lSkCore_$$ABI \
                -L$$SK/lib -lSkGui_$$ABI
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
