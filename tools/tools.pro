TEMPLATE = subdirs

SUBDIRS = includeGenerator \
          deployer \
          projectGenerator \

# NOTE Qt5: We need an updated androiddeployqt with the latests changes.
contains(QT_MAJOR_VERSION, 5) SUBDIRS += androiddeployqt
