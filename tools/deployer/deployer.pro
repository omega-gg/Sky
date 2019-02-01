SK = $$_PRO_FILE_PWD_/../..

TARGET = deployer

contains(QT_MAJOR_VERSION, 4) {
    DESTDIR = $$SK/bin
} else {
    DESTDIR = $$SK/latest
}

QT -= gui

CONFIG += console

macx:CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12

    QMAKE_CXXFLAGS += -stdlib=libc++
}
