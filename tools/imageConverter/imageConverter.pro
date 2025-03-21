SK = $$_PRO_FILE_PWD_/../..

TARGET = imageConverter

DESTDIR = $$SK/bin

CONFIG += console

macx:CONFIG -= app_bundle

# C++17
contains(QT_MAJOR_VERSION, 4) {
    QMAKE_CXXFLAGS += -std=c++1z
} else {
    CONFIG += c++1z
}

unix:QMAKE_LFLAGS += "-Wl,-rpath,'\$$ORIGIN'"

SOURCES += main.cpp
