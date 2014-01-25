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
           combolist.cpp \
           drawarea.cpp \
           apk.cpp \
           icon.cpp \
           profile.cpp \
           updater.cpp

HEADERS += main.h \
           mainwindow.h \
           combolist.h \
           drawarea.h \
           apk.h \
           icon.h \
           profile.h \
           updater.h

TRANSLATIONS += ../lang/ru/apk-icon-editor.ts

RESOURCES += ../res/resources.qrc
RC_FILE += ../res/icon.rc

win32 {
    DESTDIR = ../bin/win32
    LIBS += -lshell32 # To handle UAC
}
