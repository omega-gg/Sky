SK = $$_PRO_FILE_PWD_/../..

TARGET = androiddeployqt

DESTDIR = $$SK/bin

QT -= gui

CONFIG += console

macx:CONFIG -= app_bundle

# C++17
contains(QT_MAJOR_VERSION, 4) {
    QMAKE_CXXFLAGS += -std=c++1z
} else {
    CONFIG += c++1z
}

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

lessThan(QT_MAJOR_VERSION, 6) {
    SOURCES += qt5/main.cpp
} else {
    SOURCES += qt6/main.cpp
}
