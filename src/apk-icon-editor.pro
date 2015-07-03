QT += core widgets network
TEMPLATE = app

VERSION = 1.4.0
QMAKE_TARGET_COMPANY = Qwerty Minds
QMAKE_TARGET_DESCRIPTION = APK Icon Editor v$$VERSION
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2014-2015

CONFIG(release, debug|release) {
    TARGET = apk-icon-editor
    QMAKE_TARGET_PRODUCT = APK Icon Editor
}

CONFIG(debug, debug|release) {
    TARGET = apk-icon-editor-debug
    QMAKE_TARGET_PRODUCT = APK Icon Editor (Debug)
}

DEFINES += APP='"\\\"$$QMAKE_TARGET_PRODUCT\\\""'
DEFINES += VER=\\\"$$VERSION\\\"

win32 {
    isEqual(QT_MAJOR_VERSION, 5) {
        greaterThan(QT_MINOR_VERSION, 1): QT += winextras
    } else:greaterThan(QT_MAJOR_VERSION, 5) {
        QT += winextras
    }
}

include(core/core.pri)
include(widgets/widgets.pri)
include(dialogs/dialogs.pri)

INCLUDEPATH += ../lib/include
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib

win32 {
    DESTDIR  = ../bin/win32
    RC_ICONS = ../res/icons/icon.ico \
               ../res/icons/icon-apk.ico
}

macx {
    DESTDIR = ../bin/macosx
    ICON    = ../res/icons/icon.icns
    QMAKE_MAC_SDK = macosx10.7
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    LIBS += -lz
}

unix:!macx {
    DESTDIR = ../bin/linux
}

LIBS += -L../lib/bin
LIBS += -lquazip \
        -lsimplecrypt

DEFINES += QUAZIP_STATIC

TRANSLATIONS += ../lang/apk-icon-editor.en.ts
RESOURCES    += ../res/resources.qrc
