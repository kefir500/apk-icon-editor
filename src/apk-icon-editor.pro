QT += core gui network

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

TARGET = apk-icon-editor
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           dialogs.cpp \
           combolist.cpp \
           drawarea.cpp \
           effects.cpp \
           tooldialog.cpp \
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
           effects.h \
           tooldialog.h \
           apk.h \
           cloud.h \
           icon.h \
           profile.h \
           updater.h \
           keys.h

TRANSLATIONS += ../lang/apk-icon-editor.en.ts \
                ../lang/apk-icon-editor.fr.ts \
                ../lang/apk-icon-editor.ru.ts \
                ../lang/apk-icon-editor.zh.ts

RESOURCES += ../res/resources.qrc
RC_FILE += ../res/icon.rc

win32 {
    DESTDIR = ../bin/win32
}

unix {
    DESTDIR = ../bin/unix
}
