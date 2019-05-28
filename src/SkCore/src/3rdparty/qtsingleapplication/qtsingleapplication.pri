# Qt single application module

HEADERS += src/3rdparty/qtsingleapplication/qtlocalpeer.h \
           src/3rdparty/qtsingleapplication/qtlockedfile.h \
           src/3rdparty/qtsingleapplication/qtsinglecoreapplication.h \
           src/3rdparty/qtsingleapplication/qtsingleapplication.h \

SOURCES += src/3rdparty/qtsingleapplication/qtlocalpeer.cpp \
           src/3rdparty/qtsingleapplication/qtlockedfile.cpp \
           src/3rdparty/qtsingleapplication/qtlockedfile_unix.cpp \
           src/3rdparty/qtsingleapplication/qtlockedfile_win.cpp \
           src/3rdparty/qtsingleapplication/qtsinglecoreapplication.cpp \
           src/3rdparty/qtsingleapplication/qtsingleapplication.cpp \
