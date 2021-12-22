# Kernel module

HEADERS += src/kernel/WCoreApplication.h \
           src/kernel/WCrypter.h \
           src/kernel/WListId.h \
           src/kernel/WRegExp.h \

greaterThan(QT_MAJOR_VERSION, 5): HEADERS += src/kernel/WList.h \

SOURCES += src/kernel/WCoreApplication.cpp \
           src/kernel/WCrypter.cpp \
           src/kernel/WListId.cpp \
           src/kernel/WRegExp.cpp \
