# Qt module

contains(QT_MAJOR_VERSION, 4):HEADERS += src/3rdparty/Qt/qdeclarativemousearea_p.h \
                                         src/3rdparty/Qt/qdeclarativeevents_p_p.h \

contains(QT_MAJOR_VERSION, 4):SOURCES += src/3rdparty/Qt/qdeclarativemousearea.cpp \
