SK = $$_PRO_FILE_PWD_/../..

SK_BIN = bin

CONFIG(debug, debug|release) {
    TARGET = SkCoreD
} else {
    TARGET = SkCore
}

DESTDIR = $$SK/lib

TEMPLATE = lib

contains(QT_MAJOR_VERSION, 4) {
    QT += declarative network script xml xmlpatterns
} else {
    QT += qml network xml xmlpatterns
}

contains(QT_MAJOR_VERSION, 5) {
    android:QT += androidextras
}

win32:CONFIG += dll

DEFINES += QUAZIP_BUILD SK_CORE_LIBRARY SK_CHARSET

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

!win32-msvc*:QMAKE_CXXFLAGS += -std=c++11

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include(src/global/global.pri)
include(src/controllers/controllers.pri)
include(src/kernel/kernel.pri)
include(src/io/io.pri)
include(src/thread/thread.pri)
include(src/network/network.pri)
#include(src/script/script.pri)
#include(src/plugin/plugin.pri)

include(src/3rdparty/qtsingleapplication/qtsingleapplication.pri)
include(src/3rdparty/quazip/quazip.pri)
include(src/3rdparty/libcharsetdetect/libcharsetdetect.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkCore/private \

win32-msvc*:INCLUDEPATH += $$[QT_INSTALL_PREFIX]/include/QtZlib

win32:!win32-msvc*:LIBS += -L$$SK/lib -lz

# Windows dependency for ShellExecuteA and SystemParametersInfo
win32-msvc*:LIBS += shell32.lib User32.lib \

unix:LIBS += -lz
