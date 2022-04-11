# libcharsetdetect module

INCLUDEPATH += src/3rdparty/libcharsetdetect \
               src/3rdparty/libcharsetdetect/base \
               src/3rdparty/libcharsetdetect/nspr-emu \

HEADERS += src/3rdparty/libcharsetdetect/charsetdetect.h \
           src/3rdparty/libcharsetdetect/charsetdetectPriv.h \
           src/3rdparty/libcharsetdetect/nscore.h \
           src/3rdparty/libcharsetdetect/base/CharDistribution.h \
           src/3rdparty/libcharsetdetect/base/JpCntx.h \
           src/3rdparty/libcharsetdetect/base/nsBig5Prober.h \
           src/3rdparty/libcharsetdetect/base/nsCharSetProber.h \
           src/3rdparty/libcharsetdetect/base/nsCodingStateMachine.h \
           src/3rdparty/libcharsetdetect/base/nsEscCharsetProber.h \
           src/3rdparty/libcharsetdetect/base/nsEUCJPProber.h \
           src/3rdparty/libcharsetdetect/base/nsEUCKRProber.h \
           src/3rdparty/libcharsetdetect/base/nsEUCTWProber.h \
           src/3rdparty/libcharsetdetect/base/nsGB2312Prober.h \
           src/3rdparty/libcharsetdetect/base/nsHebrewProber.h \
           src/3rdparty/libcharsetdetect/base/nsLatin1Prober.h \
           src/3rdparty/libcharsetdetect/base/nsMBCSGroupProber.h \
           src/3rdparty/libcharsetdetect/base/nsPkgInt.h \
           src/3rdparty/libcharsetdetect/base/nsSBCharSetProber.h \
           src/3rdparty/libcharsetdetect/base/nsSBCSGroupProber.h \
           src/3rdparty/libcharsetdetect/base/nsSJISProber.h \
           src/3rdparty/libcharsetdetect/base/nsUniversalDetector.h \
           src/3rdparty/libcharsetdetect/base/nsUTF8Prober.h \
           src/3rdparty/libcharsetdetect/nspr-emu/nsDebug.h \
           src/3rdparty/libcharsetdetect/nspr-emu/prcpucfg.h \
           src/3rdparty/libcharsetdetect/nspr-emu/prcpucfg_freebsd.h \
           src/3rdparty/libcharsetdetect/nspr-emu/prcpucfg_linux.h \
           src/3rdparty/libcharsetdetect/nspr-emu/prcpucfg_mac.h \
           src/3rdparty/libcharsetdetect/nspr-emu/prcpucfg_openbsd.h \
           src/3rdparty/libcharsetdetect/nspr-emu/prcpucfg_win.h \
           src/3rdparty/libcharsetdetect/nspr-emu/prmem.h \
           src/3rdparty/libcharsetdetect/nspr-emu/prtypes.h \
           src/3rdparty/libcharsetdetect/nspr-emu/obsolete/protypes.h \

SOURCES += src/3rdparty/libcharsetdetect/charsetdetect.cpp \
           src/3rdparty/libcharsetdetect/base/CharDistribution.cpp \
           src/3rdparty/libcharsetdetect/base/JpCntx.cpp \
           src/3rdparty/libcharsetdetect/base/LangBulgarianModel.cpp \
           src/3rdparty/libcharsetdetect/base/LangCyrillicModel.cpp \
           src/3rdparty/libcharsetdetect/base/LangGreekModel.cpp \
           src/3rdparty/libcharsetdetect/base/LangHebrewModel.cpp \
           src/3rdparty/libcharsetdetect/base/LangHungarianModel.cpp \
           src/3rdparty/libcharsetdetect/base/LangThaiModel.cpp \
           src/3rdparty/libcharsetdetect/base/nsBig5Prober.cpp \
           src/3rdparty/libcharsetdetect/base/nsCharSetProber.cpp \
           src/3rdparty/libcharsetdetect/base/nsEscCharsetProber.cpp \
           src/3rdparty/libcharsetdetect/base/nsEscSM.cpp \
           src/3rdparty/libcharsetdetect/base/nsEUCJPProber.cpp \
           src/3rdparty/libcharsetdetect/base/nsEUCKRProber.cpp \
           src/3rdparty/libcharsetdetect/base/nsEUCTWProber.cpp \
           src/3rdparty/libcharsetdetect/base/nsGB2312Prober.cpp \
           src/3rdparty/libcharsetdetect/base/nsHebrewProber.cpp \
           src/3rdparty/libcharsetdetect/base/nsLatin1Prober.cpp \
           src/3rdparty/libcharsetdetect/base/nsMBCSGroupProber.cpp \
           src/3rdparty/libcharsetdetect/base/nsMBCSSM.cpp \
           src/3rdparty/libcharsetdetect/base/nsSBCharSetProber.cpp \
           src/3rdparty/libcharsetdetect/base/nsSBCSGroupProber.cpp \
           src/3rdparty/libcharsetdetect/base/nsSJISProber.cpp \
           src/3rdparty/libcharsetdetect/base/nsUniversalDetector.cpp \
           src/3rdparty/libcharsetdetect/base/nsUTF8Prober.cpp \
