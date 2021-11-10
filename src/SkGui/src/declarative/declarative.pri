# Declarative module

HEADERS += src/declarative/WDeclarativeApplication.h \
           src/declarative/WDeclarativeItem.h \
           src/declarative/WDeclarativeItem_p.h \
           src/declarative/WDeclarativeMouseArea.h \
           src/declarative/WDeclarativeMouseArea_p.h \
           src/declarative/WDeclarativeMouseWatcher.h \
           src/declarative/WDeclarativeMouseWatcher_p.h \
           src/declarative/WDeclarativeBorders.h \
           src/declarative/WDeclarativeImageBase.h \
           src/declarative/WDeclarativeImageBase_p.h \
           src/declarative/WDeclarativeImage.h \
           src/declarative/WDeclarativeImage_p.h \
           src/declarative/WDeclarativeImageSvg.h \
           src/declarative/WDeclarativeImageSvg_p.h \
           src/declarative/WDeclarativeBorderImage.h \
           src/declarative/WDeclarativeBorderImage_p.h \
           #src/declarative/WDeclarativeText.h \
           src/declarative/WDeclarativeTextSvg.h \
           src/declarative/WDeclarativeTextSvg_p.h \
           src/declarative/WDeclarativeAnimated.h \
           src/declarative/WDeclarativeAnimated_p.h \
           src/declarative/WDeclarativePlayer.h \
           src/declarative/WDeclarativePlayer_p.h \
           src/declarative/WDeclarativeNoise.h \
           src/declarative/WDeclarativeNoise_p.h \
           src/declarative/WDeclarativeListView.h \
           src/declarative/WDeclarativeListView_p.h \
           src/declarative/WDeclarativeContextualPage.h \

greaterThan(QT_MAJOR_VERSION, 4): HEADERS += src/declarative/WDeclarativeTexture.h \
                                             src/declarative/WDeclarativeTexture_p.h \
                                             src/declarative/WDeclarativeItemPaint.h \
                                             src/declarative/WDeclarativeItemPaint_p.h \

SOURCES += src/declarative/WDeclarativeApplication.cpp \
           src/declarative/WDeclarativeItem.cpp \
           src/declarative/WDeclarativeMouseArea.cpp \
           src/declarative/WDeclarativeMouseWatcher.cpp \
           src/declarative/WDeclarativeBorders.cpp \
           src/declarative/WDeclarativeImageBase.cpp \
           src/declarative/WDeclarativeImage.cpp \
           src/declarative/WDeclarativeImageSvg.cpp \
           src/declarative/WDeclarativeBorderImage.cpp \
           #src/declarative/WDeclarativeText.cpp \
           src/declarative/WDeclarativeTextSvg.cpp \
           src/declarative/WDeclarativeAnimated.cpp \
           src/declarative/WDeclarativePlayer.cpp \
           src/declarative/WDeclarativeNoise.cpp \
           src/declarative/WDeclarativeListView.cpp \
           src/declarative/WDeclarativeContextualPage.cpp \

greaterThan(QT_MAJOR_VERSION, 4): SOURCES += src/declarative/WDeclarativeTexture.cpp \
                                             src/declarative/WDeclarativeItemPaint.cpp \
