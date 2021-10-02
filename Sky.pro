TEMPLATE = subdirs

CONFIG += ordered

isEmpty(TOOLS) {
    SUBDIRS = src/SkCore \
              src/SkGui \
              src/SkMedia \
              src/SkTorrent \
              src/SkBackend \

    contains(QT_MAJOR_VERSION, 4) {
        SUBDIRS += src/SkWeb
    } else {
        # NOTE Windows: The WebView module only works with MSVC.
        win32 {
            win32-msvc*:SUBDIRS += src/SkWeb
        } else {
            SUBDIRS += src/SkWeb
        }
    }

    !android:SUBDIRS += tools
} else {
    !android:SUBDIRS = tools
}

OTHER_FILES += 3rdparty.sh \
               configure.sh \
               build.sh \
               deploy.sh \
               environment.sh \
               README.md \
               LICENSE.md \
               AUTHORS.md \
               .azure-pipelines.yml \
               .travis.yml \
               .appveyor.yml \
               include/generate.sh \
               src/SkyBase/qmldir \
               src/SkyBase/*.qml \
               src/SkyComponents/qmldir \
               src/SkyComponents/*.qml \
               src/SkyTouch/qmldir \
               src/SkyTouch/*.qml \
               src/SkyPresentation/qmldir \
               src/SkyPresentation/*.qml \
               dist/android/src/gg/omega/WActivity.java \
               dist/changes/1.0.1.md \
               dist/changes/1.1.0.md \
               dist/changes/1.1.1.md \
               dist/changes/1.1.2.md \
               dist/changes/1.2.0.md \
               dist/changes/1.3.0.md \
               dist/changes/1.4.0.md \
               dist/changes/1.5.0.md \
               dist/changes/1.6.0.md \
               dist/changes/1.7.0.md \
               dist/changes/1.8.0.md \
