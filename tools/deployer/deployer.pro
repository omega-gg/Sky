SK = $$_PRO_FILE_PWD_/../..

TARGET = deployer

greaterThan(QT_MAJOR_VERSION, 4) {
    DESTDIR = $$SK/latest
} else {
    DESTDIR = $$SK/bin
}

QT -= gui

CONFIG += console

macx:CONFIG -= app_bundle

SOURCES += main.cpp
