android:ABI = $$ANDROID_TARGET_ARCH

contains(QT_MAJOR_VERSION, 4) {
    QTX = Qt4
} else:contains(QT_MAJOR_VERSION, 5) {
    QTX = Qt5
} else {
    android {
        QTX = Qt6/$$ABI
    } else {
        QTX = Qt6
    }
}

android {
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

    macx:DEFINES += Q_OS_MACOS
}

greaterThan(QT_MAJOR_VERSION, 4) {
    DEFINES += QT_NEW
}

# NOTE Qt5: qmake won't let me write 'ios:android' in a condition with brackets.
ios {
    DEFINES += SK_MOBILE
} else:android {
    DEFINES += SK_MOBILE
} else {
    DEFINES += SK_DESKTOP
}

# FIXME Qt5.12.2: This can be useful when Q_OS_IOS is broken in certain scenarios.
ios:DEFINES += SK_OS_IOS

android:contains(ABI, arm64-v8a): DEFINES += CAN_COMPILE_NEON

# NOTE iOS: Sky projects should be prefixed with the omega domain.
ios:QMAKE_TARGET_BUNDLE_PREFIX = gg.omega
