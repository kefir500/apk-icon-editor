QT += core network
TEMPLATE = app

CONFIG(release, debug|release) {
    TARGET = apk-icon-editor
    OBJECTS_DIR = ../build/release
    MOC_DIR = ../build/release
    RCC_DIR = ../build/release
}

CONFIG(debug, debug|release) {
    TARGET = apk-icon-editor-debug
    OBJECTS_DIR = ../build/debug
    MOC_DIR = ../build/debug
    RCC_DIR = ../build/debug
}

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    win32 {
        isEqual(QT_MAJOR_VERSION, 5) {
            greaterThan(QT_MINOR_VERSION, 1): QT += winextras
        } else {
            QT += winextras
        }
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
RC_FILE += ../res/icon.rc
