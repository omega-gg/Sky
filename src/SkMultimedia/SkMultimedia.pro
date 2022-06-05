SK = $$_PRO_FILE_PWD_/../..

CONFIG(debug, debug|release) {
    TARGET = SkMultimediaD
} else {
    TARGET = SkMultimedia
}

DESTDIR = $$SK/lib

TEMPLATE = lib

QT += qml multimedia

win32:CONFIG += dll

# C++17
CONFIG += c++1z

DEFINES += SK_MULTIMEDIA_LIBRARY

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include(../Sk.pri)
include(src/media/media.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkCore/private \
               $$SK/include/SkGui \
               $$SK/include/SkGui/private \
               $$SK/include/SkBarcode \
               $$SK/include/SkBarcode/private \
               $$SK/include

android {
    CONFIG(debug, debug|release) {

        LIBS += -L$$SK/lib -lSkCoreD_$$ABI \
                -L$$SK/lib -lSkGuiD_$$ABI \
                -L$$SK/lib -lSkBarcodeD_$$ABI
    } else {
        LIBS += -L$$SK/lib -lSkCore_$$ABI \
                -L$$SK/lib -lSkGui_$$ABI \
                -L$$SK/lib -lSkBarcode_$$ABI
    }
} else {
    CONFIG(debug, debug|release) {

        LIBS += -L$$SK/lib -lSkCoreD \
                -L$$SK/lib -lSkGuiD \
                -L$$SK/lib -lSkBarcodeD
    } else {
        LIBS += -L$$SK/lib -lSkCore \
                -L$$SK/lib -lSkGui \
                -L$$SK/lib -lSkBarcode
    }
}

macx {
CONFIG(debug, debug|release) {
    QMAKE_POST_LINK  = install_name_tool -change libSkCoreD.dylib \
                       @loader_path/libSkCoreD.dylib $${DESTDIR}/lib$${TARGET}.dylib;

    QMAKE_POST_LINK += install_name_tool -change libSkGuiD.dylib \
                       @loader_path/libSkGuiD.dylib $${DESTDIR}/lib$${TARGET}.dylib;

   QMAKE_POST_LINK += install_name_tool -change libSkBarcodeD.dylib \
                      @loader_path/libSkBarcodeD.dylib $${DESTDIR}/lib$${TARGET}.dylib;
} else {
    QMAKE_POST_LINK  = install_name_tool -change libSkCore.dylib \
                       @loader_path/libSkCore.dylib $${DESTDIR}/lib$${TARGET}.dylib;

    QMAKE_POST_LINK += install_name_tool -change libSkGui.dylib \
                       @loader_path/libSkGui.dylib $${DESTDIR}/lib$${TARGET}.dylib;

    QMAKE_POST_LINK += install_name_tool -change libSkBarcode.dylib \
                       @loader_path/libSkBarcode.dylib $${DESTDIR}/lib$${TARGET}.dylib;
}
}
