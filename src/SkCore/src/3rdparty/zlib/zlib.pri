# zlib module

INCLUDEPATH += src/3rdparty/zlib \

HEADERS += src/3rdparty/zlib/crc32.h \
           src/3rdparty/zlib/deflate.h \
           src/3rdparty/zlib/gzguts.h \
           src/3rdparty/zlib/inffast.h \
           src/3rdparty/zlib/inffixed.h \
           src/3rdparty/zlib/inflate.h \
           src/3rdparty/zlib/inftrees.h \
           src/3rdparty/zlib/trees.h \
           src/3rdparty/zlib/zconf.h \
           src/3rdparty/zlib/zlib.h \
           src/3rdparty/zlib/zutil.h \

SOURCES += src/3rdparty/zlib/adler32.c \
           src/3rdparty/zlib/compress.c \
           src/3rdparty/zlib/crc32.c \
           src/3rdparty/zlib/deflate.c \
           src/3rdparty/zlib/gzclose.c \
           src/3rdparty/zlib/gzlib.c \
           src/3rdparty/zlib/gzread.c \
           src/3rdparty/zlib/gzwrite.c \
           src/3rdparty/zlib/infback.c \
           src/3rdparty/zlib/inffast.c \
           src/3rdparty/zlib/inflate.c \
           src/3rdparty/zlib/inftrees.c \
           src/3rdparty/zlib/trees.c \
           src/3rdparty/zlib/uncompr.c \
           src/3rdparty/zlib/zutil.c \
