QT += core gui network

# TODO: Following expression won't work with Qt6.
equals(QT_MAJOR_VERSION, 5) {
    QT += widgets
    greaterThan(QT_MINOR_VERSION, 1): QT += winextras
}

TARGET = apk-icon-editor
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           dialogs.cpp \
           combolist.cpp \
           drawarea.cpp \
           effects.cpp \
           apk.cpp \
           icon.cpp \
           profile.cpp \
           updater.cpp \
           dropbox.cpp

HEADERS += main.h \
           mainwindow.h \
           dialogs.h \
           combolist.h \
           drawarea.h \
           effects.h \
           apk.h \
           icon.h \
           profile.h \
           updater.h \
           dropbox.h \
    keys.h

TRANSLATIONS += ../lang/en/apk-icon-editor.ts \
                ../lang/ru/apk-icon-editor.ts

RESOURCES += ../res/resources.qrc
RC_FILE += ../res/icon.rc

win32 {
    DESTDIR = ../bin/win32
}
