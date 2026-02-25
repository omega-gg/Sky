TEMPLATE = subdirs

SUBDIRS = deployer \
          includeGenerator \
          projectGenerator \
          qmlGenerator \
          imageConverter \

# NOTE Qt5: We need an updated androiddeployqt with the latests changes.
contains(QT_VERSION, 5.15.2) SUBDIRS += androiddeployqt
