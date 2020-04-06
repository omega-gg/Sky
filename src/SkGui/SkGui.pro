SK = $$_PRO_FILE_PWD_/../..

contains(QT_MAJOR_VERSION, 4) {
    SK_BIN = bin
} else {
    SK_BIN = latest
}

CONFIG(debug, debug|release) {
    TARGET = SkGuiD
} else {
    TARGET = SkGui
}

DESTDIR = $$SK/lib

TEMPLATE = lib

contains(QT_MAJOR_VERSION, 4) {
    QT += opengl declarative network script xml svg
} else {
    QT += opengl quick network xml svg
}

contains(QT_MAJOR_VERSION, 5) {
    win32:QT += winextras

    unix:!macx:!android:QT += x11extras
}

win32:CONFIG += dll

DEFINES += QT_QTLOCKEDFILE_IMPORT

android:DEFINES += QT_OPENGL_ES_2

DEFINES += SK_GUI_LIBRARY

contains(QT_MAJOR_VERSION, 4) {
    DEFINES += QT_4

    CONFIG(release, debug|release) {

        win32:DEFINES += SK_WIN_NATIVE
    }
} else {
    DEFINES += QT_LATEST #SK_SOFTWARE

    win32:DEFINES += SK_WIN_NATIVE
}

QMAKE_CXXFLAGS += -std=c++11

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include(src/controllers/controllers.pri)
include(src/kernel/kernel.pri)
include(src/io/io.pri)
include(src/image/image.pri)
include(src/graphicsview/graphicsview.pri)
include(src/declarative/declarative.pri)
include(src/models/models.pri)
include(src/media/media.pri)

include(src/3rdparty/qtsingleapplication/qtsingleapplication.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkCore/private \
               $$SK/include/SkGui \
               $$SK/include/SkGui/private \

contains(QT_MAJOR_VERSION, 5) {
    INCLUDEPATH += $$SK/include/Qt5 \
                   $$SK/include/Qt5/QtCore \
                   $$SK/include/Qt5/QtGui \
                   $$SK/include/Qt5/QtQml \
                   $$SK/include/Qt5/QtQuick
}

unix:contains(QT_MAJOR_VERSION, 4) {
    INCLUDEPATH += $$SK/include/Qt4/QtCore \
                   $$SK/include/Qt4/QtGui \
                   $$SK/include/Qt4/QtDeclarative
}

android {
    CONFIG(debug, debug|release) {

        LIBS += -L$$SK/lib -lSkCoreD_$$ANDROID_TARGET_ARCH
    } else {
        LIBS += -L$$SK/lib -lSkCore_$$ANDROID_TARGET_ARCH
    }
} else {
    CONFIG(debug, debug|release) {

        LIBS += -L$$SK/lib -lSkCoreD
    } else {
        LIBS += -L$$SK/lib -lSkCore
    }
}

macx {
CONFIG(debug, debug|release) {
    QMAKE_POST_LINK = install_name_tool -change libSkCoreD.dylib \
                      @loader_path/libSkCoreD.dylib $${DESTDIR}/lib$${TARGET}.dylib;
} else {
    QMAKE_POST_LINK = install_name_tool -change libSkCore.dylib \
                      @loader_path/libSkCore.dylib $${DESTDIR}/lib$${TARGET}.dylib;
}
}
