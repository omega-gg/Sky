# Script module

# NOTE iOS: QProcess is not supported.

!ios:HEADERS += src/script/WBashScript.h \
                src/script/WBashScript_p.h \
                src/script/WBashManager.h \
                src/script/WBashManager_p.h \

!ios:SOURCES += src/script/WBashScript.cpp \
                src/script/WBashManager.cpp \
