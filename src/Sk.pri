contains(QT_MAJOR_VERSION, 4) {
    QTX = Qt4
} else:contains(QT_MAJOR_VERSION, 5) {
    QTX = Qt5
} else {
    android {
        QTX = Qt6/$$ANDROID_TARGET_ARCH
    } else {
        QTX = Qt6
    }
}

android {
    ABI = $$ANDROID_TARGET_ARCH

    ANDROID_LIB = $$_PRO_FILE_PWD_/dist/android/data/$$ABI/libs/$$ABI

    # NOTE Qt5: That's the only way to retrieve the proper path.
    contains(QT_MAJOR_VERSION, 5) {
        for (abi, ANDROID_ABIS) {
            ANDROID_PACKAGE = $$_PRO_FILE_PWD_/dist/android/data/$$abi
        }
    } else {
        ANDROID_PACKAGE = $$_PRO_FILE_PWD_/dist/android/data/$$ABI
    }
}

contains(QT_MAJOR_VERSION, 4) {
    DEFINES += QT_4
} else:contains(QT_MAJOR_VERSION, 5) {
    DEFINES += QT_5
} else {
    DEFINES += QT_6
}

lessThan(QT_MAJOR_VERSION, 6) {
    DEFINES += QT_OLD
}

greaterThan(QT_MAJOR_VERSION, 4) {
    DEFINES += QT_NEW
}

android {
    DEFINES += SK_MOBILE
} else {
    DEFINES += SK_DESKTOP
}
