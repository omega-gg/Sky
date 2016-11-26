SK = $$_PRO_FILE_PWD_/../..

TARGET = includeGenerator

contains(QT_MAJOR_VERSION, 4) {
    DESTDIR = $$SK/bin
} else {
    DESTDIR = $$SK/latest
}

QT -= gui

CONFIG += console

macx:CONFIG -= app_bundle

SOURCES += main.cpp
