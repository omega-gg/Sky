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
