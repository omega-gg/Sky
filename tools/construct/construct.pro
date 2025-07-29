SK = $$_PRO_FILE_PWD_/../..

TARGET = construct

DESTDIR = $$SK/bin

macx: CONFIG -= app_bundle

contains(QT_MAJOR_VERSION, 4) {
    QT += opengl declarative network xml
} else {
    QT += opengl widgets quick network xml
}

# C++17
contains(QT_MAJOR_VERSION, 4) {
    QMAKE_CXXFLAGS += -std=c++1z
} else {
    CONFIG += c++1z
}

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

include($$SK/src/Sk.pri)
include(src/global/global.pri)
include(src/controllers/controllers.pri)

INCLUDEPATH += $$SK/include/SkCore \
               $$SK/include/SkGui \
               $$SK/include/SkBarcode \
               $$SK/include/SkBackend \
               $$SK/include/SkMedia \
               $$SK/include/SkTorrent \
               $$SK/include \
               src/controllers

unix:contains(QT_MAJOR_VERSION, 4) {
    INCLUDEPATH += $$SK/include/$$QTX \
                   $$SK/include/$$QTX/QtCore \
                   $$SK/include/$$QTX/QtGui \
                   $$SK/include/$$QTX/QtDeclarative
}

greaterThan(QT_MAJOR_VERSION, 4) {
    INCLUDEPATH += $$SK/include/$$QTX \
                   $$SK/include/$$QTX/QtCore \
                   $$SK/include/$$QTX/QtGui \
                   $$SK/include/$$QTX/QtQml \
                   $$SK/include/$$QTX/QtQuick
}

CONFIG(debug, debug|release) {

    LIBS += -L$$SK/lib -lSkCoreD \
            -L$$SK/lib -lSkGuiD \
            -L$$SK/lib -lSkMediaD \
            -L$$SK/lib -lSkBarcodeD \
            -L$$SK/lib -lSkTorrentD \
            -L$$SK/lib -lSkBackendD \
} else {
    LIBS += -L$$SK/lib -lSkCore \
            -L$$SK/lib -lSkGui \
            -L$$SK/lib -lSkMedia \
            -L$$SK/lib -lSkBarcode \
            -L$$SK/lib -lSkTorrent \
            -L$$SK/lib -lSkBackend \
}

OTHER_FILES += qml/Main.qml \

macx {
CONFIG(debug, debug|release) {
    QMAKE_POST_LINK  = install_name_tool -change libSkCoreD.dylib \
                       @executable_path/libSkCoreD.dylib $${DESTDIR}/$${TARGET} ;

    QMAKE_POST_LINK += install_name_tool -change libSkGuiD.dylib \
                       @executable_path/libSkGuiD.dylib $${DESTDIR}/$${TARGET} ;

    QMAKE_POST_LINK += install_name_tool -change libSkMediaD.dylib \
                       @executable_path/libSkMediaD.dylib $${DESTDIR}/$${TARGET} ;
} else {
    QMAKE_POST_LINK  = install_name_tool -change libSkCore.dylib \
                       @executable_path/libSkCore.dylib $${DESTDIR}/$${TARGET} ;

    QMAKE_POST_LINK += install_name_tool -change libSkGui.dylib \
                       @executable_path/libSkGui.dylib $${DESTDIR}/$${TARGET} ;

    QMAKE_POST_LINK += install_name_tool -change libSkMedia.dylib \
                       @executable_path/libSkMedia.dylib $${DESTDIR}/$${TARGET} ;
}
}
