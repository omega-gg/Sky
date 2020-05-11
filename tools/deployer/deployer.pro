SK = $$_PRO_FILE_PWD_/../..

TARGET = deployer

DESTDIR = $$SK/bin

QT -= gui

CONFIG += console

macx:CONFIG -= app_bundle

!msvc:QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp
