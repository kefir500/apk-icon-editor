QT += core widgets network
TEMPLATE = app

VERSION = 1.2.0
QMAKE_TARGET_COMPANY = Qwerty Minds
QMAKE_TARGET_DESCRIPTION = A tool designed to easily edit and replace APK icons
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
    } else {
        QT += winextras
    }
}

SOURCES += main.cpp \
           mainwindow.cpp \
           dialogs.cpp \
           combolist.cpp \
           drawarea.cpp \
           filebox.cpp \
           effects.cpp \
           tooldialog.cpp \
           keymanager.cpp \
           apk.cpp \
           cloud.cpp \
           icon.cpp \
           profile.cpp \
           updater.cpp

HEADERS += main.h \
           mainwindow.h \
           dialogs.h \
           combolist.h \
           drawarea.h \
           filebox.h \
           effects.h \
           tooldialog.h \
           keymanager.h \
           apk.h \
           cloud.h \
           icon.h \
           profile.h \
           updater.h \
           keys.h

INCLUDEPATH += ../lib/include
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib

win32 {
    DESTDIR = ../bin/win32
    LIBS += -L../lib/bin/win32
    RC_ICONS = ../res/icons/icon.ico \
               ../res/icons/icon-apk.ico
}

macx {
    DESTDIR = ../bin/macosx
    LIBS += -L../lib/bin/macosx
}

unix:!macx {
    DESTDIR = ../bin/linux
    LIBS += -L../lib/bin/linux
}

LIBS += -lquazip \
        -lsimplecrypt

DEFINES += QUAZIP_STATIC

TRANSLATIONS += ../lang/apk-icon-editor.en.ts
RESOURCES += ../res/resources.qrc
