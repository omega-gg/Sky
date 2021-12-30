contains(QT_MAJOR_VERSION, 4) {
    QTX = Qt4
} else:contains(QT_MAJOR_VERSION, 5) {
    QTX = Qt5
} else {
    QTX = Qt6
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
