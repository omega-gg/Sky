SK = $$_PRO_FILE_PWD_/../..

SK_BIN = bin

CONFIG(debug, debug|release) {
    TARGET = SkWebD
} else {
    TARGET = SkWeb
}

DESTDIR = $$SK/lib

TEMPLATE = lib

contains(QT_MAJOR_VERSION, 4) {
    QT += network webkit
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

DEFINES += SK_WEB_LIBRARY

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

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include(src/io/io.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkCore/private \
               $$SK/include/SkGui \
               $$SK/include/SkGui/private \
               $$SK/include/SkWeb \
               $$SK/include/SkWeb/private

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

macx {
CONFIG(debug, debug|release) {
    QMAKE_POST_LINK  = install_name_tool -change libSkCoreD.dylib \
                       @loader_path/libSkCoreD.dylib $${DESTDIR}/lib$${TARGET}.dylib ;

    QMAKE_POST_LINK += install_name_tool -change libSkGuiD.dylib \
                       @loader_path/libSkGuiD.dylib $${DESTDIR}/lib$${TARGET}.dylib ;
} else {
    QMAKE_POST_LINK  = install_name_tool -change libSkCore.dylib \
                       @loader_path/libSkCore.dylib $${DESTDIR}/lib$${TARGET}.dylib ;

    QMAKE_POST_LINK += install_name_tool -change libSkGui.dylib \
                       @loader_path/libSkGui.dylib $${DESTDIR}/lib$${TARGET}.dylib ;
}
}
