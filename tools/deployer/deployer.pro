SK = $$_PRO_FILE_PWD_/../..

TARGET = deployer

DESTDIR = $$SK/bin

QT -= gui

CONFIG += console

macx:CONFIG -= app_bundle

!win32-msvc*:QMAKE_CXXFLAGS += -std=c++14

SOURCES += main.cpp
