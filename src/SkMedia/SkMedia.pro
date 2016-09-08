SK = $$_PRO_FILE_PWD_/../..

greaterThan(QT_MAJOR_VERSION, 4) {
    SK_BIN = latest
} else {
    SK_BIN = bin
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

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += QT_LATEST

QMAKE_CXXFLAGS += -msse

include(src/controllers/controllers.pri)
include(src/media/media.pri)
include(src/vlc/vlc.pri)

INCLUDEPATH += \
            $$SK/include/SkCore \
            $$SK/include/SkCore/private \
            $$SK/include/SkGui \
            $$SK/include/SkGui/private \
            $$SK/include/SkMedia \
            $$SK/include/SkMedia/private \
            $$SK/include/ \

greaterThan(QT_MAJOR_VERSION, 4): LIBS += -lopengl32

CONFIG(debug, debug|release) {
    LIBS += \
         -L$$SK/lib -lSkCoreD \
         -L$$SK/lib -lSkGuiD \

    linux:LIBS += -L$$SK/lib -lvlc \

    win32:LIBS += -L$$SK/lib -llibvlc \

} else {
    LIBS += \
         -L$$SK/lib -lSkCore \
         -L$$SK/lib -lSkGui \
         -L$$SK/lib -lvlc \

    linux:LIBS += -L$$SK/lib -lvlc \

    win32:LIBS += -L$$SK/lib -llibvlc \
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

macx:  QMAKE_POST_LINK += $${QMAKE_COPY} $$SK/lib/lib$${TARGET}.dylib $$SK/$$SK_BIN
linux: QMAKE_POST_LINK += $${QMAKE_COPY} $$SK/lib/lib$${TARGET}.so    $$SK/$$SK_BIN
