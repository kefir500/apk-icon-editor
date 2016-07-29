QT += widgets xml network
TEMPLATE = app

TARGET = apk-icon-editor
VERSION = 1.7.0

QMAKE_TARGET_PRODUCT = APK Icon Editor
QMAKE_TARGET_COMPANY = Alexander Gorishnyak
QMAKE_TARGET_DESCRIPTION = APK Icon Editor v$$VERSION
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2014-2016

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
include(apk/apk.pri)
include(widgets/widgets.pri)
include(dialogs/dialogs.pri)

INCLUDEPATH += $$PWD/../lib/include
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib

LIBS += -L$$PWD/../lib/bin
LIBS += -lquazip -lsimplecrypt
unix: LIBS += -lz

defineTest(deploy) {
    DIRS = $$1
    for(DIR, DIRS) {
        SRC = $$PWD/../deploy/$$DIR/.
        DST = $$DESTDIR
        macx:DST = $$DESTDIR/apk-icon-editor.app/Contents/MacOS/
        win32:SRC ~= s,/,\\,g
        win32:DST ~= s,/,\\,g
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$SRC) $$quote($$DST) $$escape_expand(\\n\\t)
    }
    export(QMAKE_POST_LINK)
}

win32 {
    DESTDIR  = $$PWD/../bin/win32
    RC_ICONS = $$PWD/../res/icons/icon.ico \
               $$PWD/../res/icons/icon-apk.ico
    deploy(general win32)
}

macx {
    DESTDIR = $$PWD/../bin/macosx
    ICON    = $$PWD/../res/icons/icon.icns
    QMAKE_MAC_SDK = macosx10.7
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    deploy(general macosx)
}

unix:!macx {
    DESTDIR = $$PWD/../bin/linux
    deploy(general linux)
}

unix {
    !macx:DST = $$DESTDIR
    macx:DST = $$DESTDIR/apk-icon-editor.app/Contents/MacOS/
    QMAKE_POST_LINK += chmod +x $$quote($$DST/aapt) && chmod +x $$quote($$DST/zipalign)
}

DEFINES += QUAZIP_STATIC

TRANSLATIONS += $$PWD/../lang/apk-icon-editor.en.ts
RESOURCES    += $$PWD/../res/resources.qrc

MOC_DIR = $$PWD/../build/moc
RCC_DIR = $$PWD/../build/rcc
OBJECTS_DIR = $$PWD/../build/obj
