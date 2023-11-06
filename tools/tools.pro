TEMPLATE = subdirs

SUBDIRS = includeGenerator \
          deployer \
          projectGenerator \

# NOTE: androiddeployqt is available since Qt5.
greaterThan(QT_MAJOR_VERSION, 4) SUBDIRS += androiddeployqt
