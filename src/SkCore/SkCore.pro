SK = $$_PRO_FILE_PWD_/../..

contains(QT_MAJOR_VERSION, 4) {
    SK_BIN = bin
} else {
    SK_BIN = latest
}

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
    QT += quick widgets network xml xmlpatterns
}

CONFIG       += plugin
win32:CONFIG += dll

DEFINES += SK_CORE_LIBRARY QUAZIP_BUILD

contains(QT_MAJOR_VERSION, 4) {
    DEFINES += QT_4
} else {
    DEFINES += QT_LATEST
}

QMAKE_CXXFLAGS += -std=c++11

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include(src/global/global.pri)
include(src/controllers/controllers.pri)
include(src/kernel/kernel.pri)
include(src/io/io.pri)
include(src/thread/thread.pri)
include(src/network/network.pri)
include(src/declarative/declarative.pri)
#include(src/script/script.pri)
include(src/plugin/plugin.pri)

include(3rdparty/qtsingleapplication/qtsingleapplication.pri)
include(3rdparty/quazip/quazip.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkCore/private \
               3rdparty/qtsingleapplication \
               3rdparty/quazip \

win32:LIBS += -L$$SK/lib -lz

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12

    QMAKE_CXXFLAGS += -stdlib=libc++
}

#--------------------------------------------------------------------------------------------------
# Copy library to the bin directory

win32 {
equals(QMAKE_COPY, "copy /y") {
    SK ~= s,/,\\,g

    QMAKE_POST_LINK += $${QMAKE_COPY} $$SK\\lib\\$${TARGET}.dll $$SK\\$$SK_BIN
} else {
    QMAKE_POST_LINK += $${QMAKE_COPY} $$SK/lib/$${TARGET}.dll $$SK/$$SK_BIN
}
}

macx: QMAKE_POST_LINK += $${QMAKE_COPY} $$SK/lib/lib$${TARGET}.dylib $$SK/$$SK_BIN

unix:!macx {
    QMAKE_POST_LINK += $${QMAKE_COPY} $$SK/lib/lib$${TARGET}.so $$SK/$$SK_BIN
}
