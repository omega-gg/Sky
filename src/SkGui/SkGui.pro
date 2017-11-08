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

QT += opengl declarative network script xml svg

contains(QT_MAJOR_VERSION, 5): win32:QT += winextras

CONFIG       += plugin
win32:CONFIG += dll

DEFINES += QT_QTLOCKEDFILE_IMPORT

DEFINES += SK_GUI_LIBRARY

contains(QT_MAJOR_VERSION, 4) {
    DEFINES += QT_4

    CONFIG(release, debug|release) {

        win32:DEFINES += SK_WIN_NATIVE
    }
} else {
    DEFINES += QT_LATEST
}

include(src/controllers/controllers.pri)
include(src/kernel/kernel.pri)
include(src/image/image.pri)
include(src/graphicsview/graphicsview.pri)
include(src/declarative/declarative.pri)
include(src/models/models.pri)
include(src/media/media.pri)

INCLUDEPATH += $$SK/include/Qt/QtCore \
               $$SK/include/Qt/QtGui \
               $$SK/include/Qt/QtDeclarative \
               $$SK/include/SkCore \
               $$SK/include/SkCore/private \
               $$SK/include/SkGui \
               $$SK/include/SkGui/private \

CONFIG(debug, debug|release) {

    LIBS += -L$$SK/lib -lSkCoreD
} else {
    LIBS += -L$$SK/lib -lSkCore
}

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12

    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
}

macx {
CONFIG(debug, debug|release) {
    QMAKE_POST_LINK = install_name_tool -change libSkCoreD.dylib \
                      @loader_path/libSkCoreD.dylib $${DESTDIR}/lib$${TARGET}.dylib ;
} else {
    QMAKE_POST_LINK = install_name_tool -change libSkCore.dylib \
                      @loader_path/libSkCore.dylib $${DESTDIR}/lib$${TARGET}.dylib ;
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
