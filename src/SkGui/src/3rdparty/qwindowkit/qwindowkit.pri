# qwindowkit module

HEADERS += src/3rdparty/qwindowkit/core/qwkconfig.h \
           src/3rdparty/qwindowkit/core/qwkglobal.h \
           src/3rdparty/qwindowkit/core/qwkglobal_p.h \
           src/3rdparty/qwindowkit/core/windowagentbase.h \
           src/3rdparty/qwindowkit/core/windowagentbase_p.h \
           src/3rdparty/qwindowkit/core/windowitemdelegate_p.h \
           src/3rdparty/qwindowkit/core/contexts/abstractwindowcontext_p.h \
           src/3rdparty/qwindowkit/core/contexts/qtwindowcontext_p.h \
           src/3rdparty/qwindowkit/core/kernel/nativeeventfilter_p.h \
           src/3rdparty/qwindowkit/core/kernel/sharedeventfilter_p.h \
           src/3rdparty/qwindowkit/core/kernel/winidchangeeventfilter_p.h \
           src/3rdparty/qwindowkit/core/shared/qwkwindowsextra_p.h \
           src/3rdparty/qwindowkit/core/shared/systemwindow_p.h \
           src/3rdparty/qwindowkit/quick/quickitemdelegate_p.h \
           src/3rdparty/qwindowkit/quick/quickwindowagent.h \
           src/3rdparty/qwindowkit/quick/quickwindowagent_p.h \
           src/3rdparty/qwindowkit/quick/qwkquickglobal.h \

win32:HEADERS += src/3rdparty/qwindowkit/core/qwindowkit_windows.h \
                 src/3rdparty/qwindowkit/core/contexts/win32windowcontext_p.h \
                 src/3rdparty/qwindowkit/core/shared/windows10borderhandler_p.h \

macx:HEADERS += src/3rdparty/qwindowkit/core/contexts/cocoawindowcontext_p.h \

unix:!macx:!android:HEADERS += src/3rdparty/qwindowkit/core/qwindowkit_linux.h \
                               src/3rdparty/qwindowkit/core/contexts/linuxwaylandcontext_p.h \
                               src/3rdparty/qwindowkit/core/contexts/linuxx11context_p.h \

SOURCES += src/3rdparty/qwindowkit/core/qwkglobal.cpp \
           src/3rdparty/qwindowkit/core/windowagentbase.cpp \
           src/3rdparty/qwindowkit/core/windowitemdelegate.cpp \
           src/3rdparty/qwindowkit/core/contexts/abstractwindowcontext.cpp \
           src/3rdparty/qwindowkit/core/contexts/qtwindowcontext.cpp \
           src/3rdparty/qwindowkit/core/kernel/nativeeventfilter.cpp \
           src/3rdparty/qwindowkit/core/kernel/sharedeventfilter.cpp \
           src/3rdparty/qwindowkit/core/kernel/winidchangeeventfilter.cpp \
           src/3rdparty/qwindowkit/quick/quickitemdelegate.cpp \
           src/3rdparty/qwindowkit/quick/quickwindowagent.cpp \
           src/3rdparty/qwindowkit/quick/qwkquickglobal.cpp \

win32:SOURCES += src/3rdparty/qwindowkit/core/qwindowkit_windows.cpp \
                 src/3rdparty/qwindowkit/core/contexts/win32windowcontext.cpp \
                 src/3rdparty/qwindowkit/quick/quickwindowagent_win.cpp \

macx:SOURCES += src/3rdparty/qwindowkit/core/contexts/cocoawindowcontext.mm \
                src/3rdparty/qwindowkit/quick/quickwindowagent_mac.cpp \

unix:!macx:!android:SOURCES += src/3rdparty/qwindowkit/core/qwindowkit_linux.cpp \
                               src/3rdparty/qwindowkit/core/contexts/linuxwaylandcontext.cpp \
                               src/3rdparty/qwindowkit/core/contexts/linuxx11context.cpp \
