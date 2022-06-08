# Media module

HEADERS += src/media/WFilterBarcode.h \
           src/media/WFilterBarcode_p.h \

contains(QT_MAJOR_VERSION, 5): {
    HEADERS += src/media/Qt/qvideoframeconversionhelper_p.h
}

SOURCES += src/media/WFilterBarcode.cpp \
