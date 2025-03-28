SK = $$_PRO_FILE_PWD_/../..

CONFIG(debug, debug|release) {
    TARGET = SkCoreD
} else {
    TARGET = SkCore
}

DESTDIR = $$SK/lib

TEMPLATE = lib

contains(QT_MAJOR_VERSION, 4) {

    QT += declarative network script xml xmlpatterns

} else:contains(QT_MAJOR_VERSION, 5) {

    QT += qml network xml xmlpatterns
} else {
    QT += qml network xml core5compat
}

contains(QT_MAJOR_VERSION, 5) {
    android:QT += androidextras
}

win32:CONFIG += dll

# C++17
contains(QT_MAJOR_VERSION, 4) {

    QMAKE_CXXFLAGS += -std=c++1z
} else {
    CONFIG += c++1z
}

DEFINES += QUAZIP_BUILD SK_CORE_LIBRARY SK_CHARSET

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include(../Sk.pri)
include(src/global/global.pri)
include(src/controllers/controllers.pri)
include(src/kernel/kernel.pri)
include(src/io/io.pri)
include(src/thread/thread.pri)
include(src/network/network.pri)
#include(src/script/script.pri)
#include(src/plugin/plugin.pri)

include(src/3rdparty/qtsingleapplication/qtsingleapplication.pri)
include(src/3rdparty/zlib/zlib.pri)
include(src/3rdparty/quazip/quazip.pri)
include(src/3rdparty/libcharsetdetect/libcharsetdetect.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkCore/private

# NOTE android: Required for qandroidextras_p
android:greaterThan(QT_MAJOR_VERSION, 4) {
    INCLUDEPATH += $$SK/include/$$QTX \
                   $$SK/include/$$QTX/QtCore
}

unix:!macx:!ios:!android:greaterThan(QT_MAJOR_VERSION, 4) {

    INCLUDEPATH += $$SK/include/$$QTX/QtDBus
}

# Windows dependency for ShellExecuteA and SystemParametersInfo
win32-msvc*:LIBS += shell32.lib User32.lib
