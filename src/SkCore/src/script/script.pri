# Script module

# NOTE iOS: QProcess is not supported.

!ios:HEADERS += src/script/WScriptBash.h \
                src/script/WScriptBash_p.h \
                src/script/WBashManager.h \
                src/script/WBashManager_p.h \

!ios:SOURCES += src/script/WBashManager.cpp \
