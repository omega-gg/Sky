TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = src/SkCore \
          src/SkGui \
          src/SkMedia \
          src/SkWeb \
          src/SkTorrent \
          src/SkBackend \
          tools \

OTHER_FILES += 3rdparty.sh \
               configure.sh \
               build.sh \
               deploy.sh \
               README.md \
               LICENSE.md \
               AUTHORS.md \
               src/SkyComponents/qmldir \
               src/SkyPresentation/qmldir \
               dist/changes/1.0.1.md \
               dist/changes/1.1.0.md \
               dist/changes/1.1.1.md \
               dist/changes/1.1.2.md \
               dist/changes/1.2.0.md \
               dist/changes/1.3.0.md \
               dist/changes/1.3.1.md \
