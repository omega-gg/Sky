SK = $$_PRO_FILE_PWD_/../..

TARGET = deployer

DESTDIR = $$SK/bin

QT -= gui

CONFIG += console

macx:CONFIG -= app_bundle

contains(QT_MAJOR_VERSION, 4) {
    QMAKE_CXXFLAGS += -std=c++17
} else {
    CONFIG += c++17
}

SOURCES += main.cpp
