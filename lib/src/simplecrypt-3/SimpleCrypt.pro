QT -= gui

TARGET = simplecrypt
TEMPLATE = lib
CONFIG += staticlib

SOURCES += simplecrypt.cpp
HEADERS += simplecrypt.h

# APK Icon Editor:

DESTDIR = $$PWD/../../bin

macx {
    QMAKE_MAC_SDK = macosx10.7
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
}
