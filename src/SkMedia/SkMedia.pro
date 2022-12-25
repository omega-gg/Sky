SK = $$_PRO_FILE_PWD_/../..

CONFIG(debug, debug|release) {
    TARGET = SkMediaD
} else {
    TARGET = SkMedia
}

DESTDIR = $$SK/lib

TEMPLATE = lib

contains(QT_MAJOR_VERSION, 4) {
    QT += opengl declarative network

} else:contains(QT_MAJOR_VERSION, 5) {

    QT += opengl quick network
} else {
    QT += quick network
}

win32:CONFIG += dll

# C++17
contains(QT_MAJOR_VERSION, 4) {
    QMAKE_CXXFLAGS += -std=c++1z
} else {
    CONFIG += c++1z
}

DEFINES += SK_MEDIA_LIBRARY

!win32-msvc*:!ios:!android:DEFINES += CAN_COMPILE_SSE2

!win32-msvc*:!ios:!android:QMAKE_CXXFLAGS += -msse

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include(../Sk.pri)
include(src/controllers/controllers.pri)
include(src/media/media.pri)
include(src/vlc/vlc.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkCore/private \
               $$SK/include/SkGui \
               $$SK/include/SkGui/private \
               $$SK/include/SkMedia \
               $$SK/include/SkMedia/private \
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

win32:greaterThan(QT_MAJOR_VERSION, 4) {
    LIBS += -lopengl32
}

win32:LIBS += -L$$SK/lib -llibvlc

unix:!android:LIBS += -L$$SK/lib -lvlc

android:LIBS += -L$$SK/lib/$$ABI -lvlc

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
