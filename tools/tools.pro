TEMPLATE = subdirs

SUBDIRS = includeGenerator \
          deployer \
          projectGenerator \
          imageConverter \

include(ffmpeg/ffmpeg.pri)
include(youtube-dl/youtube-dl.pri)
include(Runway/Runway.pri)
include(Freepik/Freepik.pri)
include(fal/fal.pri)
include(Luma/Luma.pri)
include(LivePortrait/LivePortrait.pri)

# NOTE Qt5: We need an updated androiddeployqt with the latests changes.
contains(QT_VERSION, 5.15.2) SUBDIRS += androiddeployqt
