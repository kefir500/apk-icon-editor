QT += widgets xml network
TEMPLATE = app
CONFIG += c++11

TARGET = apk-icon-editor
VERSION = 2.1.0

QMAKE_TARGET_PRODUCT = APK Icon Editor
QMAKE_TARGET_COMPANY = Alexander Gorishnyak
QMAKE_TARGET_DESCRIPTION = APK Icon Editor v$$VERSION
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2014-2018

DEFINES += APP='"\\\"$$QMAKE_TARGET_PRODUCT\\\""'
DEFINES += VER=\\\"$$VERSION\\\"
#DEFINES += PORTABLE

TRANSLATIONS += $$PWD/../lang/apk-icon-editor.en.ts
RESOURCES    += $$PWD/../res/resources.qrc

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
LIBS += -lsimplecrypt
unix: LIBS += -lz

# Build:

win32 {
    DESTDIR = $$PWD/../bin/win32
    RC_ICONS = $$PWD/../res/icons/win32/icon.ico \
               $$PWD/../res/icons/win32/icon-apk.ico
}

unix:!macx {
    DESTDIR = $$PWD/../bin/linux/bin
}

macx {
    DESTDIR = $$PWD/../bin/macosx
    QMAKE_MAC_SDK = macosx10.7
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
}

MOC_DIR = $$PWD/../build/moc
RCC_DIR = $$PWD/../build/rcc
OBJECTS_DIR = $$PWD/../build/obj

# Deploy:

SHARED = $$DESTDIR
win32: SHARED ~= s,/,\\,g
unix:!macx: SHARED = $$DESTDIR/../share/apk-icon-editor/
unix:!macx: QMAKE_POST_LINK += mkdir -p $$quote($$SHARED) $$escape_expand(\\n\\t)
macx:SHARED = $$DESTDIR/apk-icon-editor.app/Contents/MacOS/

defineTest(deploy) {
    DIRS = $$1
    for(DIR, DIRS) {
        DST = $$SHARED
        unix {
            !macx: !equals(DIR, "general"): DST = $$DESTDIR
            macx: !equals(DIR, "general"): DST = $$SHARED/../
        }
        SRC = $$PWD/../deploy/$$DIR/.
        win32: SRC ~= s,/,\\,g
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$SRC) $$quote($$DST) $$escape_expand(\\n\\t)
    }
    export(QMAKE_POST_LINK)
}

win32: deploy(general win32)
unix:!macx: deploy(general linux)
macx: deploy(general macosx)

unix:!macx: QMAKE_POST_LINK += chmod +x $$quote($$DESTDIR/zipalign) $$escape_expand(\\n\\t)
macx:       QMAKE_POST_LINK += chmod +x $$quote($$SHARED/zipalign) $$escape_expand(\\n\\t)

# Install:

unix:!macx {
    isEmpty(PREFIX): PREFIX = /usr
    target.path   = $$PREFIX/bin
    share.files   = $$PWD/../deploy/general/*
    share.path    = $$PREFIX/share/apk-icon-editor
    icons.files   = $$PWD/../res/icons/linux/apk-icon-editor/*
    icons.path    = $$PREFIX/share/icons/hicolor
    desktop.files = $$PWD/../res/icons/linux/apk-icon-editor.desktop
    desktop.path  = $$PREFIX/share/applications
    INSTALLS      += target share icons desktop
    isEmpty(PACKAGE) {
        bin.files   = $$PWD/../deploy/linux/*
        bin.path    = $$PREFIX/bin
        INSTALLS    += bin
    }
}
