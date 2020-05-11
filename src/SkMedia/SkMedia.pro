SK = $$_PRO_FILE_PWD_/../..

SK_BIN = bin

CONFIG(debug, debug|release) {
    TARGET = SkMediaD
} else {
    TARGET = SkMedia
}

DESTDIR = $$SK/lib

TEMPLATE = lib

contains(QT_MAJOR_VERSION, 4) {
    QT += opengl declarative network
} else {
    QT += opengl quick network
}

win32:CONFIG += dll

DEFINES += SK_MEDIA_LIBRARY

!msvc:!android:DEFINES += CAN_COMPILE_SSE2

contains(QT_MAJOR_VERSION, 4) {
    DEFINES += QT_4
} else {
    DEFINES += QT_LATEST
}

!msvc:QMAKE_CXXFLAGS += -std=c++11

!msvc:!android:QMAKE_CXXFLAGS += -msse

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include(src/controllers/controllers.pri)
include(src/media/media.pri)
include(src/vlc/vlc.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkCore/private \
               $$SK/include/SkGui \
               $$SK/include/SkGui/private \
               $$SK/include/SkMedia \
               $$SK/include/SkMedia/private \
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

win32:contains(QT_MAJOR_VERSION, 5) {
    LIBS += -lopengl32
}

win32:LIBS += -L$$SK/lib -llibvlc

macx:LIBS += -L$$SK/lib -lvlc

unix:!macx:!android: LIBS += -lvlc

android:LIBS += -lvlc_$$ANDROID_TARGET_ARCH

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
