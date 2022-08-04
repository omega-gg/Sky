TEMPLATE = subdirs

CONFIG += ordered

isEmpty(TOOLS) {
    SUBDIRS = src/SkCore \
              src/SkGui \
              src/SkMedia \
              src/SkBarcode \

    greaterThan(QT_MAJOR_VERSION, 4) {
        SUBDIRS += src/SkMultimedia
    }

    # NOTE Qt5 Windows: The WebView module is only available for MSVC but currently unsupported.
    # contains(QT_MAJOR_VERSION, 4) {
    #     SUBDIRS += src/SkWeb
    # }

    # NOTE iOS: Torrents are not available.
    !ios: SUBDIRS += src/SkTorrent

    SUBDIRS += src/SkBackend

    !ios:!android:SUBDIRS += tools
} else {
    !ios:!android:SUBDIRS = tools
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
               .appveyor.yml \
               include/generate.sh \
               src/SkyBase/qmldir \
               src/SkyBase/*.qml \
               src/SkyComponents/icons.sh \
               src/SkyComponents/qmldir \
               src/SkyComponents/*.qml \
               src/SkyTouch/icons.sh \
               src/SkyTouch/qmldir \
               src/SkyTouch/*.qml \
               src/SkyPresentation/qmldir \
               src/SkyPresentation/*.qml \
               dist/shaders/generate.sh \
               dist/shaders/video.vert \
               dist/shaders/video.frag \
               dist/iOS/WorkspaceSettings.xcsettings \
               dist/android/src/gg/omega/WActivity.java \
               dist/android/src/gg/omega/WFile.java \
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
               dist/changes/1.9.0.md \
