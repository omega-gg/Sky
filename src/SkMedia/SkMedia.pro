SK = $$_PRO_FILE_PWD_/../..

contains(QT_MAJOR_VERSION, 4) {
    SK_BIN = bin
} else {
    SK_BIN = latest
}

CONFIG(debug, debug|release) {
    TARGET = SkMediaD
} else {
    TARGET = SkMedia
}

DESTDIR = $$SK/lib

TEMPLATE = lib

QT += opengl declarative network

CONFIG       += plugin
win32:CONFIG += dll

DEFINES += SK_MEDIA_LIBRARY CAN_COMPILE_SSE2

contains(QT_MAJOR_VERSION, 4) {
    DEFINES += QT_4
} else {
    DEFINES += QT_LATEST
}

QMAKE_CXXFLAGS += -msse

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

CONFIG(debug, debug|release) {

    LIBS += -L$$SK/lib -lSkCoreD \
            -L$$SK/lib -lSkGuiD
} else {
    LIBS += -L$$SK/lib -lSkCore \
            -L$$SK/lib -lSkGui
}

contains(QT_MAJOR_VERSION, 5):!macx {
    LIBS += -lopengl32
}

win32:LIBS += -L$$SK/lib -llibvlc
unix: LIBS += -lvlc

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12

    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
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

#--------------------------------------------------------------------------------------------------
# Copy library to the bin directory

win32:equals(QMAKE_COPY, "copy /y") {
    SK ~= s,/,\\,g

    QMAKE_POST_LINK += $${QMAKE_COPY} $$SK\\lib\\$${TARGET}.dll $$SK\\$$SK_BIN
}

win32:equals(QMAKE_COPY, "cp") {
    QMAKE_POST_LINK += $${QMAKE_COPY} $$SK/lib/$${TARGET}.dll $$SK/$$SK_BIN
}

macx: QMAKE_POST_LINK += $${QMAKE_COPY} $$SK/lib/lib$${TARGET}.dylib $$SK/$$SK_BIN

unix:!macx {
    QMAKE_POST_LINK += $${QMAKE_COPY} $$SK/lib/lib$${TARGET}.so $$SK/$$SK_BIN
}
