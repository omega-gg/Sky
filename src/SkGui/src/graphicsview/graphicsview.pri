# Graphics view module

HEADERS += src/graphicsview/WAbstractView.h \
           src/graphicsview/WAbstractView_p.h \
           src/graphicsview/WView.h \
           src/graphicsview/WView_p.h \
           src/graphicsview/WViewResizer.h \
           src/graphicsview/WViewDrag.h \
           src/graphicsview/WWindow.h \
           src/graphicsview/WWindow_p.h \
           src/graphicsview/WResizer.h \
           src/graphicsview/WResizer_p.h \

greaterThan(QT_MAJOR_VERSION, 5): HEADERS += src/graphicsview/WTextureVideo.h \
                                             src/graphicsview/WTextureVideo_p.h \

SOURCES += src/graphicsview/WAbstractView.cpp \
           src/graphicsview/WView.cpp \
           src/graphicsview/WViewResizer.cpp \
           src/graphicsview/WViewDrag.cpp \
           src/graphicsview/WWindow.cpp \
           src/graphicsview/WResizer.cpp \

greaterThan(QT_MAJOR_VERSION, 5): SOURCES += src/graphicsview/WTextureVideo.cpp \
