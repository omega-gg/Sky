TEMPLATE = subdirs

SUBDIRS = src/SkCore \
          src/SkGui \
          src/SkMedia \
          src/SkWeb \
          src/SkBackend \
          tools \

OTHER_FILES += configure.sh \
               deploy.sh \
               README.md \
               LICENSE.md \
               src/SkyComponents/qmldir \
               dist/changes/1.0.1.md \
               dist/changes/1.1.0.md \
               dist/changes/1.1.1.md \
